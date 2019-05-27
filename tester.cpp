#include "tester.hpp"
#include "algorithmtest.hpp"
#include "imageloader.hpp"
#include "imageprocessor.hpp"

#include <opencv4/opencv2/core/core.hpp>

#include <iostream>

struct TesterPrivate {
    std::vector<AlgorithmTest*> tester;
    std::vector<ImageProcessor *> imgsProcessor;
    ImageLoader *imgLoader{nullptr};
};

Tester::Tester() : d_ptr(new TesterPrivate)
{

}

std::string Tester::name()
{
    return "No Name";
}

std::vector<cv::Mat> Tester::imagesTrain()
{
    return std::vector<cv::Mat>();
}

std::vector<cv::Mat> Tester::imagesTest()
{
    return std::vector<cv::Mat>();
}

std::vector<AlgorithmTest*> Tester::algorithms()
{
    return d_ptr->tester;
}

void Tester::addAlgorithm(AlgorithmTest *test)
{
    d_ptr->tester.push_back(test);
}

std::vector<ImageProcessor *> Tester::preProcessor()
{
    return d_ptr->imgsProcessor;
}

void Tester::addPreProcessor(ImageProcessor *processor)
{
    d_ptr->imgsProcessor.push_back(processor);
}

ImageLoader *Tester::imageLoader()
{
    return d_ptr->imgLoader;
}

void Tester::setImageLoader(ImageLoader *processor)
{
    d_ptr->imgLoader = processor;
}

void Tester::run()
{
    std::cout << "Teste " << name() << std::endl;

    std::cout << "  Carregando imagens" << std::endl;

    if (imageLoader()) {
        imageLoader()->loadTrainingImages();
        imageLoader()->loadTestImages();
    }

    if (!imagesTrain().size()){
        std::cout << "    Nenhuma imagem de trainamento carregada" << std::endl;
        std::cout << "  Teste finalizado" << std::endl;
        return;
    }
    if (!imagesTest().size()){
        std::cout << "    Nenhuma imagem de testes carregada" << std::endl;
        std::cout << "  Teste finalizado" << std::endl;
        return;
    }
}
