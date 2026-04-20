# Tests

Carpeta de pruebas unitarias e integración para el TP2.

## Ejecutar todo

```bash
cd /home/mariano/Documentos/Mariano/Universidad/QuintoAño/PrimerCuatrimestre/SistemasDeComputacion/CACHEngue/TP2/tests
make
```

## Ejecutar por partes

```bash
make test-python
make test-c
make test-integration
```

## Cobertura

- `python/test_fetch_gini.py`: pruebas unitarias de la capa Python.
- `python/integration_all_layers.py`: integración de Python + C + ASM.
- `c/test_float_to_int.c`: prueba unitaria de `float_to_int`.
- `c/test_gini_plus_one.c`: prueba unitaria de `gini_plus_one`.
- `c/test_process_gini.c`: prueba unitaria de la función C `process_gini`.
