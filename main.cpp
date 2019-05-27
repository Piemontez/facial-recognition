#include "tester.hpp"
#include "algorithmtest.hpp"
#include "facerecognizer.hpp"

#include "tester/3dtester.hpp"
#include "tester/irtester.hpp"
#include "tester/vistester.hpp"

#include <iostream>

int main(int /*argc*/, char **/*argv*/)
{
    std::cout << PROJECT_NAME << " iniciado com sucesso." << std::endl;

//    std::cout << "Procurando por ImageLoader em '" << LOADER_FOLDER << "'." << std::endl;
//    std::cout << "Procurando por ImageProcessor de imagem em '" << PREPROCESSOR_FOLDER << "'." << std::endl;
//    std::cout << "Procurando por FaceRecognier em '" << RECOGNIZER_FOLDER << "'." << std::endl;
//    std::cout << "Procurando por Tester em '" << TESTER_FOLDER << "'." << std::endl;

//    std::cout << "Resultado da busca" << std::endl
//              << "  ImageLoader: " << 0 <<  std::endl
//              << "  ImageProcessor: " << 0 <<  std::endl
//              << "  FaceRecognier: " << 0 <<  std::endl
//              << "  Tester: " << 0 <<  std::endl;

    std::cout << "Coletando Testadores" << std::endl;

    std::vector<Tester *> tests;
    tests.push_back(new ThreeDTester);
    tests.push_back(new IRTester);
    tests.push_back(new VisTester);

    std::cout << "  Total testadores:" << tests.size() << std::endl;
    for(auto && tester: tests)
        std::cout << "  " << tester->name() << std::endl
                  << "    Total Algorithms: " << tester->algorithms().size() << std::endl
                  << "    Total Pre processadores: " << tester->preProcessor().size() << std::endl;

    std::cout << "Iniciando testes" << std::endl;
    for(auto && tester: tests)
        tester->run();

    return 0;
}
