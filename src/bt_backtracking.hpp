#ifndef BT_BACKTRACKING_HPP
#define BT_BACKTRACKING_HPP

#include <string>
#include <vector>
#include <cstddef>
#include <cstdint>

namespace bt {

struct PoliticaConfig {
    std::size_t longitud = 8;
    int min_minusculas = 3; // Basado en Semilla 3817
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
private:
    std::string alfabeto_base;
    PoliticaConfig politica;

    // Evaluación de factibilidad con contadores incrementales (O(1) por nodo)
    bool es_factible(std::size_t k, char ultimo_char, char penultimo_char,
                     int minus, int mayus, int dig, int simb) const;

    // Recursión incremental con Poda (usa contadores pasados por parámetro)
    void resolver_con_poda_impl(std::string& actual, ResultadoBT& res,
                                int minus, int mayus, int dig, int simb);

    // Recursión exhaustiva sin Poda (para calibración de la rúbrica)
    void resolver_sin_poda(std::string& actual, ResultadoBT& res);

public:
    BacktrackingEngine(const PoliticaConfig& config);
    
    // Método principal para ejecutar el experimento
    ResultadoBT resolver(bool usar_poda);
};

} // namespace bt

#endif // BT_BACKTRACKING_HPP