#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../src/bt_backtracking.hpp"
#include "../src/fb_bruteforce.hpp"

namespace {

const std::string ALFABETO_A1 = "abcdefghijklmnopqrstuvwxyz";
const std::string ALFABETO_A2 = "abcdefghijklmnopqrstuvwxyz0123456789";

struct InstanciaFB {
    std::string nombre;
    std::string contrasena;
    std::string alfabeto;
    bool instancia_equipo;
};

std::vector<InstanciaFB> generar_instancias_equipo() {
    std::int64_t estado = 3817;
    const std::vector<std::size_t> longitudes = {4, 4, 5, 5, 6};
    const std::vector<std::string> alfabetos = {
        ALFABETO_A1, ALFABETO_A2, ALFABETO_A1, ALFABETO_A2, ALFABETO_A1};
    std::vector<InstanciaFB> instancias;

    for (std::size_t indice = 0; indice < longitudes.size(); ++indice) {
        std::string contrasena;
        for (std::size_t posicion = 0; posicion < longitudes[indice]; ++posicion) {
            estado = (1103515245LL * estado + 12345LL) % 2147483648LL;
            contrasena += alfabetos[indice][static_cast<std::size_t>(estado) % alfabetos[indice].size()];
        }
        instancias.push_back({"equipo_" + std::to_string(indice + 1), contrasena,
                              alfabetos[indice], true});
    }
    return instancias;
}

std::vector<InstanciaFB> construir_experimentos_fb() {
    std::vector<InstanciaFB> instancias = generar_instancias_equipo();
    instancias.push_back({"calibracion_a1_n3", "abc", ALFABETO_A1, false});
    instancias.push_back({"calibracion_a2_n3", "abc", ALFABETO_A2, false});
    return instancias;
}

double a_milisegundos(std::int64_t microsegundos) {
    return static_cast<double>(microsegundos) / 1000.0;
}

void ejecutar_experimentos_fb() {
    std::cout << "[+] Ejecutando experimentos de Fuerza Bruta (FB)...\n";
    std::ofstream csv("results/tiempos_fb.csv");
    if (!csv) {
        throw std::runtime_error("No se pudo crear results/tiempos_fb.csv.");
    }

    csv << "instancia,tipo,contrasena_real,hash_sha256,alfabeto_tam,longitud,"
           "intentos_fb,tiempo_fb_us,tiempo_fb_ms,encontrada_dict,intentos_dict,"
           "tiempo_dict_us,tiempo_dict_ms\n";
    csv << std::fixed << std::setprecision(3);

    for (const auto& instancia : construir_experimentos_fb()) {
        const std::string hash = fb::sha256(instancia.contrasena);
        const auto resultado_fb = fb::ataque_fuerza_bruta(
            hash, instancia.alfabeto, 1, instancia.contrasena.size());
        const auto resultado_diccionario = fb::ataque_diccionario(
            hash, "resources/diccionario.txt");

        if (!resultado_fb.encontrada || resultado_fb.contrasena != instancia.contrasena) {
            throw std::runtime_error("La fuerza bruta no resolvio una instancia esperada.");
        }

        csv << instancia.nombre << ','
            << (instancia.instancia_equipo ? "equipo" : "calibracion") << ','
            << instancia.contrasena << ',' << hash << ',' << instancia.alfabeto.size() << ','
            << instancia.contrasena.size() << ',' << resultado_fb.intentos << ','
            << resultado_fb.tiempo_microsegundos << ','
            << a_milisegundos(resultado_fb.tiempo_microsegundos) << ','
            << (resultado_diccionario.encontrada ? "si" : "no") << ','
            << resultado_diccionario.intentos << ','
            << resultado_diccionario.tiempo_microsegundos << ','
            << a_milisegundos(resultado_diccionario.tiempo_microsegundos) << '\n';
    }

    std::cout << "    [OK] Se registraron 7 configuraciones de FB.\n";
}

void verificar_referencia_bt() {
    const bt::PoliticaConfig referencia = {6, 2, 1, 1, 1, true};
    bt::BacktrackingEngine motor(referencia);
    const auto con_poda = motor.resolver(true);
    const auto sin_poda = motor.resolver(false);

    if (con_poda.soluciones_encontradas == 0 ||
        con_poda.soluciones_encontradas != sin_poda.soluciones_encontradas ||
        con_poda.nodos_visitados >= sin_poda.nodos_visitados) {
        throw std::runtime_error("Fallo la validacion de la instancia de referencia de BT.");
    }
    std::cout << "[OK] Instancia de referencia de BT verificada correctamente.\n\n";
}

void escribir_fila_bt(std::ofstream& csv, const std::string& nombre,
                      const bt::PoliticaConfig& politica, bool poda_activa,
                      const bt::ResultadoBT& resultado) {
    csv << nombre << ',' << politica.longitud << ','
        << (poda_activa ? "true" : "false") << ','
        << resultado.nodos_visitados << ',' << resultado.nodos_generados << ','
        << resultado.soluciones_encontradas << ',' << resultado.tiempo_microsegundos << ','
        << a_milisegundos(resultado.tiempo_microsegundos) << ','
        << resultado.porcentaje_reduccion << "\n";
}

void ejecutar_experimentos_bt() {
    std::cout << "[+] Ejecutando experimentos de Backtracking (BT)...\n";
    std::ofstream csv("results/tiempos_bt.csv");
    if (!csv) {
        throw std::runtime_error("No se pudo crear results/tiempos_bt.csv.");
    }

    csv << "variante,longitud,poda_activa,nodos_visitados,nodos_generados,soluciones,"
           "tiempo_us,tiempo_ms,porcentaje_reduccion\n";
    csv << std::fixed << std::setprecision(3);

    const std::vector<bt::PoliticaConfig> politicas = {
        {8, 3, 2, 2, 1, true},
        {6, 3, 2, 2, 1, true},
        {10, 3, 2, 2, 1, true},
        {8, 1, 0, 0, 0, true},
        {6, 0, 0, 0, 0, true},
    };
    const std::vector<std::string> nombres = {
        "equipo_n8", "equipo_n6", "equipo_n10", "relajada_n8", "sin_restricciones_n6"};

    for (std::size_t indice = 0; indice < politicas.size(); ++indice) {
        bt::BacktrackingEngine motor(politicas[indice]);
        const auto con_poda = motor.resolver(true);
        const auto sin_poda = motor.resolver(false);

        if (con_poda.soluciones_encontradas != sin_poda.soluciones_encontradas) {
            throw std::runtime_error("La poda cambio la cantidad de soluciones de BT.");
        }

        escribir_fila_bt(csv, nombres[indice], politicas[indice], true, con_poda);
        escribir_fila_bt(csv, nombres[indice], politicas[indice], false, sin_poda);
    }

    std::cout << "    [OK] Se registraron 5 configuraciones de BT con y sin poda.\n";
}

void verificar_referencia_fb() {
    const std::string hash_referencia =
        "8d51feb34e3e69f6fa6dffc577e2c60490cf9a7fcd835f9f6af1505b71d74773";
    const auto resultado = fb::ataque_fuerza_bruta(hash_referencia, ALFABETO_A2, 5, 5);
    if (!resultado.encontrada || resultado.contrasena != "abc12") {
        throw std::runtime_error("Fallo la validacion de la instancia de referencia de FB.");
    }
    std::cout << "[OK] Instancia de referencia de FB verificada correctamente.\n";
}

}  // namespace

int main() {
    try {
        std::cout << "=====================================================\n";
        std::cout << "    SUITE DE PRUEBAS Y RECOLECCION DE DATOS (ADA P1)\n";
        std::cout << "=====================================================\n";

        verificar_referencia_fb();
        verificar_referencia_bt();
        ejecutar_experimentos_fb();
        ejecutar_experimentos_bt();

        std::cout << "\nProceso completado. CSVs listos en results/.\n";
    } catch (const std::exception& error) {
        std::cerr << "Error durante la ejecucion: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
