#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <cmath>
#include <cstddef>
#include "../src/fb_bruteforce.hpp"
#include "../src/bt_backtracking.hpp"

// Generador congruencial lineal para la semilla 3772 del equipo
std::vector<std::string> generar_contrasenas_equipo() {
    long long semilla = 3772; // Calculada determinísticamente
    long long x = semilla;
    
    std::string A1 = "abcdefghijklmnopqrstuvwxyz";
    std::string A2 = "abcdefghijklmnopqrstuvwxyz0123456789";
    
    // Ajustado a std::size_t para coherencia de tipos en C++17
    std::vector<std::size_t> longitudes = {4, 4, 5, 5, 6};
    std::vector<std::string> alfabetos = {A1, A2, A1, A2, A1};
    std::vector<std::string> contrasenas;

    for (std::size_t i = 0; i < 5; ++i) {
        std::string pwd = "";
        std::string alf = alfabetos[i];
        for (std::size_t j = 0; j < longitudes[i]; ++j) {
            x = (1103515245LL * x + 12345LL) % 2147483648LL; // 2^31
            pwd += alf[static_cast<std::size_t>(x) % alf.size()];
        }
        contrasenas.push_back(pwd);
    }
    return contrasenas;
}

void ejecutar_experimentos_fb() {
    std::cout << "[+] Ejecutando experimentos de Fuerza Bruta (FB)...\n";
    std::ofstream csv("results/tiempos_fb.csv");
    csv << "instancia,contrasena_real,hash_sha256,alfabeto_tam,longitud,intentos_fb,tiempo_fb_us,encontrada_dict,intentos_dict,tiempo_dict_us\n";

    std::vector<std::string> pwds = generar_contrasenas_equipo();
    std::string A1 = "abcdefghijklmnopqrstuvwxyz";
    std::string A2 = "abcdefghijklmnopqrstuvwxyz0123456789";
    std::vector<std::string> alfabetos = {A1, A2, A1, A2, A1};
    std::vector<std::size_t> longitudes = {4, 4, 5, 5, 6};

    for (std::size_t i = 0; i < pwds.size(); ++i) {
        std::string pwd = pwds[i];
        std::string hash = fb::sha256(pwd);
        std::string alf = alfabetos[i];
        std::size_t len = longitudes[i];

        // 1. Ejecución Fuerza Bruta Pura
        auto res_fb = fb::ataque_fuerza_bruta(hash, alf, 1, len);
        
        // 2. Ejecución Ataque por Diccionario
        auto res_dict = fb::ataque_diccionario(hash, "resources/diccionario.txt");

        csv << (i + 1) << ","
            << pwd << ","
            << hash << ","
            << alf.size() << ","
            << len << ","
            << res_fb.intentos << ","
            << res_fb.tiempo_microsegundos << ","
            << (res_dict.encontrada ? "si" : "no") << ","
            << res_dict.intentos << ","
            << res_dict.tiempo_microsegundos << "\n";
    }

    csv.close();
    std::cout << "    [->] Archivo 'results/tiempos_fb.csv' generado exitosamente.\n";
}

void ejecutar_experimentos_bt() {
    std::cout << "[+] Ejecutando experimentos de Backtracking (BT)...\n";
    std::ofstream csv("results/tiempos_bt.csv");
    csv << "variante,longitud,poda_activa,nodos_visitados,nodos_generados,soluciones,tiempo_us,porcentaje_reduccion\n";

    // 5 variantes exigidas por la Sección 9.2 de la guía
    std::vector<bt::PoliticaConfig> politicas = {
        {8, 3, 1, 2, 1, true},  // (i) Política completa del equipo (n=8)
        {6, 3, 1, 2, 1, true},  // (ii) Misma política (n=6)
        {10, 3, 1, 2, 1, true}, // (iii) Misma política (n=10)
        {8, 1, 0, 0, 0, false}, // (iv) Política relajada (minLower=1, sin repetidos desactivado)
        {6, 0, 0, 0, 0, false}  // (v) Política sin restricciones (n=6, calibración de poda nula)
    };

    std::vector<std::string> nombres = {
        "equipo_n8", "equipo_n6", "equipo_n10", "relajada_n8", "sin_restricciones_n6"
    };

    for (std::size_t i = 0; i < politicas.size(); ++i) {
        bt::BacktrackingEngine engine(politicas[i]);
        
        // Con poda
        auto res_poda = engine.resolver(true);
        csv << nombres[i] << "," << politicas[i].longitud << ",true,"
            << res_poda.nodos_visitados << "," << res_poda.nodos_generados << ","
            << res_poda.soluciones_encontradas << "," << res_poda.tiempo_microsegundos << ","
            << res_poda.porcentaje_reduccion << "\n";

        // Sin poda
        auto res_sin_poda = engine.resolver(false);
        csv << nombres[i] << "," << politicas[i].longitud << ",false,"
            << res_sin_poda.nodos_visitados << "," << res_sin_poda.nodos_generados << ","
            << res_sin_poda.soluciones_encontradas << "," << res_sin_poda.tiempo_microsegundos << ","
            << 0.0 << "\n";
    }

    csv.close();
    std::cout << "    [->] Archivo 'results/tiempos_bt.csv' generado exitosamente.\n";
}

int main() {
    try {
        std::cout << "=====================================================\n";
        std::cout << "    SUITE DE PRUEBAS Y RECOLECCION DE DATOS (ADA P1)  \n";
        std::cout << "=====================================================\n";

        // Validación previa de Instancia de Referencia
        std::string hash_ref = "8d51feb34e3e69f6fa6dffc577e2c60490cf9a7fcd835f9f6af1505b71d74773";
        std::string A2 = "abcdefghijklmnopqrstuvwxyz0123456789";
        auto ref_test = fb::ataque_fuerza_bruta(hash_ref, A2, 5, 5);

        if (ref_test.encontrada && ref_test.contrasena == "abc12") {
            std::cout << "[OK] Instancia de referencia (abc12) verificada correctamente.\n\n";
        } else {
            std::cerr << "[ERROR] Fallo en la verificacion de la contraseña de referencia (abc12).\n";
            return 1;
        }

        // Generar reportes CSV
        ejecutar_experimentos_fb();
        ejecutar_experimentos_bt();

        std::cout << "\n=====================================================\n";
        std::cout << " Proceso completado. Archivos CSV en 'results/' listos.\n";
        std::cout << "=====================================================\n";

    } catch (const std::exception& e) {
        std::cerr << "Error durante la ejecucion: " << e.what() << "\n";
        return 1;
    }
    return 0;
}