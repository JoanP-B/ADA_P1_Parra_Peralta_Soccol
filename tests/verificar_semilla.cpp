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

    std::cout << "==========================================" << std::endl;
    std::cout << "Cadena concatenada: " << cadena_concatenada << std::endl;
    std::cout << "Suma ASCII Total:  " << suma_ascii << std::endl;
    std::cout << "SEMILLA OFICIAL:   " << semilla << std::endl;
    std::cout << "==========================================" << std::endl;

    return 0;
}