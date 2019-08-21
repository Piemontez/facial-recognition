#include "tester.hpp"
#include "algorithmtest.hpp"
#include "imageloader.hpp"
#include "imageprocessor.hpp"

#include "opencv4/opencv2/opencv.hpp"

#include <iostream>

struct TesterPrivate {
    int leaveOneOutGroupSize{5};

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

    std::vector<std::vector<ImageProcessor *>> _permutations = permutations(preProcessor());
    std::vector<std::vector<int>> testGroups = leaveOneOutGroups(images().size());

    std::cout << "  Total de permutações:" << _permutations.size() << std::endl;
    std::cout << "  Total de testes por permutação:" << testGroups.size() << std::endl;
    std::cout << "  Total de amostras por teste:" << (testGroups.size() ? (images().size() / testGroups.size()) : 0) << std::endl;
    std::cout << "  Total de permutações:" << _permutations.size() << std::endl;

    std::cout << "  Iniciando pré processamento." << std::endl;
    //Percorre as imagens e aplica os pré-processadors por permutação
    int perm = 0;
    for (auto && perms: _permutations) {
        std::cout << "    Pré processamento da permutação:" << ++perm << std::endl;
        std::vector<cv::Mat> imgProcessed;
        for (auto && img: this->d_ptr->images) {
            for (auto && pre: perms) {
                img = pre->proccess(img.clone());
            }
            imgProcessed.push_back(img);
        }

        std::cout << "    Separando imagens para trainamento." << std::endl;
        std::vector<cv::Mat> train, test;
        std::vector<int> trainLabels, testLabels;

        //realiza um teste para cada grupo
        for (int testPos = 0; testPos < testGroups.size(); testPos++)
        {
            int groupPos = 0;
            //percorre os grupos de testes e adiciona lista de trainamento ou na lista para testes
            for (auto group: testGroups)
            {
                if (groupPos == testPos) {
                    for (auto imgPos: group) {
                        test.push_back(imgProcessed[imgPos]);
                        testLabels.push_back(this->d_ptr->labels[imgPos]);
                    }
                } else {
                    for (auto imgPos: group) {
                        train.push_back(imgProcessed[imgPos]);
                        trainLabels.push_back(this->d_ptr->labels[imgPos]);
                    }
                }



                groupPos++;
            }



        }

    }
}

/**
 * calcula as permutações
 */
std::vector<std::vector<ImageProcessor *> > Tester::permutations(const std::vector<ImageProcessor *> &processors)
{
    std::vector<ImageProcessor *> _processors = processors;

    std::vector<std::vector<ImageProcessor *>> _permutations;

    std::sort(_processors.begin(), _processors.end());
    do {
      _permutations.push_back(_processors);

      //for (auto && item: _processors) std::cout << "  " << (item ? item->name() : "-");
      //std::cout << std::endl;
    } while ( std::next_permutation(_processors.begin(), _processors.end()) );

    int size = _processors.size();
    for (int pos = 0; pos < size; pos++) {
        std::vector<ImageProcessor *> _nextProcessors = _processors;
        _nextProcessors.erase(_nextProcessors.begin() + pos);

        if (_nextProcessors.size()) {
            std::vector<std::vector<ImageProcessor *> > others = permutations(_nextProcessors);

            _permutations.insert(_permutations.end(), others.begin(), others.end());
        }
    }

    return _permutations;
}

std::vector<std::vector<int> > Tester::leaveOneOutGroups(int imageSize)
{
    std::vector< std::vector<int> > res;

    std::vector<int> sorted(imageSize);
    for (int x = 0; x< imageSize; x++) {
        sorted[x] = x;
    }

    int imagesPerGroup = imageSize / d_ptr->leaveOneOutGroupSize;

    for (int start = 0; start < d_ptr->leaveOneOutGroupSize; start++) {
        std::vector<int>::iterator begin = sorted.begin() + (start * d_ptr->leaveOneOutGroupSize);
        std::vector<int>::iterator end = (start * d_ptr->leaveOneOutGroupSize) > sorted.size()
                ? sorted.end()
                : sorted.begin() + (start * d_ptr->leaveOneOutGroupSize);

        res.push_back(std::vector<int> ( begin, end ));
    }

    return res;
}

void Tester::testSensitivitiesSpecificity()
{

}
