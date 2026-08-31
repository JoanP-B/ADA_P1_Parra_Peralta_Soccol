# Práctica 1 — Fuerza Bruta y Backtracking (ADA P1)

## Integrantes del equipo

- **Parra El Masri, Juan Pablo**
- **Peralta Bedoya, Joan Stiven**
- **Soccol Mejia, Alessandro**

**Semilla oficial del equipo:** `3817`  
> **Cálculo de la semilla:** Se ordenaron alfabéticamente los apellidos de los integrantes (`parraelmasri`, `peraltabedoya`, `soccolmejia`), se concatenaron en minúsculas sin tildes ni espacios (`parraelmasriperaltabedoyasoccolmejia`), se sumaron los códigos ASCII de cada carácter (3817) y se aplicó la operación módulo $100000$, produciendo determinísticamente el valor `3817`.

---

## 1. Descripción General y Organización del Proyecto

Este proyecto implementa en C++17 dos módulos algorítmicos para el análisis y diseño de contraseñas bajo la función hash criptográfica SHA-256:
- **Módulo FB (Fuerza Bruta):** Implementa la enumeración exhaustiva sistemática del espacio $\Sigma^n$ con complejidad espacial $O(n)$ y lo compara contra un ataque heurístico por diccionario sobre listas sintéticas.
- **Módulo BT (Backtracking):** Modela el problema de generación de contraseñas como un Problema de Satisfacción de Restricciones (CSP), construyendo contraseñas carácter a carácter y aplicando poda temprana $O(1)$ por factibilidad de mínimos y restricciones locales de no repetición consecutiva.

El repositorio está organizado de forma modular: el código fuente se ubica en `src/` (incluyendo la biblioteca de cabecera `picosha2.h` en `src/third_party/`), los datos de entrada en `resources/`, las suites de validación y cálculo de semilla en `tests/`, los resultados experimentales (tablas `.csv` y gráficas `.png`) en `results/`, y el documento técnico formal en `report/Informe.pdf`.

> ⚠️ **Alcance ético:** Todo el software y los experimentos se desarrollaron con fines estrictamente académicos utilizando datos sintéticos generados por el propio equipo.

---

## 2. Estructura del Repositorio

```
.
├── report/
│   └── Informe.pdf                 Informe técnico formal (Sección 12)
├── src/
│   ├── main.cpp                    CLI interactiva principal
│   ├── fb_bruteforce.cpp/.hpp      Módulo de Fuerza Bruta y Diccionario
│   ├── bt_backtracking.cpp/.hpp    Módulo de Backtracking con poda O(1)
│   └── third_party/
│       └── picosha2.h              Biblioteca SHA-256 de cabecera única (Licencia MIT)
├── resources/
│   └── diccionario.txt             Diccionario sintético de 500 candidatos
├── tests/
│   ├── test_runner.cpp             Suite de pruebas automatizadas y exportación de CSVs
│   └── verificar_semilla.cpp       Utilidad de validación determinista de semilla
├── results/
│   ├── tiempos_fb.csv              Métricas experimentales de Fuerza Bruta
│   ├── tiempos_bt.csv              Métricas experimentales de Backtracking
│   ├── fb_crecimiento_exponencial.png Gráfica de crecimiento exponencial en FB
│   ├── fb_vs_diccionario.png       Gráfica comparativa FB vs. Diccionario
│   ├── bt_reduccion_poda_nodos.png Gráfica de reducción del árbol de búsqueda (BT)
│   └── generar_graficas.py         Script Python para procesar datos y generar gráficas
├── PSEUDOCODIGO_FB.md              Especificación formal y análisis teórico de FB
├── PSEUDOCODIGO_BT.md              Especificación formal y análisis teórico de BT
└── README.md                        Instrucciones de compilación, ejecución y documentación
```

---

## 3. Requisitos y Guía de Instalación Rápida

Para compilar y reproducir la totalidad de los experimentos se requiere:

### A. Compilador C++17 (GCC / G++ / MinGW-w64)
- **Windows:** 
  1. Descargar e instalar **MSYS2** desde [msys2.org](https://www.msys2.org/) o **w64devkit** desde GitHub.
  2. En la terminal MSYS2 UCRT64 ejecutar: `pacman -S mingw-w64-ucrt-x86_64-gcc`
  3. Agregar la ruta `C:\msys64\ucrt64\bin` a las Variables de Entorno del Sistema (`PATH`).
- **Linux (Ubuntu/Debian):** `sudo apt update && sudo apt install build-essential g++`
- **macOS:** `xcode-select --install` o `brew install gcc`

### B. Python 3.8+ y Librerías de Visualización
- Descargar Python desde [python.org](https://www.python.org/) asegurando marcar la opción **"Add python.exe to PATH"**.
- Instalar las dependencias ejecutando en la terminal:
  ```bash
  pip install pandas matplotlib
  ```

### C. Terminal (PowerShell / Bash)
- **Windows:** Presionar `Win + X` y seleccionar **PowerShell** o **Terminal Windows**.
- **Linux/macOS:** Abrir la aplicación **Terminal**.

---

## 4. Instalación y Compilación

### Paso 1: Clonar el repositorio

```bash
git clone https://github.com/JoanP-B/ADA_P1_Parra_Peralta_Soccol
cd ADA_P1_Parra_Peralta_Soccol
```

### Paso 2: Compilar el programa principal

```bash
g++ -std=c++17 -O2 -o ada_p1 src/*.cpp
```

> **Nota**: Si se compila archivo por archivo en entornos Windows/MinGW, se puede utilizar: 
> `g++ -std=c++17 -O2 -o ada_p1 src/*.cpp`

### Paso 3: Compilar la suite de pruebas automatizadas y utilidad de semilla

```bash
# Suite de recolección de experimentos
g++ -std=c++17 -O2 -o test_runner tests/test_runner.cpp src/fb_bruteforce.cpp src/bt_backtracking.cpp

# Utilidad para verificar la semilla del equipo
g++ -std=c++17 -O2 -o verificar_semilla tests/verificar_semilla.cpp
```

> **Nota**: En sistemas Windows, los archivos ejecutables generados tendrán extensión `.exe` automáticamente.

---

## 5. Ejecución y Reproducción de Experimentos

### A. Ejecución Rápida y Automatizada (Un único comando por módulo)

Para reproducir todos los experimentos de la Sección 9, generar las tablas CSV y las gráficas del informe:

**1. Ejecutar las pruebas completas de FB y BT:**
```bash
./test_runner
```
*(Valida las instancias de referencia de FB y BT, resuelve las 5 contraseñas del equipo más dos configuraciones de calibración de FB, y evalúa las 5 variantes oficiales de BT con y sin poda).*

**2. Generar todas las figuras y gráficas del informe técnico:**
```bash
python results/generar_graficas.py
```

---

### B. Pruebas Interactivas y Casos Específicos (CLI Principal)

El ejecutable `ada_p1` permite probar cualquier instancia individual o comparar estrategias:

#### 1. Módulo FB (Fuerza Bruta)
- **Instancia de referencia común (`abc12`):**
  ```bash
  ./ada_p1 fb 8d51feb34e3e69f6fa6dffc577e2c60490cf9a7fcd835f9f6af1505b71d74773 abcdefghijklmnopqrstuvwxyz0123456789 5 5
  ```
- **Comparación directa: Fuerza Bruta vs. Diccionario:**
  ```bash
  ./ada_p1 comparar 8d51feb34e3e69f6fa6dffc577e2c60490cf9a7fcd835f9f6af1505b71d74773 abcdefghijklmnopqrstuvwxyz0123456789 5 5 resources/diccionario.txt
  ```

#### 2. Módulo BT (Backtracking)
- **Política completa del equipo con poda (`n=8, min_lower=3, min_upper=2, min_digit=2, min_symbol=1`):**
  ```bash
  ./ada_p1 bt 8 3 2 2 1 1
  ```
- **Comparación: Backtracking con poda vs. sin poda:**
  ```bash
  ./ada_p1 bt-comparar 4 0 0 0 0
  ```

#### 3. Verificación Determinista de Semilla
```bash
./verificar_semilla
```

---

## 6. Metodología de medición BT

Para conservar las cinco configuraciones obligatorias de BT sin ocultar datos ni usar tiempos simulados, el motor agrupa prefijos equivalentes por sus contadores de categoría, la categoría del último carácter y la presencia de repeticiones consecutivas. Las multiplicidades calculan exactamente los nodos del árbol lógico de 69 símbolos y sus soluciones, tanto con poda como sin poda. Esta técnica permite medir las instancias de `n=8` y `n=10` de forma reproducible; `PSEUDOCODIGO_BT.md` documenta la recurrencia y sus límites.

## 7. Créditos y Licencias

- El archivo `src/third_party/picosha2.h` proviene del proyecto [PicoSHA2](https://github.com/okdshin/PicoSHA2) de okdshin y se distribuye bajo licencia de código abierto MIT.
- Todos los algoritmos, funciones de factibilidad, controladores y generadores de datos experimentales fueron diseñados e implementados por los integrantes del equipo para el curso de Análisis y Diseño de Algoritmos (Universidad EAFIT).
