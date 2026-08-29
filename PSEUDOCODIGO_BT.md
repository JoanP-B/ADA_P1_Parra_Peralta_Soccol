# Pseudocódigo formal del módulo BT (Backtracking con Poda)

## Algoritmo 1: Generación de Contraseñas bajo Política (Con Poda)

**Entrada:** Alfabeto `Σ`, configuración de política `P` (min_lower, min_upper, min_digit, min_symbol, n).
**Salida:** Métricas del árbol (nodos_visitados, soluciones, tiempo).

```text
ALGORITMO BacktrackingConPoda(P, Σ)
    nodos_visitados <- 0
    soluciones <- 0
    actual <- cadena_vacia
    inicio <- RelojMonotono()

    PROCEDIMIENTO Resolver(actual)
        nodos_visitados <- nodos_visitados + 1

        // Evaluación de Factibilidad sobre el prefijo parcial
        SI NOT EsFactible(actual, P) ENTONCES
            RETORNAR // Poda efectiva de la rama
        FIN SI

        SI Longitud(actual) = P.n ENTONCES
            soluciones <- soluciones + 1
            RETORNAR
        FIN SI

        PARA CADA simbolo c EN Σ HACER
            actual.Agregar(c)       // Aplicar selección
            Resolver(actual)         // Llamada recursiva
            actual.RemoverUltimo()  // Deshacer (Backtrack)
        FIN PARA
    FIN PROCEDIMIENTO

    Resolver(actual)
    tiempo <- Microsegundos(RelojMonotono() - inicio)
    RETORNAR (nodos_visitados, soluciones, tiempo)
FIN ALGORITMO

---

## Algoritmo 2: Función de Evaluación de Factibilidad

FUNCIÓN EsFactible(prefijo, P)
    k <- Longitud(prefijo)

    // Restricción local: No caracteres idénticos consecutivos
    SI k > 1 Y prefijo[k-1] = prefijo[k-2] ENTONCES
        RETORNAR FALSO
    FIN SI

    minus <- ContarMinusculas(prefijo)
    mayus <- ContarMayusculas(prefijo)
    dig   <- ContarDigitos(prefijo)
    simb  <- ContarSimbolos(prefijo)

    restantes <- P.n - k

    falta_minus <- Max(0, P.min_lower - minus)
    falta_mayus <- Max(0, P.min_upper - mayus)
    falta_dig   <- Max(0, P.min_digit - dig)
    falta_simb  <- Max(0, P.min_symbol - simb)

    total_faltantes <- falta_minus + falta_mayus + falta_dig + falta_simb

    // Poda por insuficiencia de casillas libres
    SI total_faltantes > restantes ENTONCES
        RETORNAR FALSO
    FIN SI

    RETORNAR VERDADERO
FIN FUNCIÓN