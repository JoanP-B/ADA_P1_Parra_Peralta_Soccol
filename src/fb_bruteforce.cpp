#include "fb_bruteforce.hpp"

#include "third_party/picosha2.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cctype>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <vector>

namespace fb {
namespace {

using Reloj = std::chrono::steady_clock;

void validar_alfabeto(const std::string& alfabeto) {
    if (alfabeto.empty()) {
        throw std::invalid_argument("El alfabeto no puede estar vacio.");
    }

    std::array<bool, 256> visto{};
    for (const unsigned char simbolo : alfabeto) {
        if (visto[simbolo]) {
            throw std::invalid_argument("El alfabeto no puede contener simbolos repetidos.");
        }
        visto[simbolo] = true;
    }
}

void validar_longitudes(std::size_t longitud_minima,
                        std::size_t longitud_maxima) {
    if (longitud_minima == 0) {
        throw std::invalid_argument("La longitud minima debe ser mayor que cero.");
    }
    if (longitud_minima > longitud_maxima) {
        throw std::invalid_argument(
            "La longitud minima no puede superar la longitud maxima.");
    }
}

std::string normalizar_hash(const std::string& hash) {
    if (hash.size() != 64) {
        throw std::invalid_argument(
            "Un hash SHA-256 debe contener exactamente 64 caracteres hexadecimales.");
    }

    std::string normalizado;
    normalizado.reserve(hash.size());
    for (const unsigned char caracter : hash) {
        if (!std::isxdigit(caracter)) {
            throw std::invalid_argument("El hash objetivo no es hexadecimal.");
        }
        normalizado.push_back(
            static_cast<char>(std::tolower(static_cast<unsigned char>(caracter))));
    }
    return normalizado;
}

bool incrementar_candidato(std::vector<std::size_t>& digitos,
                           std::string& candidato,
                           const std::string& alfabeto) {
    for (std::size_t posicion = digitos.size(); posicion > 0; --posicion) {
        const std::size_t indice = posicion - 1;
        if (digitos[indice] + 1 < alfabeto.size()) {
            ++digitos[indice];
            candidato[indice] = alfabeto[digitos[indice]];
            return true;
        }

        digitos[indice] = 0;
        candidato[indice] = alfabeto.front();
    }
    return false;
}

std::int64_t microsegundos_desde(const Reloj::time_point& inicio) {
    return std::chrono::duration_cast<std::chrono::microseconds>(Reloj::now() - inicio)
        .count();
}

}  // namespace

std::string sha256(const std::string& texto) {
    return picosha2::hash256_hex_string(texto);
}

std::uint64_t calcular_espacio_busqueda(const std::string& alfabeto,
                                        std::size_t longitud_minima,
                                        std::size_t longitud_maxima) {
    validar_alfabeto(alfabeto);
    validar_longitudes(longitud_minima, longitud_maxima);

    const auto base = static_cast<std::uint64_t>(alfabeto.size());
    const auto maximo = std::numeric_limits<std::uint64_t>::max();
    std::uint64_t potencia = 1;
    std::uint64_t total = 0;

    for (std::size_t longitud = 1;; ++longitud) {
        if (potencia > maximo / base) {
            throw std::overflow_error("El espacio de busqueda excede uint64_t.");
        }
        potencia *= base;

        if (longitud >= longitud_minima) {
            if (total > maximo - potencia) {
                throw std::overflow_error("El espacio de busqueda excede uint64_t.");
            }
            total += potencia;
        }

        if (longitud == longitud_maxima) {
            break;
        }
    }
    return total;
}

ResultadoAtaque ataque_fuerza_bruta(const std::string& hash_objetivo,
                                    const std::string& alfabeto,
                                    std::size_t longitud_minima,
                                    std::size_t longitud_maxima) {
    const std::string objetivo = normalizar_hash(hash_objetivo);
    // Ademas de validar, impide que el contador de intentos se desborde.
    static_cast<void>(
        calcular_espacio_busqueda(alfabeto, longitud_minima, longitud_maxima));

    ResultadoAtaque resultado;
    const auto inicio = Reloj::now();

    for (std::size_t longitud = longitud_minima;; ++longitud) {
        std::vector<std::size_t> digitos(longitud, 0);
        std::string candidato(longitud, alfabeto.front());

        do {
            ++resultado.intentos;
            if (sha256(candidato) == objetivo) {
                resultado.encontrada = true;
                resultado.contrasena = candidato;
                resultado.tiempo_microsegundos = microsegundos_desde(inicio);
                return resultado;
            }
        } while (incrementar_candidato(digitos, candidato, alfabeto));

        if (longitud == longitud_maxima) {
            break;
        }
    }

    resultado.tiempo_microsegundos = microsegundos_desde(inicio);
    return resultado;
}

ResultadoAtaque ataque_diccionario(const std::string& hash_objetivo,
                                   const std::string& ruta_diccionario) {
    const std::string objetivo = normalizar_hash(hash_objetivo);
    std::ifstream archivo(ruta_diccionario);
    if (!archivo) {
        throw std::runtime_error("No se pudo abrir el diccionario: " +
                                 ruta_diccionario);
    }

    ResultadoAtaque resultado;
    const auto inicio = Reloj::now();
    std::string candidato;

    while (std::getline(archivo, candidato)) {
        // Permite usar el mismo archivo con terminaciones LF o CRLF.
        if (!candidato.empty() && candidato.back() == '\r') {
            candidato.pop_back();
        }

        ++resultado.intentos;
        if (sha256(candidato) == objetivo) {
            resultado.encontrada = true;
            resultado.contrasena = candidato;
            resultado.tiempo_microsegundos = microsegundos_desde(inicio);
            return resultado;
        }
    }

    resultado.tiempo_microsegundos = microsegundos_desde(inicio);
    return resultado;
}

ComparacionAtaques comparar_ataques(const std::string& hash_objetivo,
                                    const std::string& alfabeto,
                                    std::size_t longitud_minima,
                                    std::size_t longitud_maxima,
                                    const std::string& ruta_diccionario) {
    ComparacionAtaques comparacion;
    comparacion.fuerza_bruta = ataque_fuerza_bruta(
        hash_objetivo, alfabeto, longitud_minima, longitud_maxima);
    comparacion.diccionario =
        ataque_diccionario(hash_objetivo, ruta_diccionario);
    return comparacion;
}

}  // namespace fb
