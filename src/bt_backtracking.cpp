#include "bt_backtracking.hpp"
#include <chrono>
#include <cctype>
#include <algorithm>
#include <cmath>

namespace bt {

using Reloj = std::chrono::steady_clock;

BacktrackingEngine::BacktrackingEngine(const PoliticaConfig& config) : politica(config) {
    // Alfabeto base de 69 caracteres exigido por la guía
    alfabeto_base = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%";
}

bool BacktrackingEngine::es_factible(const std::string& prefijo, std::size_t k) const {
    // Restricción Local: Prohibición de dos caracteres idénticos consecutivos
    if (politica.prohibir_consecutivos_repetidos && k > 1) {
        if (prefijo[k - 1] == prefijo[k - 2]) {
            return false; // Poda inmediata por violación local
        }
    }

    // Contadores de los tipos de símbolos presentes en el prefijo parcial
    int minus = 0, mayus = 0, dig = 0, simb = 0;
    for (std::size_t i = 0; i < k; ++i) {
        char c = prefijo[i];
        if (std::islower(static_cast<unsigned char>(c))) minus++;
        else if (std::isupper(static_cast<unsigned char>(c))) mayus++;
        else if (std::isdigit(static_cast<unsigned char>(c))) dig++;
        else simb++;
    }

    // Posiciones restantes en el string para completar la longitud objetivo
    int restantes = static_cast<int>(politica.longitud - k);

    // Cálculo de mínimos faltantes
    int falta_minus = std::max(0, politica.min_minusculas - minus);
    int falta_mayus = std::max(0, politica.min_mayusculas - mayus);
    int falta_dig   = std::max(0, politica.min_digitos - dig);
    int falta_simb  = std::max(0, politica.min_simbolos - simb);

    int total_faltantes = falta_minus + falta_mayus + falta_dig + falta_simb;

    // Si la cantidad de caracteres obligatorios supera el espacio disponible -> Infactible
    return total_faltantes <= restantes;
}

void BacktrackingEngine::resolver_con_poda(std::string& actual, ResultadoBT& res) {
    res.nodos_visitados++;

    // Poda por factibilidad sobre prefijo parcial k <= n
    if (!es_factible(actual, actual.length())) {
        return; // Detener la exploración en esta rama
    }

    // Caso base: se alcanzó una solución completa de longitud n
    if (actual.length() == politica.longitud) {
        res.soluciones_encontradas++;
        return;
    }

    // Exploración incremental
    for (char c : alfabeto_base) {
        actual.push_back(c);
        res.nodos_generados++;
        resolver_con_poda(actual, res);
        actual.pop_back(); // Deshacer (Backtrack)
    }
}

void BacktrackingEngine::resolver_sin_poda(std::string& actual, ResultadoBT& res) {
    res.nodos_visitados++;

    if (actual.length() == politica.longitud) {
        if (es_factible(actual, actual.length())) {
            res.soluciones_encontradas++;
        }
        return;
    }

    for (char c : alfabeto_base) {
        actual.push_back(c);
        res.nodos_generados++; // Contar el nodo al generarlo
        resolver_sin_poda(actual, res);
        actual.pop_back();
    }
}

ResultadoBT BacktrackingEngine::resolver(bool usar_poda) {
    ResultadoBT res;
    std::string actual = "";
    actual.reserve(politica.longitud);

    auto inicio = Reloj::now();

    if (usar_poda) {
        resolver_con_poda(actual, res);
    } else {
        resolver_sin_poda(actual, res);
    }

    auto fin = Reloj::now();
    res.tiempo_microsegundos = std::chrono::duration_cast<std::chrono::microseconds>(fin - inicio).count();

    // Cálculo del porcentaje de reducción del espacio explorado
    if (usar_poda && res.nodos_generados > 0) {
        double nodos_totales_teoricos = 0.0;
        for (std::size_t k = 0; k <= politica.longitud; ++k) {
            nodos_totales_teoricos += std::pow(alfabeto_base.size(), k);
        }
        res.porcentaje_reduccion = (1.0 - (static_cast<double>(res.nodos_visitados) / nodos_totales_teoricos)) * 100.0;
    }

    return res;
}

} // namespace bt