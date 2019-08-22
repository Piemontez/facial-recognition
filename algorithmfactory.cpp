#include "algorithmfactory.hpp"
#include "facerecognizer.hpp"

#include "recognizer/pca.hpp"
#include "recognizer/lda.hpp"

AlgorithmFactory::AlgorithmFactory()
{

}

AlgorithmFactory *AlgorithmFactory::inst = nullptr;
AlgorithmFactory *AlgorithmFactory::instance()
{
    if (!inst) inst = new AlgorithmFactory();
    return inst;
}

/*std::vector<std::string> AlgorithmFactory::listAlgorithms()
{

}

Recognizer *AlgorithmFactory::createAlgorithm(std::string name)
{

}*/

std::vector<Recognizer *> AlgorithmFactory::createAllAlgorithm()
{
    std::vector<Recognizer *> rs;
    rs.push_back(new PCA);
    rs.push_back(new LDA);
    return rs;
}

