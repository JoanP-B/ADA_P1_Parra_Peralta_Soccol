# Pseudocodigo formal del modulo FB

## Enumeracion exhaustiva por fuerza bruta

**Entrada:** hash objetivo `H`, alfabeto ordenado `A`, longitud minima `m` y
longitud maxima `n`.

**Salida:** tupla `(encontrada, contrasena, intentos, tiempo)`.

```text
ALGORITMO FuerzaBrutaSHA256(H, A, m, n)
    VALIDAR que H sea un SHA-256 hexadecimal de 64 simbolos
    VALIDAR que A no este vacio y no tenga simbolos repetidos
    VALIDAR 1 <= m <= n

    intentos <- 0
    inicio <- RelojMonotono()

    PARA longitud <- m HASTA n HACER
        digitos <- vector de longitud posiciones inicializadas en 0
        candidato <- cadena de longitud copias de A[0]

        REPETIR
            intentos <- intentos + 1
            SI SHA256(candidato) = H ENTONCES
                tiempo <- Microsegundos(RelojMonotono() - inicio)
                RETORNAR (VERDADERO, candidato, intentos, tiempo)
            FIN SI
        HASTA QUE IncrementarBaseA(digitos, candidato, A) = FALSO
    FIN PARA

    tiempo <- Microsegundos(RelojMonotono() - inicio)
    RETORNAR (FALSO, cadena_vacia, intentos, tiempo)
FIN ALGORITMO
```

```text
SUBALGORITMO IncrementarBaseA(digitos, candidato, A)
    PARA posicion <- longitud(digitos) - 1 HASTA 0 PASO -1 HACER
        SI digitos[posicion] + 1 < longitud(A) ENTONCES
            digitos[posicion] <- digitos[posicion] + 1
            candidato[posicion] <- A[digitos[posicion]]
            RETORNAR VERDADERO
        FIN SI

        digitos[posicion] <- 0
        candidato[posicion] <- A[0]
    FIN PARA
    RETORNAR FALSO
FIN SUBALGORITMO
```

El vector `digitos` funciona como un contador en base `|A|`. Por ello, para
cada longitud se genera cada cadena exactamente una vez. Como se recorren
todas las longitudes entre `m` y `n`, si la contrasena pertenece al espacio de
busqueda el algoritmo termina encontrandola; si no pertenece, agota el espacio
y reporta que no fue encontrada.

El numero de candidatos en el peor caso es:

```text
T(m,n) = sumatoria desde k=m hasta n de |A|^k
```

Cada candidato exige un SHA-256 sobre `k` caracteres. Considerando el hash
como costo lineal en la longitud, el tiempo es
`O(sumatoria |A|^k * k)`; para una longitud maxima acotada suele expresarse
como `O(|A|^n)`. El espacio auxiliar es `O(n)`.

## Ataque por diccionario

```text
ALGORITMO AtaqueDiccionarioSHA256(H, ruta)
    VALIDAR que H sea un SHA-256 hexadecimal de 64 simbolos
    ABRIR archivo ubicado en ruta
    intentos <- 0
    inicio <- RelojMonotono()

    PARA CADA linea EN archivo, conservando su orden HACER
        candidato <- linea sin terminador CR/LF
        intentos <- intentos + 1
        SI SHA256(candidato) = H ENTONCES
            tiempo <- Microsegundos(RelojMonotono() - inicio)
            RETORNAR (VERDADERO, candidato, intentos, tiempo)
        FIN SI
    FIN PARA

    tiempo <- Microsegundos(RelojMonotono() - inicio)
    RETORNAR (FALSO, cadena_vacia, intentos, tiempo)
FIN ALGORITMO
```

Para `D` entradas de longitud maxima `L`, su tiempo es `O(D * L)` y usa
`O(L)` espacio adicional. A diferencia de FB, puede ser mucho mas rapido si la
contrasena esta incluida cerca del inicio, pero no garantiza encontrar cadenas
que no aparezcan en el archivo.
