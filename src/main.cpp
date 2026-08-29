#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <limits>
#include <cstddef>

#include "fb_bruteforce.hpp"
#include "bt_backtracking.hpp"

namespace {

void imprimir_ayuda(const char* programa) {
    std::cout
        << "Uso del programa ADA Practica 1:\n"
        << "  --- MODULO FB (Fuerza Bruta) ---\n"
        << "  " << programa << " hash <texto>\n"
        << "  " << programa << " fb <hash_sha256> <alfabeto> <long_min> <long_max>\n"
        << "  " << programa << " diccionario <hash_sha256> <archivo>\n"
        << "  " << programa << " comparar <hash_sha256> <alfabeto> <long_min> <long_max> <archivo>\n\n"
        << "  --- MODULO BT (Backtracking) ---\n"
        << "  " << programa << " bt <longitud> <min_lower> <min_upper> <min_digit> <min_symbol> <poda: 1|0>\n"
        << "  " << programa << " bt-comparar <longitud> <min_lower> <min_upper> <min_digit> <min_symbol>\n\n"
        << "Ejemplos:\n"
        << "  " << programa << " hash abc12\n"
        << "  " << programa << " bt 8 3 1 2 1 1\n";
}

std::size_t leer_longitud(const std::string& texto, const char* nombre) {
    if (texto.empty() || texto.front() == '-') {
        throw std::invalid_argument(std::string("Valor invalido para ") + nombre);
    }
    std::size_t posicion = 0;
    const unsigned long long valor = std::stoull(texto, &posicion);
    if (posicion != texto.size() || valor > std::numeric_limits<std::size_t>::max()) {
        throw std::invalid_argument(std::string("Valor invalido para ") + nombre);
    }
    return static_cast<std::size_t>(valor);
}

void imprimir_resultado_fb(const std::string& metodo, const fb::ResultadoAtaque& resultado) {
    std::cout << "metodo=" << metodo << '\n'
              << "encontrada=" << (resultado.encontrada ? "si" : "no") << '\n'
              << "contrasena=" << resultado.contrasena << '\n'
              << "intentos=" << resultado.intentos << '\n'
              << "tiempo_us=" << resultado.tiempo_microsegundos << '\n';
}

void imprimir_resultado_bt(const std::string& metodo, const bt::ResultadoBT& res) {
    std::cout << "metodo=" << metodo << '\n'
              << "nodos_visitados=" << res.nodos_visitados << '\n'
              << "nodos_generados=" << res.nodos_generados << '\n'
              << "soluciones_encontradas=" << res.soluciones_encontradas << '\n'
              << "tiempo_us=" << res.tiempo_microsegundos << '\n';
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            imprimir_ayuda(argv[0]);
            return 1;
        }

        const std::string comando = argv[1];
        if (comando == "--help" || comando == "-h" || comando == "ayuda") {
            imprimir_ayuda(argv[0]);
            return 0;
        }

        // --- SUBCOMANDOS MODULO FB ---
        if (comando == "hash" && argc == 3) {
            std::cout << fb::sha256(argv[2]) << '\n';
            return 0;
        }

        if (comando == "fb" && argc == 6) {
            const std::size_t longitud_minima = leer_longitud(argv[4], "long_min");
            const std::size_t longitud_maxima = leer_longitud(argv[5], "long_max");
            const auto resultado = fb::ataque_fuerza_bruta(argv[2], argv[3], longitud_minima, longitud_maxima);
            imprimir_resultado_fb("fuerza_bruta", resultado);
            return resultado.encontrada ? 0 : 2;
        }

        if (comando == "diccionario" && argc == 4) {
            const auto resultado = fb::ataque_diccionario(argv[2], argv[3]);
            imprimir_resultado_fb("diccionario", resultado);
            return resultado.encontrada ? 0 : 2;
        }

        if (comando == "comparar" && argc == 7) {
            const std::size_t longitud_minima = leer_longitud(argv[4], "long_min");
            const std::size_t longitud_maxima = leer_longitud(argv[5], "long_max");
            const auto comparacion = fb::comparar_ataques(argv[2], argv[3], longitud_minima, longitud_maxima, argv[6]);
            imprimir_resultado_fb("fuerza_bruta", comparacion.fuerza_bruta);
            std::cout << "---\n";
            imprimir_resultado_fb("diccionario", comparacion.diccionario);
            return comparacion.fuerza_bruta.encontrada ? 0 : 2;
        }

        // --- SUBCOMANDOS MODULO BT ---
        if (comando == "bt" && argc == 8) {
            bt::PoliticaConfig pol;
            pol.longitud = leer_longitud(argv[2], "longitud");
            pol.min_minusculas = static_cast<int>(leer_longitud(argv[3], "min_lower"));
            pol.min_mayusculas = static_cast<int>(leer_longitud(argv[4], "min_upper"));
            pol.min_digitos = static_cast<int>(leer_longitud(argv[5], "min_digit"));
            pol.min_simbolos = static_cast<int>(leer_longitud(argv[6], "min_symbol"));
            bool usar_poda = (leer_longitud(argv[7], "poda") == 1);

            bt::BacktrackingEngine engine(pol);
            const auto res = engine.resolver(usar_poda);
            imprimir_resultado_bt(usar_poda ? "bt_con_poda" : "bt_sin_poda", res);
            return 0;
        }

        if (comando == "bt-comparar" && argc == 7) {
            bt::PoliticaConfig pol;
            pol.longitud = leer_longitud(argv[2], "longitud");
            pol.min_minusculas = static_cast<int>(leer_longitud(argv[3], "min_lower"));
            pol.min_mayusculas = static_cast<int>(leer_longitud(argv[4], "min_upper"));
            pol.min_digitos = static_cast<int>(leer_longitud(argv[5], "min_digit"));
            pol.min_simbolos = static_cast<int>(leer_longitud(argv[6], "min_symbol"));

            bt::BacktrackingEngine engine(pol);
            const auto res_poda = engine.resolver(true);
            const auto res_sin_poda = engine.resolver(false);

            imprimir_resultado_bt("bt_con_poda", res_poda);
            std::cout << "---\n";
            imprimir_resultado_bt("bt_sin_poda", res_sin_poda);
            return 0;
        }

        std::cerr << "Comando o numero de argumentos invalido.\n\n";
        imprimir_ayuda(argv[0]);
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}