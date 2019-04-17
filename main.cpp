#include "tester.hpp"

#include <iostream>

int main(int /*argc*/, char **/*argv*/)
{
    std::cout << PROJECT_NAME << " iniciado com sucesso." << std::endl;

    std::cout << "Procurando por ImageLoader em '" << LOADER_FOLDER << "'." << std::endl;
    std::cout << "Procurando por ImageProcessor de imagem em '" << PREPROCESSOR_FOLDER << "'." << std::endl;
    std::cout << "Procurando por FaceRecognier em '" << RECOGNIZER_FOLDER << "'." << std::endl;
    std::cout << "Procurando por Tester em '" << TESTER_FOLDER << "'." << std::endl;


    std::cout << "Resultado da busca" << std::endl
              << "  ImageLoader: " << 0 <<  std::endl
              << "  ImageProcessor: " << 0 <<  std::endl
              << "  FaceRecognier: " << 0 <<  std::endl
              << "  Tester: " << 0 <<  std::endl;

    std::cout << "Iniciando Testes" << std::endl;

    return 0;
}
