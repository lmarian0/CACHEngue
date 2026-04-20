import requests
import ctypes

def apiRest():
    res = requests.get('https://api.worldbank.org/v2/en/country/all/indicator/SI.POV.GINI?format=json&date=2011:2020&per_page=32500&page=1&country=%22Argentina%22')

    if res:
        print('Response OK')
    else:
        print('Response Failed')
    print(res.status_code)
    print(res)
    data = res.json()

    return data

def get_gini_data():
    resultados = []
    data = apiRest()
    lista_registros = data[1] # La posicion 0 es metadata
    paises_procesados = set()

    
    for registro in lista_registros:
        valor = registro.get('value')
        fecha = registro.get('date')
        pais = registro.get('country')
        pais = pais.get('value')

        if valor is not None and pais not in paises_procesados:
            paises_procesados.add(pais)
            resultados.append((pais, fecha, valor))
    
    return resultados

def main():
    datos = get_gini_data()
    
    ruta_libreria = "./libgini.so" 
    
    # 1. Cargamos la librería en la memoria de Python
    lib = ctypes.CDLL(ruta_libreria)
    
    # 2. Recibe un double (c_double) y devuelve un int (c_int).
    lib.process_gini.argtypes = [ctypes.c_double]
    lib.process_gini.restype = ctypes.c_int
    
    # 3. Iteramos nuestros datos y convocamos a C por cada uno
    print("--- INICIANDO PROCESAMIENTO EN CAPA 2 (C) ---")
    for pais, fecha, valor in datos:
        resultado_c = lib.process_gini(valor)
        
        # Opcional: imprimir solo cuando el país empiece con "A" para no saturar la consola
        print(f"País: {pais} | Original: {valor} | C devuelve: {resultado_c}")

if __name__ == "__main__":
    main()



