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

// Evaluación de factibilidad con contadores ya calculados incrementalmente (O(1))
// Verifica si el prefijo actual de longitud k puede llevar a una solución válida.
bool BacktrackingEngine::es_factible(std::size_t k, char ultimo_char, char penultimo_char,
                                     int minus, int mayus, int dig, int simb) const {
    // Restricción local: No caracteres idénticos consecutivos
    if (politica.prohibir_consecutivos_repetidos && k >= 2 && ultimo_char == penultimo_char) {
        return false;
    }

    // Posiciones restantes para completar la longitud objetivo
    int restantes = static_cast<int>(politica.longitud - k);

    // Mínimos faltantes por categoría
    int falta_minus = std::max(0, politica.min_minusculas - minus);
    int falta_mayus = std::max(0, politica.min_mayusculas - mayus);
    int falta_dig   = std::max(0, politica.min_digitos - dig);
    int falta_simb  = std::max(0, politica.min_simbolos - simb);

    int total_faltantes = falta_minus + falta_mayus + falta_dig + falta_simb;

    // Si la cantidad de caracteres obligatorios supera el espacio disponible → Infactible
    return total_faltantes <= restantes;
}

// Variante privada con contadores incrementales para evitar recorrido O(k) por nodo
void BacktrackingEngine::resolver_con_poda_impl(
        std::string& actual, ResultadoBT& res,
        int minus, int mayus, int dig, int simb) {

    res.nodos_visitados++;

    std::size_t k = actual.size();
    char ultimo    = (k >= 1) ? actual[k - 1] : '\0';
    char penultimo = (k >= 2) ? actual[k - 2] : '\0';

    // Poda: evaluar factibilidad del prefijo actual
    if (!es_factible(k, ultimo, penultimo, minus, mayus, dig, simb)) {
        return;
    }

    // Caso base: solución completa de longitud n
    if (k == politica.longitud) {
        res.soluciones_encontradas++;
        return;
    }

    // Exploración incremental: agregar un nuevo símbolo
    for (char c : alfabeto_base) {
        // Actualizar contadores en O(1) según el tipo del nuevo carácter
        int d_minus = 0, d_mayus = 0, d_dig = 0, d_simb = 0;
        unsigned char uc = static_cast<unsigned char>(c);
        if (std::islower(uc))       d_minus = 1;
        else if (std::isupper(uc))  d_mayus = 1;
        else if (std::isdigit(uc))  d_dig   = 1;
        else                        d_simb  = 1;

        actual.push_back(c);
        res.nodos_generados++;
        resolver_con_poda_impl(actual, res,
                               minus + d_minus,
                               mayus + d_mayus,
                               dig   + d_dig,
                               simb  + d_simb);
        actual.pop_back(); // Deshacer (Backtrack)
    }
}

void BacktrackingEngine::resolver_sin_poda(std::string& actual, ResultadoBT& res) {
    res.nodos_visitados++;

    if (actual.length() == politica.longitud) {
        // Verificar política de composición y restricción de consecutivos al final
        int minus = 0, mayus = 0, dig = 0, simb = 0;
        bool tiene_consecutivos = false;
        for (std::size_t i = 0; i < actual.size(); ++i) {
            char c = actual[i];
            unsigned char uc = static_cast<unsigned char>(c);
            if (std::islower(uc))       minus++;
            else if (std::isupper(uc))  mayus++;
            else if (std::isdigit(uc))  dig++;
            else                        simb++;
            if (politica.prohibir_consecutivos_repetidos && i > 0 && c == actual[i - 1]) {
                tiene_consecutivos = true;
            }
        }
        if (!tiene_consecutivos          &&
            minus  >= politica.min_minusculas &&
            mayus  >= politica.min_mayusculas &&
            dig    >= politica.min_digitos    &&
            simb   >= politica.min_simbolos) {
            res.soluciones_encontradas++;
        }
        return;
    }

    for (char c : alfabeto_base) {
        actual.push_back(c);
        res.nodos_generados++;
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
        resolver_con_poda_impl(actual, res, 0, 0, 0, 0);
    } else {
        resolver_sin_poda(actual, res);
    }

    auto fin = Reloj::now();
    res.tiempo_microsegundos = std::chrono::duration_cast<std::chrono::microseconds>(fin - inicio).count();

    // Cálculo del porcentaje de reducción del espacio explorado
    if (usar_poda) {
        double nodos_totales_teoricos = 0.0;
        for (std::size_t k = 0; k <= politica.longitud; ++k) {
            nodos_totales_teoricos += std::pow(static_cast<double>(alfabeto_base.size()), static_cast<double>(k));
        }
        if (nodos_totales_teoricos > 0.0) {
            res.porcentaje_reduccion = (1.0 - (static_cast<double>(res.nodos_visitados) / nodos_totales_teoricos)) * 100.0;
        }
    }

    return res;
}

} // namespace bt