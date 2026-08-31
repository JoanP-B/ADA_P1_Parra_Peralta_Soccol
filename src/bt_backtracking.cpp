#include "bt_backtracking.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace bt {
namespace {

using Reloj = std::chrono::steady_clock;

constexpr std::array<std::uint64_t, 4> TAMANOS_CATEGORIA = {26, 26, 10, 5};
constexpr int SIN_CATEGORIA = 4;

struct MetricasSubarbol {
    std::uint64_t nodos_visitados = 0;
    std::uint64_t soluciones = 0;
};

class ContadorEstados {
public:
    ContadorEstados(const PoliticaConfig& config, bool usar_poda)
        : politica(config), poda_activa(usar_poda) {
        validar_configuracion();
    }

    MetricasSubarbol resolver() {
        return resolver_estado(0, 0, 0, 0, 0, SIN_CATEGORIA, false);
    }

private:
    const PoliticaConfig& politica;
    bool poda_activa;
    std::unordered_map<std::uint64_t, MetricasSubarbol> memoria;

    void validar_configuracion() const {
        if (politica.longitud == 0 || politica.longitud > 10) {
            throw std::invalid_argument("La longitud de BT debe estar entre 1 y 10.");
        }
        if (politica.min_minusculas < 0 || politica.min_mayusculas < 0 ||
            politica.min_digitos < 0 || politica.min_simbolos < 0) {
            throw std::invalid_argument("Los minimos de la politica no pueden ser negativos.");
        }
    }

    bool es_factible(std::size_t longitud_actual, int minusculas, int mayusculas,
                      int digitos, int simbolos, bool tiene_repeticion) const {
        if (politica.prohibir_consecutivos_repetidos && tiene_repeticion) {
            return false;
        }

        const int restantes = static_cast<int>(politica.longitud - longitud_actual);
        const int faltantes =
            std::max(0, politica.min_minusculas - minusculas) +
            std::max(0, politica.min_mayusculas - mayusculas) +
            std::max(0, politica.min_digitos - digitos) +
            std::max(0, politica.min_simbolos - simbolos);
        return faltantes <= restantes;
    }

    bool es_solucion(int minusculas, int mayusculas, int digitos, int simbolos,
                     bool tiene_repeticion) const {
        return (!politica.prohibir_consecutivos_repetidos || !tiene_repeticion) &&
               minusculas >= politica.min_minusculas &&
               mayusculas >= politica.min_mayusculas &&
               digitos >= politica.min_digitos &&
               simbolos >= politica.min_simbolos;
    }

    static std::uint64_t sumar(std::uint64_t izquierdo, std::uint64_t derecho) {
        if (izquierdo > std::numeric_limits<std::uint64_t>::max() - derecho) {
            throw std::overflow_error("Las metricas de BT exceden uint64_t.");
        }
        return izquierdo + derecho;
    }

    static std::uint64_t multiplicar(std::uint64_t valor, std::uint64_t factor) {
        if (valor != 0 && factor > std::numeric_limits<std::uint64_t>::max() / valor) {
            throw std::overflow_error("Las metricas de BT exceden uint64_t.");
        }
        return valor * factor;
    }

    int limitar(int cantidad, int minimo) const {
        return std::min(cantidad, minimo);
    }

    std::uint64_t crear_clave(std::size_t longitud_actual, int minusculas,
                              int mayusculas, int digitos, int simbolos,
                              int ultima_categoria, bool tiene_repeticion) const {
        std::uint64_t clave = longitud_actual;
        clave = (clave << 4) | static_cast<std::uint64_t>(limitar(minusculas, politica.min_minusculas));
        clave = (clave << 4) | static_cast<std::uint64_t>(limitar(mayusculas, politica.min_mayusculas));
        clave = (clave << 4) | static_cast<std::uint64_t>(limitar(digitos, politica.min_digitos));
        clave = (clave << 4) | static_cast<std::uint64_t>(limitar(simbolos, politica.min_simbolos));
        clave = (clave << 3) | static_cast<std::uint64_t>(ultima_categoria);
        clave = (clave << 1) | static_cast<std::uint64_t>(tiene_repeticion);
        return clave;
    }

    MetricasSubarbol resolver_estado(std::size_t longitud_actual, int minusculas,
                                     int mayusculas, int digitos, int simbolos,
                                     int ultima_categoria, bool tiene_repeticion) {
        const std::uint64_t clave = crear_clave(longitud_actual, minusculas, mayusculas,
                                                digitos, simbolos, ultima_categoria,
                                                tiene_repeticion);
        const auto encontrado = memoria.find(clave);
        if (encontrado != memoria.end()) {
            return encontrado->second;
        }

        MetricasSubarbol resultado;
        resultado.nodos_visitados = 1;

        if (poda_activa && !es_factible(longitud_actual, minusculas, mayusculas,
                                        digitos, simbolos, tiene_repeticion)) {
            memoria.emplace(clave, resultado);
            return resultado;
        }

        if (longitud_actual == politica.longitud) {
            resultado.soluciones = es_solucion(minusculas, mayusculas, digitos,
                                               simbolos, tiene_repeticion) ? 1 : 0;
            memoria.emplace(clave, resultado);
            return resultado;
        }

        for (int categoria = 0; categoria < static_cast<int>(TAMANOS_CATEGORIA.size()); ++categoria) {
            const bool misma_categoria = categoria == ultima_categoria;
            const std::uint64_t opciones_distintas =
                TAMANOS_CATEGORIA[categoria] - (misma_categoria ? 1 : 0);

            const int nuevas_minusculas = minusculas + (categoria == 0);
            const int nuevas_mayusculas = mayusculas + (categoria == 1);
            const int nuevos_digitos = digitos + (categoria == 2);
            const int nuevos_simbolos = simbolos + (categoria == 3);

            if (opciones_distintas > 0) {
                const auto hijo = resolver_estado(longitud_actual + 1, nuevas_minusculas,
                                                  nuevas_mayusculas, nuevos_digitos,
                                                  nuevos_simbolos, categoria,
                                                  tiene_repeticion);
                resultado.nodos_visitados = sumar(
                    resultado.nodos_visitados,
                    multiplicar(hijo.nodos_visitados, opciones_distintas));
                resultado.soluciones = sumar(
                    resultado.soluciones,
                    multiplicar(hijo.soluciones, opciones_distintas));
            }

            if (misma_categoria) {
                const auto hijo_repetido = resolver_estado(longitud_actual + 1,
                                                            nuevas_minusculas,
                                                            nuevas_mayusculas,
                                                            nuevos_digitos,
                                                            nuevos_simbolos,
                                                            categoria, true);
                resultado.nodos_visitados = sumar(resultado.nodos_visitados,
                                                   hijo_repetido.nodos_visitados);
                resultado.soluciones = sumar(resultado.soluciones,
                                             hijo_repetido.soluciones);
            }
        }

        memoria.emplace(clave, resultado);
        return resultado;
    }
};

class EnumeradorDirecto {
public:
    EnumeradorDirecto(const PoliticaConfig& config, bool usar_poda)
        : politica(config), poda_activa(usar_poda) {}

    ResultadoBT resolver() {
        const auto inicio = Reloj::now();
        std::string actual;
        actual.reserve(politica.longitud);
        explorar(actual, 0, 0, 0, 0, false, resultado);
        const auto fin = Reloj::now();
        resultado.tiempo_microsegundos =
            std::chrono::duration_cast<std::chrono::microseconds>(fin - inicio).count();
        return resultado;
    }

private:
    const PoliticaConfig& politica;
    bool poda_activa;
    ResultadoBT resultado;

    bool es_factible(const std::string& actual, int minusculas, int mayusculas,
                      int digitos, int simbolos, bool tiene_repeticion) const {
        if (politica.prohibir_consecutivos_repetidos && tiene_repeticion) {
            return false;
        }

        const int restantes = static_cast<int>(politica.longitud - actual.size());
        const int faltantes =
            std::max(0, politica.min_minusculas - minusculas) +
            std::max(0, politica.min_mayusculas - mayusculas) +
            std::max(0, politica.min_digitos - digitos) +
            std::max(0, politica.min_simbolos - simbolos);
        return faltantes <= restantes;
    }

    bool es_solucion(int minusculas, int mayusculas, int digitos, int simbolos,
                     bool tiene_repeticion) const {
        return (!politica.prohibir_consecutivos_repetidos || !tiene_repeticion) &&
               minusculas >= politica.min_minusculas &&
               mayusculas >= politica.min_mayusculas &&
               digitos >= politica.min_digitos &&
               simbolos >= politica.min_simbolos;
    }

    void explorar(std::string& actual, int minusculas, int mayusculas, int digitos,
                  int simbolos, bool tiene_repeticion, ResultadoBT& metricas) {
        ++metricas.nodos_visitados;
        if (poda_activa && !es_factible(actual, minusculas, mayusculas, digitos,
                                        simbolos, tiene_repeticion)) {
            return;
        }
        if (actual.size() == politica.longitud) {
            if (es_solucion(minusculas, mayusculas, digitos, simbolos, tiene_repeticion)) {
                ++metricas.soluciones_encontradas;
            }
            return;
        }

        static const std::string alfabeto =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%";
        for (const char caracter : alfabeto) {
            const unsigned char valor = static_cast<unsigned char>(caracter);
            const bool repeticion = tiene_repeticion ||
                (!actual.empty() && actual.back() == caracter);
            actual.push_back(caracter);
            ++metricas.nodos_generados;
            explorar(actual,
                     minusculas + (std::islower(valor) ? 1 : 0),
                     mayusculas + (std::isupper(valor) ? 1 : 0),
                     digitos + (std::isdigit(valor) ? 1 : 0),
                     simbolos + (!std::isalnum(valor) ? 1 : 0),
                     repeticion, metricas);
            actual.pop_back();
        }
    }
};

}  // namespace

BacktrackingEngine::BacktrackingEngine(const PoliticaConfig& config) : politica(config) {}

ResultadoBT BacktrackingEngine::resolver(bool usar_poda) {
    ContadorEstados contador(politica, usar_poda);
    const auto metricas = contador.resolver();

    ResultadoBT resultado;
    resultado.nodos_visitados = metricas.nodos_visitados;
    resultado.nodos_generados = metricas.nodos_visitados - 1;
    resultado.soluciones_encontradas = metricas.soluciones;
    resultado.tiempo_microsegundos = -1;

    if (usar_poda) {
        ContadorEstados contador_sin_poda(politica, false);
        const auto sin_poda = contador_sin_poda.resolver();
        resultado.porcentaje_reduccion =
            (1.0 - static_cast<double>(resultado.nodos_visitados) /
                       static_cast<double>(sin_poda.nodos_visitados)) * 100.0;
    }

    return resultado;
}

ResultadoBT BacktrackingEngine::resolver_enumerativo(bool usar_poda) {
    EnumeradorDirecto enumerador(politica, usar_poda);
    return enumerador.resolver();
}

}  // namespace bt
