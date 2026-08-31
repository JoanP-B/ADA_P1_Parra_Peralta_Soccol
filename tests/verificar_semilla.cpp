#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

int main() {
    // Apellidos ordenados alfabéticamente
    std::vector<std::string> apellidos = {"parraelmasri", "peraltabedoya", "soccolmejia"};
    std::sort(apellidos.begin(), apellidos.end());

    std::string cadena_concatenada = "";
    for (const auto& ap : apellidos) {
        cadena_concatenada += ap;
    }

    long long suma_ascii = 0;
    for (char c : cadena_concatenada) {
        suma_ascii += static_cast<int>(c);
    }

    long long semilla = suma_ascii % 100000;

    // Cálculo de los parámetros de la política del Módulo BT
    int min_lower = 2 + static_cast<int>(semilla % 3);
    int min_upper = 1 + static_cast<int>(semilla % 2);
    int min_digit = 1 + static_cast<int>(semilla % 3);
    int min_symbol = 1; // Fijo por la guía
    int suma_minimos = min_lower + min_upper + min_digit + min_symbol;

    std::cout << "==========================================" << std::endl;
    std::cout << "  VERIFICACION DE SEMILLA Y PARAMETROS BT  " << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "Cadena concatenada: " << cadena_concatenada << std::endl;
    std::cout << "Suma ASCII Total:   " << suma_ascii << std::endl;
    std::cout << "SEMILLA OFICIAL:    " << semilla << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "PARAMETROS DE LA POLITICA BT (n = 8):" << std::endl;
    std::cout << "  - minLower  = 2 + (" << semilla << " % 3) = " << min_lower << std::endl;
    std::cout << "  - minUpper  = 1 + (" << semilla << " % 2) = " << min_upper << std::endl;
    std::cout << "  - minDigit  = 1 + (" << semilla << " % 3) = " << min_digit << std::endl;
    std::cout << "  - minSymbol = " << min_symbol << " (Fijo)" << std::endl;
    std::cout << "------------------------------------------" << std::endl;
    if (suma_minimos <= 8) {
        std::cout << "Estado: VALIDO - Cumple minLower + minUpper + minDigit + minSymbol <= 8" << std::endl;
    } else {
        std::cout << "Estado: REQUIERE REAJUSTE - La suma supera la longitud n = 8" << std::endl;
    }
    std::cout << "==========================================" << std::endl;

    return 0;
}