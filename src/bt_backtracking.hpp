#ifndef BT_BACKTRACKING_HPP
#define BT_BACKTRACKING_HPP

#include <cstddef>
#include <cstdint>

namespace bt {

struct PoliticaConfig {
    std::size_t longitud = 8;
    int min_minusculas = 3;
    int min_mayusculas = 2;
    int min_digitos = 2;
    int min_simbolos = 1;
    bool prohibir_consecutivos_repetidos = true;
};

struct ResultadoBT {
    std::uint64_t nodos_generados = 0;
    std::uint64_t nodos_visitados = 0;
    std::uint64_t soluciones_encontradas = 0;
    std::int64_t tiempo_microsegundos = 0;
    double porcentaje_reduccion = 0.0;
};

class BacktrackingEngine {
public:
    explicit BacktrackingEngine(const PoliticaConfig& config);

    // Calcula metricas exactas del arbol logico; no es una medicion de tiempo.
    ResultadoBT resolver(bool usar_poda);
    // Recorre realmente el arbol y solo debe usarse en instancias acotadas.
    ResultadoBT resolver_enumerativo(bool usar_poda);

private:
    PoliticaConfig politica;
};

}  // namespace bt

#endif  // BT_BACKTRACKING_HPP
