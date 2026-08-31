# Pseudocodigo formal del modulo BT

## Estado y representacion compacta

Cada nodo del arbol representa un prefijo. El estado mantiene en `O(1)` la
longitud `k`, las cantidades por categoria, la categoria del ultimo caracter y
si ya existe una repeticion consecutiva. Para medir `n = 6, 8, 10`, los
prefijos equivalentes se agrupan por ese estado. La multiplicidad de cada
transicion conserva exactamente el numero de nodos y soluciones del arbol de
69 simbolos, sin materializar ramas equivalentes.

## Algoritmo 1: Backtracking con poda

**Entrada:** politica `P` y categorias de tamanos `(26, 26, 10, 5)`.

**Salida:** `(nodos_visitados, nodos_generados, soluciones, tiempo)`.

```text
ALGORITMO ResolverConPoda(P)
    memoria <- mapa vacio
    inicio <- RelojMonotono()
    resultado <- Contar(0, 0, 0, 0, 0, SIN_CATEGORIA, FALSO, P, memoria)
    resultado.tiempo <- Microsegundos(RelojMonotono() - inicio)
    resultado.nodos_generados <- resultado.nodos_visitados - 1
    RETORNAR resultado
FIN ALGORITMO

FUNCION Contar(k, minus, mayus, dig, simb, ultima_categoria, repetido, P, memoria)
    estado <- (k, Min(minus, P.minLower), Min(mayus, P.minUpper),
               Min(dig, P.minDigit), Min(simb, P.minSymbol),
               ultima_categoria, repetido)
    SI estado esta en memoria ENTONCES
        RETORNAR memoria[estado]
    FIN SI

    resultado <- (nodos_visitados = 1, soluciones = 0)
    SI NO EsFactible(k, minus, mayus, dig, simb, repetido, P) ENTONCES
        memoria[estado] <- resultado
        RETORNAR resultado
    FIN SI
    SI k = P.n ENTONCES
        resultado.soluciones <- 1
        memoria[estado] <- resultado
        RETORNAR resultado
    FIN SI

    PARA CADA categoria c EN {minuscula, mayuscula, digito, simbolo} HACER
        diferentes <- Tamano(c) - (c = ultima_categoria ? 1 : 0)
        hijo <- Contar(k + 1, actualizar_contador(c), c, repetido, P, memoria)
        resultado <- resultado + diferentes * hijo
        SI c = ultima_categoria ENTONCES
            hijo_repetido <- Contar(k + 1, actualizar_contador(c), c,
                                    VERDADERO, P, memoria)
            resultado <- resultado + hijo_repetido
        FIN SI
    FIN PARA
    memoria[estado] <- resultado
    RETORNAR resultado
FIN FUNCION
```

```text
FUNCION EsFactible(k, minus, mayus, dig, simb, repetido, P)
    SI P.prohibeRepetidos Y repetido ENTONCES
        RETORNAR FALSO
    FIN SI
    restantes <- P.n - k
    faltantes <- Max(0, P.minLower - minus) + Max(0, P.minUpper - mayus)
                  + Max(0, P.minDigit - dig) + Max(0, P.minSymbol - simb)
    RETORNAR faltantes <= restantes
FIN FUNCION
```

La factibilidad es `O(1)`. La multiplicidad `diferentes` separa el unico
caracter que repite exactamente el ultimo simbolo de los demas caracteres de
la misma categoria.

## Algoritmo 2: exploracion sin poda

La comparacion ejecuta la misma recurrencia, pero omite `EsFactible` y valida
la politica completa solo en las hojas. Asi explora exactamente
`sum_{k=0}^{n} 69^k` nodos y debe producir el mismo conteo de soluciones que
la version con poda.

## Complejidad

La exploracion sin poda tarda `Theta(69^n)` y usa `O(n)` de pila. Con poda, el
peor caso sigue siendo exponencial y el mejor caso descarta ramas temprano. El
conteo compacto conserva las metricas exactas del arbol logico y usa memoria
proporcional a los estados distintos de la politica.
