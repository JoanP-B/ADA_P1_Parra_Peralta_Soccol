#ifndef FB_BRUTEFORCE_HPP
#define FB_BRUTEFORCE_HPP

#include <string>
#include <cstddef>
#include <cstdint>

namespace fb {

struct ResultadoAtaque {
    bool encontrada = false;
    std::string contrasena = "";
    std::uint64_t intentos = 0;
    std::int64_t tiempo_microsegundos = 0;
};

struct ComparacionAtaques {
    ResultadoAtaque fuerza_bruta;
    ResultadoAtaque diccionario;
};

std::string sha256(const std::string& texto);

std::uint64_t calcular_espacio_busqueda(const std::string& alfabeto,
                                        std::size_t longitud_minima,
                                        std::size_t longitud_maxima);

ResultadoAtaque ataque_fuerza_bruta(const std::string& hash_objetivo,
                                    const std::string& alfabeto,
                                    std::size_t longitud_minima,
                                    std::size_t longitud_maxima);

ResultadoAtaque ataque_diccionario(const std::string& hash_objetivo,
                                   const std::string& ruta_diccionario);

ComparacionAtaques comparar_ataques(const std::string& hash_objetivo,
                                    const std::string& alfabeto,
                                    std::size_t longitud_minima,
                                    std::size_t longitud_maxima,
                                    const std::string& ruta_diccionario);

} // namespace fb

#endif // FB_BRUTEFORCE_HPP