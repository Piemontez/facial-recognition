#include "tester.hpp"

#include <iostream>

int main(int /*argc*/, char **/*argv*/)
{
    std::cout << PROJECT_NAME << " iniciado com sucesso." << std::endl;

    std::cout << "Procurando por carragadores de imagens em '" << LOADER_FOLDER << "'." << std::endl;

    std::cout << "Procurando por pré-processadores de imagem em '" << PREPROCESSOR_FOLDER << "'." << std::endl;

    std::cout << "Procurando por algoritmos de reconhecimento em '" << RECOGNIZER_FOLDER << "'." << std::endl;

    std::cout << "Procurando por algorítmos testadores em '" << TESTER_FOLDER << "'." << std::endl;

    new Tester;



    return 0;
}
