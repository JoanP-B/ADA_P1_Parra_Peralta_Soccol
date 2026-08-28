#ifndef ADA_P1_FB_BRUTEFORCE_HPP
#define ADA_P1_FB_BRUTEFORCE_HPP

#include <cstddef>
#include <cstdint>
#include <string>

namespace fb {

// Resultado comun para fuerza bruta y ataque por diccionario.
struct ResultadoAtaque {
    bool encontrada = false;
    std::string contrasena;
    std::uint64_t intentos = 0;
    std::int64_t tiempo_microsegundos = 0;
};

struct ComparacionAtaques {
    ResultadoAtaque fuerza_bruta;
    ResultadoAtaque diccionario;
};

// Calcula SHA-256 y devuelve 64 caracteres hexadecimales en minuscula.
std::string sha256(const std::string& texto);

// Numero de cadenas que se enumeran entre longitud_minima y longitud_maxima.
// Lanza std::invalid_argument ante parametros invalidos y std::overflow_error
// cuando el espacio no cabe en std::uint64_t.
std::uint64_t calcular_espacio_busqueda(const std::string& alfabeto,
                                        std::size_t longitud_minima,
                                        std::size_t longitud_maxima);

// Enumera exhaustivamente, por longitud y luego en el orden del alfabeto,
// hasta encontrar una cadena cuyo SHA-256 sea hash_objetivo.
ResultadoAtaque ataque_fuerza_bruta(const std::string& hash_objetivo,
                                    const std::string& alfabeto,
                                    std::size_t longitud_minima,
                                    std::size_t longitud_maxima);

// Prueba, en el orden del archivo, una contrasena por linea.
ResultadoAtaque ataque_diccionario(const std::string& hash_objetivo,
                                   const std::string& ruta_diccionario);

// Ejecuta ambos ataques con el mismo hash para facilitar una comparacion justa.
ComparacionAtaques comparar_ataques(const std::string& hash_objetivo,
                                    const std::string& alfabeto,
                                    std::size_t longitud_minima,
                                    std::size_t longitud_maxima,
                                    const std::string& ruta_diccionario);

}  // namespace fb

#endif  // ADA_P1_FB_BRUTEFORCE_HPP
