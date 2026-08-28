# ADA P1 - Fuerza bruta, diccionario y backtracking

Proyecto en C++17 para estudiar recuperacion de contrasenas representadas por
su hash SHA-256. Esta version contiene el modulo FB completo: enumeracion
exhaustiva, ataque por diccionario, comparacion de resultados, medicion en
microsegundos y pruebas automatizadas. Los archivos `bt_backtracking.*` quedan
reservados para el modulo BT del integrante responsable.

> El software se construyo con fines exclusivamente academicos. Debe usarse
> solo sobre instancias y credenciales autorizadas por sus propietarios.

## Estructura

```text
.
|-- src/
|   |-- main.cpp                 CLI del proyecto
|   |-- fb_bruteforce.cpp/.hpp   modulo FB y diccionario
|   |-- bt_backtracking.cpp/.hpp espacio reservado para BT
|   `-- third_party/picosha2.h   implementacion SHA-256 (licencia MIT)
|-- resources/diccionario.txt    diccionario reproducible de prueba
|-- tests/
|   |-- test_runner.cpp          pruebas automatizadas del modulo FB
|   `-- verificar_semilla.cpp    calculo de la semilla oficial
|-- results/                     resultados experimentales
`-- PSEUDOCODIGO_FB.md           especificacion formal y complejidad
```

## Requisitos

- Compilador compatible con C++17 (`g++` recomendado).
- PowerShell, Bash u otra terminal equivalente.
- No se necesitan dependencias binarias externas; PicoSHA2 esta incluido como
  encabezado y conserva su licencia MIT original.

## Compilacion con g++ -std=c++17 -O2

Desde la raiz del repositorio, en PowerShell:

```powershell
New-Item -ItemType Directory -Force build | Out-Null
g++ -std=c++17 -O2 -Wall -Wextra -pedantic src/main.cpp src/fb_bruteforce.cpp -o build/ada_p1.exe
g++ -std=c++17 -O2 -Wall -Wextra -pedantic -Isrc tests/test_runner.cpp src/fb_bruteforce.cpp -o build/test_runner.exe
g++ -std=c++17 -O2 -Wall -Wextra -pedantic tests/verificar_semilla.cpp -o build/verificar_semilla.exe
```

En Linux o macOS se usan los mismos comandos cambiando los nombres de salida
para omitir `.exe` si se desea.

## Ejecucion

La ayuda integrada muestra todos los subcomandos:

```powershell
.\build\ada_p1.exe --help
```

Calcular el hash de una instancia controlada:

```powershell
$hash = .\build\ada_p1.exe hash "3cab"
```

Ejecutar FB con el alfabeto `abc123` y longitud exacta 4:

```powershell
.\build\ada_p1.exe fb $hash "abc123" 4 4
```

Ejecutar el ataque por diccionario:

```powershell
.\build\ada_p1.exe diccionario $hash resources/diccionario.txt
```

Comparar ambos ataques con el mismo objetivo:

```powershell
.\build\ada_p1.exe comparar $hash "abc123" 4 4 resources/diccionario.txt
```

Cada ataque reporta `encontrada`, `contrasena`, `intentos` y `tiempo_us`. Un
ataque que agota su espacio sin encontrar la cadena termina con codigo 2; un
error de argumentos o de archivos termina con codigo 1.

## Pruebas automatizadas

Ejecutar siempre desde la raiz para que la ruta relativa del diccionario sea
reproducible:

```powershell
.\build\verificar_semilla.exe
.\build\test_runner.exe resources/diccionario.txt
```

Las pruebas cubren:

- vectores SHA-256 publicos para la cadena vacia y `abc`;
- instancia de referencia `cab`, encontrada en exactamente 32 intentos con
  alfabeto `abc` y longitudes 1 a 3;
- cinco contrasenas deterministas del equipo por FB y por diccionario;
- comparacion de ambos enfoques;
- validacion de hash, alfabeto, longitudes y archivo inexistente.

## Diseno y complejidad del modulo FB

El algoritmo interpreta cada candidato como un contador en base `|alfabeto|`.
Primero enumera todas las cadenas de longitud minima y continua hasta la
longitud maxima. Este orden es determinista y permite verificar el numero
exacto de intentos.

Para alfabeto de tamano `a` y longitudes entre `m` y `n`, el peor caso evalua
`a^m + a^(m+1) + ... + a^n` candidatos. Este crecimiento exponencial explica
el muro practico de fuerza bruta. El diccionario, en cambio, evalua a lo sumo
una vez cada una de sus `D` lineas, pero solo puede recuperar una contrasena
incluida en el archivo.

El pseudocodigo formal, el argumento de correctitud y el analisis completo se
encuentran en [PSEUDOCODIGO_FB.md](PSEUDOCODIGO_FB.md).

## API principal

El encabezado `src/fb_bruteforce.hpp` expone funciones independientes para:

- `sha256`;
- `calcular_espacio_busqueda`;
- `ataque_fuerza_bruta`;
- `ataque_diccionario`;
- `comparar_ataques`.

Esta separacion permite reutilizar el modulo desde la CLI, las pruebas y los
experimentos sin duplicar logica.

## Reproducibilidad y resultados

La semilla oficial se obtiene al ordenar los apellidos, concatenarlos, sumar
sus codigos ASCII y aplicar modulo 100000. El ejecutable
`verificar_semilla.cpp` documenta y verifica ese calculo. Los experimentos
posteriores deben registrar en `results/tiempos_fb.csv` el alfabeto, las
longitudes, el espacio de busqueda, los intentos y el tiempo medido, junto con
la identificacion del hardware y el compilador utilizado.

No se versionan ejecutables ni objetos de compilacion; cualquier persona puede
reconstruirlos desde cero con los comandos anteriores.

## Creditos de terceros

`src/third_party/picosha2.h` proviene del proyecto PicoSHA2 de okdshin y se
distribuye bajo licencia MIT. El aviso de copyright y el texto de la licencia
se conservan dentro del encabezado.
