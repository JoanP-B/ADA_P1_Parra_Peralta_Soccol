# Práctica 1 — Fuerza Bruta y Backtracking (ADA P1)

## Integrantes del equipo

- Parra El Masri, [Nombre]
- Peralta Bedoya, [Nombre]
- Soccol Mejia, [Nombre]

**Semilla oficial del equipo:** `3772`

> Calculada ordenando alfabéticamente los apellidos, concatenándolos, sumando sus códigos ASCII y aplicando módulo 100000.

---

## 1. Descripción General

Este proyecto en C++17 implementa dos módulos algorítmicos para el análisis de contraseñas mediante hashes SHA-256:

- **Módulo FB (Fuerza Bruta)**: Explora la enumeración exhaustiva y ataques por diccionario para cuantificar la intratabilidad del espacio de búsqueda.
- **Módulo BT (Backtracking)**: Construye incrementalmente contraseñas válidas bajo políticas restrictivas, demostrando la reducción masiva del espacio explorado mediante la técnica de poda por factibilidad.

> ⚠️ El software se construyó con fines exclusivamente académicos.

## 2. Estructura del Repositorio

```
.
├── src/
│   ├── main.cpp                    CLI principal interactiva
│   ├── fb_bruteforce.cpp/.hpp      Módulo de Fuerza Bruta y Diccionario
│   ├── bt_backtracking.cpp/.hpp    Módulo de Backtracking con poda
│   └── third_party/picosha2.h      Implementación SHA-256 (Licencia MIT)
├── resources/
│   └── diccionario.txt             Diccionario sintético de prueba
├── tests/
│   ├── test_runner.cpp             Suite de recolección de datos y CSVs
│   └── verificar_semilla.cpp       Verificación determinista de la semilla
├── results/
│   ├── tiempos_fb.csv              Datos crudos - Fuerza Bruta
│   ├── tiempos_bt.csv              Datos crudos - Backtracking
│   └── generar_graficas.py         Script para generar visualizaciones
├── PSEUDOCODIGO_FB.md              Especificaciones y complejidad - FB
├── PSEUDOCODIGO_BT.md              Especificaciones y complejidad - BT
└── README.md                        Este archivo
```

## 3. Requisitos

- **Compilador**: GCC/G++ o Clang compatible con el estándar **C++17**
- **Python**: 3.8 o superior (librerías: `pandas`, `matplotlib`)
- **Terminal**: PowerShell, Bash u otra equivalente

## 4. Instalación y Compilación

### Paso 1: Clonar el repositorio

```bash
git clone <URL_DEL_REPOSITORIO>
cd ADA_P1_Parra_Peralta_Soccol
```

### Paso 2: Compilar el programa principal

```bash
g++ -std=c++17 -O2 -o ada_p1 src/*.cpp
```

> **Nota**: Se ajustó el comando de la guía original para evitar el error de múltiple definición de `main`.

### Paso 3: Compilar la suite de pruebas

```bash
g++ -std=c++17 -O2 -o test_runner tests/test_runner.cpp src/fb_bruteforce.cpp src/bt_backtracking.cpp
```

> **Nota**: En Windows, los archivos generados automáticamente tendrán extensión `.exe`.

## 5. Ejecución y Reproducción de Experimentos

### A. Pruebas Interactivas (CLI principal)

Para reproducir manualmente cada instancia interactiva:

#### Módulo FB (Fuerza Bruta)

**Ejecutar ataque por fuerza bruta** con el alfabeto base para la contraseña de referencia (`abc12` → hash `8d51f...`):

```bash
./ada_p1 fb 8d51feb34e3e69f6fa6dffc577e2c60490cf9a7fcd835f9f6af1505b71d74773 abcdefghijklmnopqrstuvwxyz0123456789 5 5
```

**Comparar exhaustivamente contra el diccionario:**

```bash
./ada_p1 comparar 8d51feb34e3e69f6fa6dffc577e2c60490cf9a7fcd835f9f6af1505b71d74773 abcdefghijklmnopqrstuvwxyz0123456789 5 5 resources/diccionario.txt
```

#### Módulo BT (Backtracking)

**Construir contraseñas** bajo la política del equipo (n=8, min_lower=3, min_upper=1, min_digit=2, min_symbol=1) **CON PODA activada** (1):

```bash
./ada_p1 bt 8 3 1 2 1 1
```

### B. Pruebas Automatizadas y Generación de Gráficas

Para reproducir completamente las métricas del informe (tiempos, intentos, y porcentajes de poda):

**1. Generar los archivos CSV** de resultados:

```bash
./test_runner
```

Esto genera `tiempos_fb.csv` y `tiempos_bt.csv` en la carpeta `results/`.

**2. Procesar datos y generar visualizaciones:**

```bash
python results/generar_graficas.py
```

## 6. Créditos de terceros

El archivo `src/third_party/picosha2.h` proviene del proyecto [PicoSHA2](https://github.com/okdshin/PicoSHA2) de okdshin y se distribuye bajo licencia MIT.

> El aviso de copyright original se conserva dentro del encabezado del archivo.