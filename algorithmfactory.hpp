#ifndef ALGORITHMFACTORY_HPP
#define ALGORITHMFACTORY_HPP

#include <string>
#include <vector>

class AlgorithmTest;

class AlgorithmFactory
{
public:
    AlgorithmFactory();

    std::vector<std::string> listAlgorithms();
    AlgorithmTest* createAlgorithm(std::string name);

};

#endif // ALGORITHMFACTORY_HPP
