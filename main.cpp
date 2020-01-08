#include "tester.hpp"

#include "tester/3dtester.hpp"
#include "tester/irtester.hpp"
#include "tester/vistester.hpp"

#include <iostream>

int main(int /*argc*/, char **/*argv*/)
{
    std::cout << PROJECT_NAME << " iniciado com sucesso." << std::endl;

    std::vector<Tester *> tests;

    tests.push_back(new ThreeDTester);
    tests.push_back(new IRTester);
    tests.push_back(new VisTester);

    std::cout << "  Total testadores:" << tests.size() << std::endl;
    for (auto && tester: tests)
        std::cout << "  " << tester->name() << std::endl
                  << "    Total Algorithms: " << tester->algorithms().size() << std::endl
                  << "    Total Pre processadores: " << tester->preProcessors().size() << std::endl;

    std::cout << "Iniciando testes" << std::endl;
    for (auto && tester: tests)
        tester->run();

    return 0;
}
