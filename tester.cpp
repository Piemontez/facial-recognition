#include "tester.hpp"
#include "algorithmtest.hpp"
#include "imageloader.hpp"
#include "imageprocessor.hpp"

#include "opencv4/opencv2/opencv.hpp"

#include <iostream>

struct TesterPrivate {
    std::vector<cv::Mat> images;
    std::vector<int> labels;

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

std::vector<cv::Mat> Tester::images()
{
    return this->d_ptr->images;
}

std::vector<int> Tester::labels()
{
    return this->d_ptr->labels;
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
    std::cout << "< TESTE: " << name() << " >" << std::endl;

    std::cout << "  Carregando imagens." << std::endl;

    if (imageLoader()) {
        this->d_ptr->images = imageLoader()->images();
        this->d_ptr->labels = imageLoader()->labels();
    }

    if (!images().size()){
        std::cout << "  Nenhuma imagem carregada." << std::endl;
        std::cout << "  Teste finalizado." << std::endl;
        return;
    }

    if (images().size() != labels().size()) {
        std::cout << "  A quantidade de labels é diferente da quantidade de imagens." << std::endl;
        std::cout << "  Teste finalizado." << std::endl;
        return;
    }

    std::cout << "  Total de imagens:" << images().size() << std::endl;
    std::cout << "  Total de pre-processadores:" << preProcessor().size() << std::endl;


    //calcula as permutações
    std::vector<std::vector<ImageProcessor *>> permutations;
    std::vector<ImageProcessor *> pres = Tester::preProcessor();

    std::sort(pres.begin(), pres.end());
    do {
      permutations.push_back(pres);

      for (auto && item: pres) std::cout << item->name();
      std::cout << std::endl;
    } while ( std::next_permutation(pres.begin(), pres.end()) );



    for (auto && img: images()) {
        cv::imshow("raw", img);

        int pos = 0;
        for (auto && pre: preProcessor()) {
            img = pre->proccess(img);

            cv::imshow(std::string("allProcess") + ("" + pos++), img);
        }
    }

    cv::waitKey();
}
