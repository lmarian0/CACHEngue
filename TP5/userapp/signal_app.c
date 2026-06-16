/*
 * signal_app.c - Aplicación de usuario para leer el CDD y servir
 *                los datos vía HTTP con gráfico en tiempo real.
 *
 * Uso:
 *   ./signal_app [canal] [puerto_http]
 *     canal      : 0 = senoidal (default), 1 = cuadrada
 *     puerto_http: 8080 (default)
 *
 * Desde el navegador del host (o de la red):
 *   http://<IP_QEMU>:8080
 *
 * El programa:
 *   1. Abre /dev/signaldev
 *   2. Selecciona el canal vía ioctl
 *   3. Samplea la señal cada 1 segundo en un hilo
 *   4. Sirve una página web con gráfico en tiempo real (Chart.js vía CDN)
 *   5. Expone /data como endpoint JSON  →  { "value": 42, "time": "12:34:56",
 *                                            "channel": 0, "label": "Senoidal" }
 *   6. Acepta POST /channel con body "ch=0" o "ch=1" para cambiar canal
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/* ---------- Constantes ---------- */
#define DEVICE_PATH   "/dev/signaldev"
#define BUF_SIZE      64
#define HTTP_BUF      4096
#define MAX_SAMPLES   120   /* últimas 120 muestras en memoria */

/* ---------- Estado compartido entre hilos ---------- */
typedef struct {
    int    values[MAX_SAMPLES];
    char   times[MAX_SAMPLES][16];  /* "HH:MM:SS" */
    int    head;        /* índice del próximo slot a escribir */
    int    count;       /* muestras almacenadas */
    int    channel;     /* 0=senoidal, 1=cuadrada */
    int    fd_dev;      /* file descriptor del CDD */
    pthread_mutex_t lock;
    volatile int running;
} AppState;

static AppState g_state;

/* ---------- Hilo de muestreo ---------- */
static void *sampler_thread(void *arg)
{
    AppState *s = (AppState *)arg;
    char buf[BUF_SIZE];
    struct timespec ts = {1, 0};  /* sleep 1 segundo */

    while (s->running) {
        /* Abrir, leer y cerrar en cada muestra */
        int fd_tmp = open(DEVICE_PATH, O_RDONLY);
        if (fd_tmp < 0) {
            fprintf(stderr, "Error abriendo dispositivo: %s\n", strerror(errno));
            nanosleep(&ts, NULL);
            continue;
        }
        ssize_t n = read(fd_tmp, buf, sizeof(buf) - 1);
        close(fd_tmp);
        if (n <= 0) {
            fprintf(stderr, "Error leyendo /dev/signaldev: %s\n", strerror(errno));
            nanosleep(&ts, NULL);
            continue;
        }
        buf[n] = '\0';
        int val = atoi(buf);

        /* Timestamp */
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        char timestr[16];
        strftime(timestr, sizeof(timestr), "%H:%M:%S", tm_info);

        /* Guardar muestra */
        pthread_mutex_lock(&s->lock);
        s->values[s->head] = val;
        strncpy(s->times[s->head], timestr, 15);
        s->head  = (s->head + 1) % MAX_SAMPLES;
        if (s->count < MAX_SAMPLES) s->count++;
        pthread_mutex_unlock(&s->lock);

        nanosleep(&ts, NULL);
    }
    return NULL;
}

/* ---------- Construye JSON con las muestras ---------- */
static void build_json(AppState *s, char *out, size_t max_len)
{
    const char *label = (s->channel == 0) ? "Senoidal" : "Cuadrada";
    const char *unit  = "Unidades";   /* adimensional en la simulación */

    pthread_mutex_lock(&s->lock);

    /* Construir arrays de valores y tiempos ordenados */
    int n     = s->count;
    int start = (s->count < MAX_SAMPLES) ? 0 : s->head;

    char vals[MAX_SAMPLES * 8]  = "[";
    char labs[MAX_SAMPLES * 20] = "[";

    for (int i = 0; i < n; i++) {
        int idx = (start + i) % MAX_SAMPLES;
        char tmp[32];
        snprintf(tmp, sizeof(tmp), "%d", s->values[idx]);
        strcat(vals, tmp);
        if (i < n - 1) strcat(vals, ",");

        strcat(labs, "\"");
        strcat(labs, s->times[idx]);
        strcat(labs, "\"");
        if (i < n - 1) strcat(labs, ",");
    }
    strcat(vals, "]");
    strcat(labs, "]");

    /* Último valor */
    int last_val = (n > 0) ?
        s->values[(s->head - 1 + MAX_SAMPLES) % MAX_SAMPLES] : 0;
    char last_time[16] = "N/A";
    if (n > 0)
        strncpy(last_time,
                s->times[(s->head - 1 + MAX_SAMPLES) % MAX_SAMPLES], 15);

    pthread_mutex_unlock(&s->lock);

    snprintf(out, max_len,
        "{"
        "\"value\":%d,"
        "\"time\":\"%s\","
        "\"channel\":%d,"
        "\"label\":\"%s\","
        "\"unit\":\"%s\","
        "\"values\":%s,"
        "\"times\":%s"
        "}",
        last_val, last_time, s->channel, label, unit, vals, labs);
}

/* ---------- Página HTML embebida ---------- */
static const char *HTML_PAGE =
"<!DOCTYPE html>\n"
"<html lang='es'>\n"
"<head>\n"
"  <meta charset='UTF-8'>\n"
"  <meta name='viewport' content='width=device-width, initial-scale=1'>\n"
"  <title>Monitor de Señales - CDD</title>\n"
"  <script src='https://cdn.jsdelivr.net/npm/chart.js'></script>\n"
"  <style>\n"
"    body { font-family: Arial, sans-serif; background:#1a1a2e; color:#eee; margin:0; padding:20px; }\n"
"    h1   { color:#e94560; text-align:center; }\n"
"    .info{ text-align:center; margin:10px 0; font-size:1.1em; }\n"
"    .badge{ display:inline-block; background:#e94560; color:#fff;\n"
"            padding:4px 12px; border-radius:12px; font-weight:bold; }\n"
"    .chart-container { background:#16213e; border-radius:12px;\n"
"                       padding:20px; margin:20px auto; max-width:900px; }\n"
"    .controls { text-align:center; margin:15px; }\n"
"    button { background:#e94560; color:#fff; border:none; padding:10px 24px;\n"
"             border-radius:8px; cursor:pointer; font-size:1em; margin:5px; }\n"
"    button:hover { background:#c73652; }\n"
"    button.active { background:#0f3460; border:2px solid #e94560; }\n"
"    #lastval { font-size:2em; font-weight:bold; color:#e94560; }\n"
"  </style>\n"
"</head>\n"
"<body>\n"
"  <h1>&#128268; Monitor de Se&#241;ales</h1>\n"
"  <div class='info'>\n"
"    Canal activo: <span class='badge' id='channel-label'>...</span>\n"
"    &nbsp;&nbsp; Ultimo valor: <span id='lastval'>-</span>\n"
"    <span id='unit-label'></span>\n"
"  </div>\n"
"  <div class='controls'>\n"
"    <button id='btn0' onclick='setChannel(0)'>Se&#241;al 0: Senoidal</button>\n"
"    <button id='btn1' onclick='setChannel(1)'>Se&#241;al 1: Cuadrada</button>\n"
"  </div>\n"
"  <div class='chart-container'>\n"
"    <canvas id='myChart'></canvas>\n"
"  </div>\n"
"  <script>\n"
"    let currentChannel = -1;\n"
"    const ctx = document.getElementById('myChart').getContext('2d');\n"
"    const chart = new Chart(ctx, {\n"
"      type: 'line',\n"
"      data: { labels: [], datasets: [{\n"
"        label: 'Señal',\n"
"        data: [],\n"
"        borderColor: '#e94560',\n"
"        backgroundColor: 'rgba(233,69,96,0.1)',\n"
"        tension: 0.3,\n"
"        pointRadius: 3,\n"
"        fill: true\n"
"      }]},\n"
"      options: {\n"
"        animation: false,\n"
"        responsive: true,\n"
"        scales: {\n"
"          x: { title: { display:true, text:'Tiempo', color:'#aaa' },\n"
"               ticks: { color:'#aaa', maxTicksLimit: 15 },\n"
"               grid: { color:'#333' } },\n"
"          y: { title: { display:true, text:'Amplitud', color:'#aaa' },\n"
"               ticks: { color:'#aaa' },\n"
"               grid: { color:'#333' } }\n"
"        },\n"
"        plugins: { legend: { labels: { color:'#eee' } } }\n"
"      }\n"
"    });\n"
"\n"
"    function setChannel(ch) {\n"
"      fetch('/channel', {\n"
"        method: 'POST',\n"
"        headers: {'Content-Type':'application/x-www-form-urlencoded'},\n"
"        body: 'ch=' + ch\n"
"      }).then(() => { currentChannel = ch; updateButtons(ch); });\n"
"    }\n"
"\n"
"    function updateButtons(ch) {\n"
"      document.getElementById('btn0').classList.toggle('active', ch === 0);\n"
"      document.getElementById('btn1').classList.toggle('active', ch === 1);\n"
"    }\n"
"\n"
"    function fetchData() {\n"
"      fetch('/data')\n"
"        .then(r => r.json())\n"
"        .then(d => {\n"
"          // Detectar cambio de canal → resetear gráfico\n"
"          if (d.channel !== currentChannel) {\n"
"            currentChannel = d.channel;\n"
"            chart.data.labels = [];\n"
"            chart.data.datasets[0].data = [];\n"
"            chart.data.datasets[0].label = d.label + ' [' + d.unit + ']';\n"
"            document.getElementById('unit-label').textContent = ' ' + d.unit;\n"
"            updateButtons(d.channel);\n"
"          }\n"
"          // Actualizar con todos los datos históricos\n"
"          chart.data.labels = d.times;\n"
"          chart.data.datasets[0].data = d.values;\n"
"          chart.update();\n"
"          document.getElementById('channel-label').textContent = d.label;\n"
"          document.getElementById('lastval').textContent = d.value;\n"
"        })\n"
"        .catch(e => console.error(e));\n"
"    }\n"
"\n"
"    // Polling cada 1 segundo\n"
"    fetchData();\n"
"    setInterval(fetchData, 1000);\n"
"  </script>\n"
"</body>\n"
"</html>\n";

/* ---------- Manejo simplificado de HTTP ---------- */
static void handle_request(int client_fd, AppState *s)
{
    char req[HTTP_BUF] = {0};
    read(client_fd, req, sizeof(req) - 1);

    if (strncmp(req, "GET / ", 6) == 0 ||
        strncmp(req, "GET /index", 10) == 0) {
        /* Servir página HTML */
        char header[256];
        snprintf(header, sizeof(header),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "Content-Length: %zu\r\n"
            "Connection: close\r\n\r\n",
            strlen(HTML_PAGE));
        write(client_fd, header, strlen(header));
        write(client_fd, HTML_PAGE, strlen(HTML_PAGE));

    } else if (strncmp(req, "GET /data", 9) == 0) {
        /* JSON con muestras */
        char json[MAX_SAMPLES * 30 + 256];
        build_json(s, json, sizeof(json));
        char header[256];
        snprintf(header, sizeof(header),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %zu\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Connection: close\r\n\r\n",
            strlen(json));
        write(client_fd, header, strlen(header));
        write(client_fd, json, strlen(json));

}    else if (strncmp(req, "POST /channel", 13) == 0) {
        char *body = strstr(req, "\r\n\r\n");
        if (body) {
            body += 4;
            int ch = -1;
            if (strncmp(body, "ch=0", 4) == 0) ch = 0;
            else if (strncmp(body, "ch=1", 4) == 0) ch = 1;
            if (ch >= 0) {
                pthread_mutex_lock(&s->lock);
                s->channel = ch;
                s->head  = 0;
                s->count = 0;
                pthread_mutex_unlock(&s->lock);
                fprintf(stdout, "Nota: cambio de canal solo visual, reiniciar módulo para efecto real\n");
            }
        }
        const char *resp = "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK";
        write(client_fd, resp, strlen(resp));

    } else {
        const char *resp = "HTTP/1.1 404 Not Found\r\nContent-Length: 9\r\n\r\nNot Found";
        write(client_fd, resp, strlen(resp));
    }

    close(client_fd);
}

/* ---------- Servidor HTTP (hilo principal) ---------- */
static void run_http_server(AppState *s, int port)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); exit(1); }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family      = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port        = htons(port)
    };
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind"); exit(1);
    }
    listen(server_fd, 10);

    printf("Servidor HTTP escuchando en http://0.0.0.0:%d\n", port);
    printf("Abrí tu navegador y entrá a  http://<IP_QEMU>:%d\n", port);

    while (s->running) {
        int client = accept(server_fd, NULL, NULL);
        if (client >= 0)
            handle_request(client, s);
    }
    close(server_fd);
}

/* ---------- Señal SIGINT para salir limpiamente ---------- */
static volatile int g_stop = 0;
static void sig_handler(int sig) { (void)sig; g_stop = 1; g_state.running = 0; }

/* ---------- main ---------- */
int main(int argc, char *argv[])
{
    int channel   = (argc >= 2) ? atoi(argv[1]) : 0;
    int http_port = (argc >= 3) ? atoi(argv[2]) : 8080;

    if (channel != 0 && channel != 1) {
        fprintf(stderr, "Canal inválido. Use 0 (senoidal) o 1 (cuadrada).\n");
        return 1;
    }

    /* Abrir dispositivo */
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Error abriendo %s: %s\n", DEVICE_PATH, strerror(errno));
        fprintf(stderr, "¿Está cargado el módulo? → sudo insmod signal_driver.ko\n");
        return 1;
    }

    printf("Canal inicial: %d (%s)\n", channel,
           channel == 0 ? "Senoidal" : "Cuadrada");

    /* Inicializar estado */
    memset(&g_state, 0, sizeof(g_state));
    g_state.fd_dev  = fd;
    g_state.channel = channel;
    g_state.running = 1;
    pthread_mutex_init(&g_state.lock, NULL);

    /* Señal de interrupción */
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    /* Hilo de muestreo */
    pthread_t sampler;
    pthread_create(&sampler, NULL, sampler_thread, &g_state);

    /* Servidor HTTP (bloqueante) */
    run_http_server(&g_state, http_port);

    /* Limpieza */
    pthread_join(sampler, NULL);
    pthread_mutex_destroy(&g_state.lock);
    close(fd);
    printf("\nAplicación terminada.\n");
    return 0;
}
