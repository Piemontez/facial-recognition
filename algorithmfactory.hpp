#ifndef ALGORITHMFACTORY_HPP
#define ALGORITHMFACTORY_HPP

#include <string>
#include <vector>

class Recognizer;

class AlgorithmFactory
{
    static AlgorithmFactory *inst;
    AlgorithmFactory();
public:
    static AlgorithmFactory *instance();

    /*std::vector<std::string> listAlgorithms();
    Recognizer* createAlgorithm(std::string name);*/

    std::vector<Recognizer *> createAllAlgorithm();
};

#endif // ALGORITHMFACTORY_HPP
