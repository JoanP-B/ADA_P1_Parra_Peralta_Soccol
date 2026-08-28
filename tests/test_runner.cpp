#include "fb_bruteforce.hpp"

#include <cstddef>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

int fallos = 0;

void verificar(bool condicion, const std::string& nombre) {
    if (condicion) {
        std::cout << "[OK] " << nombre << '\n';
    } else {
        std::cerr << "[FALLO] " << nombre << '\n';
        ++fallos;
    }
}

template <typename Excepcion, typename Funcion>
void verificar_excepcion(Funcion funcion, const std::string& nombre) {
    try {
        funcion();
        verificar(false, nombre);
    } catch (const Excepcion&) {
        verificar(true, nombre);
    } catch (...) {
        verificar(false, nombre + " (tipo de excepcion inesperado)");
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    const std::string ruta_diccionario =
        argc >= 2 ? argv[1] : "resources/diccionario.txt";

    verificar(
        fb::sha256("") ==
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
        "SHA-256 del texto vacio");
    verificar(
        fb::sha256("abc") ==
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
        "SHA-256 del vector conocido abc");
    verificar(fb::calcular_espacio_busqueda("ab", 1, 3) == 14,
              "Calculo del espacio |A|^1 + |A|^2 + |A|^3");

    // Instancia de referencia: 3 + 9 + 20 = 32 intentos hasta encontrar cab.
    const auto referencia =
        fb::ataque_fuerza_bruta(fb::sha256("cab"), "abc", 1, 3);
    verificar(referencia.encontrada && referencia.contrasena == "cab",
              "FB encuentra la instancia de referencia");
    verificar(referencia.intentos == 32,
              "FB respeta el orden exhaustivo documentado");

    struct CasoEquipo {
        std::string contrasena;
        std::size_t posicion_diccionario;
    };
    const std::vector<CasoEquipo> casos_equipo = {
        {"a1", 3}, {"b2a", 4}, {"3cab", 5}, {"ab12c", 6}, {"c3b21a", 7}};
    const std::string alfabeto = "abc123";

    for (std::size_t i = 0; i < casos_equipo.size(); ++i) {
        const auto& caso = casos_equipo[i];
        const std::string hash = fb::sha256(caso.contrasena);
        const auto resultado_fb = fb::ataque_fuerza_bruta(
            hash, alfabeto, caso.contrasena.size(), caso.contrasena.size());
        verificar(resultado_fb.encontrada &&
                      resultado_fb.contrasena == caso.contrasena,
                  "FB recupera contrasena de equipo " + std::to_string(i + 1));

        try {
            const auto resultado_diccionario =
                fb::ataque_diccionario(hash, ruta_diccionario);
            verificar(resultado_diccionario.encontrada &&
                          resultado_diccionario.contrasena == caso.contrasena,
                      "Diccionario recupera contrasena de equipo " +
                          std::to_string(i + 1));
            verificar(resultado_diccionario.intentos == caso.posicion_diccionario,
                      "Diccionario conserva el orden para caso " +
                          std::to_string(i + 1));
        } catch (const std::exception& error) {
            std::cerr << "[FALLO] No se pudo probar el diccionario: "
                      << error.what() << '\n';
            ++fallos;
        }
    }

    const auto comparacion = fb::comparar_ataques(
        fb::sha256("3cab"), alfabeto, 4, 4, ruta_diccionario);
    verificar(comparacion.fuerza_bruta.encontrada &&
                  comparacion.diccionario.encontrada,
              "Comparacion ejecuta ambos enfoques sobre el mismo objetivo");

    verificar_excepcion<std::invalid_argument>(
        [] { fb::ataque_fuerza_bruta(fb::sha256("a"), "", 1, 1); },
        "Rechaza alfabeto vacio");
    verificar_excepcion<std::invalid_argument>(
        [] { fb::ataque_fuerza_bruta(fb::sha256("a"), "aab", 1, 1); },
        "Rechaza simbolos repetidos");
    verificar_excepcion<std::invalid_argument>(
        [] { fb::ataque_fuerza_bruta("hash-invalido", "ab", 1, 2); },
        "Rechaza hash SHA-256 invalido");
    verificar_excepcion<std::invalid_argument>(
        [] { fb::ataque_fuerza_bruta(fb::sha256("a"), "ab", 3, 2); },
        "Rechaza intervalo de longitudes invalido");
    verificar_excepcion<std::runtime_error>(
        [] {
            fb::ataque_diccionario(
                fb::sha256("a"), "archivo_que_no_debe_existir_ada_p1.txt");
        },
        "Informa diccionario inexistente");

    if (fallos == 0) {
        std::cout << "\nTodas las pruebas del modulo FB pasaron.\n";
        return 0;
    }

    std::cerr << "\nTotal de fallos: " << fallos << '\n';
    return 1;
}
