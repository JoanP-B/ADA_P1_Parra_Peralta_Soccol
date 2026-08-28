#include "fb_bruteforce.hpp"

#include <cstddef>
#include <exception>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

namespace {

void imprimir_ayuda(const char* programa) {
    std::cout
        << "Uso:\n"
        << "  " << programa << " hash <texto>\n"
        << "  " << programa
        << " fb <hash_sha256> <alfabeto> <long_min> <long_max>\n"
        << "  " << programa
        << " diccionario <hash_sha256> <archivo>\n"
        << "  " << programa
        << " comparar <hash_sha256> <alfabeto> <long_min> <long_max> "
           "<archivo>\n\n"
        << "Ejemplo:\n"
        << "  " << programa << " hash cab\n";
}

std::size_t leer_longitud(const std::string& texto, const char* nombre) {
    if (texto.empty() || texto.front() == '-') {
        throw std::invalid_argument(std::string("Valor invalido para ") + nombre);
    }
    std::size_t posicion = 0;
    const unsigned long long valor = std::stoull(texto, &posicion);
    if (posicion != texto.size() ||
        valor > std::numeric_limits<std::size_t>::max()) {
        throw std::invalid_argument(std::string("Valor invalido para ") + nombre);
    }
    return static_cast<std::size_t>(valor);
}

void imprimir_resultado(const std::string& metodo,
                        const fb::ResultadoAtaque& resultado) {
    std::cout << "metodo=" << metodo << '\n'
              << "encontrada=" << (resultado.encontrada ? "si" : "no") << '\n'
              << "contrasena=" << resultado.contrasena << '\n'
              << "intentos=" << resultado.intentos << '\n'
              << "tiempo_us=" << resultado.tiempo_microsegundos << '\n';
}

}  // namespace

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

        if (comando == "hash" && argc == 3) {
            std::cout << fb::sha256(argv[2]) << '\n';
            return 0;
        }

        if (comando == "fb" && argc == 6) {
            const std::size_t longitud_minima = leer_longitud(argv[4], "long_min");
            const std::size_t longitud_maxima = leer_longitud(argv[5], "long_max");
            const auto resultado = fb::ataque_fuerza_bruta(
                argv[2], argv[3], longitud_minima, longitud_maxima);
            imprimir_resultado("fuerza_bruta", resultado);
            return resultado.encontrada ? 0 : 2;
        }

        if (comando == "diccionario" && argc == 4) {
            const auto resultado = fb::ataque_diccionario(argv[2], argv[3]);
            imprimir_resultado("diccionario", resultado);
            return resultado.encontrada ? 0 : 2;
        }

        if (comando == "comparar" && argc == 7) {
            const std::size_t longitud_minima = leer_longitud(argv[4], "long_min");
            const std::size_t longitud_maxima = leer_longitud(argv[5], "long_max");
            const auto comparacion = fb::comparar_ataques(
                argv[2], argv[3], longitud_minima, longitud_maxima, argv[6]);
            imprimir_resultado("fuerza_bruta", comparacion.fuerza_bruta);
            std::cout << "---\n";
            imprimir_resultado("diccionario", comparacion.diccionario);
            return comparacion.fuerza_bruta.encontrada ? 0 : 2;
        }

        std::cerr << "Comando o numero de argumentos invalido.\n\n";
        imprimir_ayuda(argv[0]);
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
