#include "tester.hpp"
#include "facerecognizer.hpp"
#include "algorithmfactory.hpp"
#include "imageloader.hpp"
#include "imageprocessor.hpp"

#include <iostream>
#include <map>
#include <opencv4/opencv2/opencv.hpp>

struct TesterPrivate {
    int leaveOneOutGroupSize{5};

    std::vector<cv::Mat> images;
    std::vector<int> labels;
    std::vector<int> flags;

    std::vector<AlgorithmTest*> tester;
    std::vector<ImageProcessor *> imgsProcessorOrdered;
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

std::vector<ImageProcessor *> Tester::preProcessors()
{
    std::vector<ImageProcessor *> rs;

    rs.insert(rs.begin(), d_ptr->imgsProcessorOrdered.begin(), d_ptr->imgsProcessorOrdered.end());
    rs.insert(rs.begin(), d_ptr->imgsProcessor.begin(), d_ptr->imgsProcessor.end());

    return rs;
}

void Tester::addPreProcessorOrderFixed(ImageProcessor *processor)
{
    d_ptr->imgsProcessorOrdered.push_back(processor);
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
        this->d_ptr->flags = imageLoader()->flags();
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
    std::cout << "  Total de pre-processadores:" << preProcessors().size() << std::endl;

    std::vector<std::vector<ImageProcessor *>> _permutations = permutations(
                d_ptr->imgsProcessorOrdered,
                d_ptr->imgsProcessor);
    std::vector<std::vector<int>> testGroups = leaveOneOutGroups();
    std::vector<Recognizer *> recogs = AlgorithmFactory::instance()->createAllAlgorithm();

    std::cout << "  Total de permutações:" << _permutations.size() << std::endl;
    std::cout << "  Total de testes por permutação:" << testGroups.size() << std::endl;
    std::cout << "  Total de amostras por teste:" << (testGroups.size() ? (images().size() / testGroups.size()) : 0) << std::endl;
    std::cout << "  Total de reconhecedores:" << recogs.size() << std::endl;

    std::cout << "  Iniciando pré processamento." << std::endl;
    //Percorre as imagens e aplica os pré-processadors por permutação

    std::vector< std::string > recogsNames;
    std::vector< std::string > processorsNames;
    std::vector< std::tuple<int, int, int, int> > resultTests; //lista de <VP, FP, FN, VN>;

    //percorre as permutações de pré processamento
    for (auto && perms: _permutations) {
        std::cout << "    Realizando pré-processando." << std::endl;
        std::string processorName;
        std::vector<cv::Mat> imgProcessed;
        std::vector<cv::Mat> _recogTrainImgs, _recogTestImgs, _compareTestImgs;
        std::vector<int> _recogTrainLabels, _recogTestLabels, _compareTestLabels;
        std::vector<std::tuple<cv::Mat, cv::Mat>> _compareTrainImgs;
        std::vector<int> _compareTrainLabels;


        for (auto && pre: perms) {
            processorName.append(" ").append(pre->name());
        }
        std::cout << "    Permutação:" << processorName << std::endl;

        //Realiza os pré-processamentos da imagem
        for (auto img: this->d_ptr->images) {
            //cv::imshow("original", img);

            for (auto && pre: perms) {
                img = pre->proccess(img.clone());
            }
            if (img.channels() > 1) {
                cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
            }

            imgProcessed.push_back(img);

            //cv::imshow("processed", img);
            //cv::waitKey();
        }

        continue;
        //Todo: remover para inicar treinamento


        //Percorre os algorítmos de reconhecimento e realiza os testes de reconhecimento
        for (auto && recog: recogs) {
            std::cout << "    Reconhecedor: " << recog->algorithmName() << std::endl;
            std::cout << "    Separando imagens para trainamento." << std::endl;

            for (int testPos = 0; testPos < testGroups.size(); testPos++)
            {
                int groupPos = 0;
                _recogTrainImgs.clear();
                _recogTrainLabels.clear();
                _recogTestImgs.clear();
                _recogTestLabels.clear();
                _compareTrainImgs.clear();
                _compareTrainLabels.clear();
                _compareTestImgs.clear();
                _compareTestLabels.clear();

                //percorre os grupos de testes e adiciona na lista de trainamento ou na lista para testes
                for (auto labels: testGroups) {
                    if (groupPos == testPos) {
                        for (auto imgPos: labels) {
                            _recogTestImgs.push_back(imgProcessed[imgPos]);
                            _recogTestLabels.push_back(this->d_ptr->labels[imgPos]);
                        }
                    } else {
                        for (auto imgPos: labels) {
                            _recogTrainImgs.push_back(imgProcessed[imgPos]);
                            _recogTrainLabels.push_back(this->d_ptr->labels[imgPos]);
                        }
                    }
                    groupPos++;
                }

                recog->resetTrain();
                recog->train(_recogTrainImgs, _recogTrainLabels);

                //std::cout << "    Realizando teste de reconhecimento:" << testPos << std::endl;
                int posTest = 0;
                int VP = 0, FP = 0, FN = 0, VN = 0;
                for (auto trainImg: _recogTrainImgs) {
                    testSensitivitiesSpecificity(true,
                                                 recog->predict(trainImg),
                                                 _recogTrainLabels[posTest],
                                                 VP, FP, FN, VN);
                    posTest++;
                }
                posTest = 0;
                for (auto testImg: _recogTestImgs) {
                    int realLabel = _recogTestLabels[posTest];

                    testSensitivitiesSpecificity(std::find(_recogTrainLabels.begin(), _recogTrainLabels.end(), realLabel) != _recogTrainLabels.end(),
                                                 recog->predict(testImg),
                                                 realLabel,
                                                 VP, FP, FN, VN);
                    posTest++;
                }

                //Contabiliza o teste
                recogsNames.push_back(recog->algorithmName());
                processorsNames.push_back(processorName);
                resultTests.push_back(std::make_tuple(VP, FP, FN, VN));

                //std::cout << "    Realizando teste de comparacao:" << testPos << std::endl;
                /*
                posTest = 0;
                VP = 0; FP = 0; FN = 0; VN = 0;
                for (auto testImg: _testImgs)
                {
                    if (test(testImg, testImg) == _testLabels[posTest]) {

                    } else {

                    }
                    posTest++;
                }
                */

                //recog->algorithmName();
                //testName
            }
        }
    }

    showResults(recogsNames, processorsNames, resultTests);
}

/**
 * calcula as permutações
 */
std::vector<std::vector<ImageProcessor *> > Tester::permutations(
        const std::vector<ImageProcessor *> &processorsOrderFixed,
        const std::vector<ImageProcessor *> &processors)
{
    std::vector<std::vector<ImageProcessor *>> rs;
    std::vector<ImageProcessor *> _processors = processors;

    //Ordena processadores
    std::sort(_processors.begin(), _processors.end());
    do {
      //inicia lista com processadores com ordem fixada
      std::vector<ImageProcessor *> _perm;
      _perm.insert(_perm.end(), processorsOrderFixed.begin(), processorsOrderFixed.end());
      _perm.insert(_perm.end(), _processors.begin(), _processors.end());

      rs.push_back(_perm);

      //for (auto && item: _processors) std::cout << "  " << (item ? item->name() : "-");
      //std::cout << std::endl;
    } while ( std::next_permutation(_processors.begin(), _processors.end()) );

    //Cria lista de permutações com um item a menos.
    int size = _processors.size();
    for (int pos = 0; pos < size; pos++) {
        std::vector<ImageProcessor *> _nextProcessors = _processors;
        _nextProcessors.erase(_nextProcessors.begin() + pos);

        if (_nextProcessors.size()) {
            std::vector<std::vector<ImageProcessor *> > others = permutations(processorsOrderFixed, _nextProcessors);
            rs.insert(rs.end(), others.begin(), others.end());
        }
    }

    return rs;
}

/**
 * Cria grupo de testes
 * Lista composta pelos rótulos das faces
 */
std::vector<std::vector<int> > Tester::leaveOneOutGroups()
{
    std::vector< std::vector<int> > res;

    std::vector<int> sortedTrain;
    int pos = -1;
    for (int flag: this->d_ptr->flags) {
        pos++;
        int label = this->d_ptr->labels[pos];
        if ((flag | RECOG_TRAIN) && !std::binary_search(sortedTrain.begin(), sortedTrain.end(), label))
            sortedTrain.push_back(label);
    }

    int imagesPerGroup = sortedTrain.size() / d_ptr->leaveOneOutGroupSize;

    for (int start = 0; start < d_ptr->leaveOneOutGroupSize; start++) {
        std::vector<int>::iterator begin = sortedTrain.begin() + (start * imagesPerGroup);
        std::vector<int>::iterator end = ((1+start) * imagesPerGroup) > sortedTrain.size()
                ? sortedTrain.end()
                : sortedTrain.begin() + ((1+start) * imagesPerGroup);

        res.push_back(std::vector<int> ( begin, end ));
    }

    return res;
}

void Tester::testSensitivitiesSpecificity(bool inTrain, int predictedLabel, int realLabel, int &VP, int &FP, int &FN, int &VN)
{
    if (inTrain) {
        if (predictedLabel == realLabel) {
            VP++;
        } else if (predictedLabel == -1) {
            FN++;
        } else {
            FP++;
        }
    } else {
        if (predictedLabel == realLabel) {
            VP++;
        } else if (predictedLabel == -1) {
            VN++;
        } else {
            FP++;
        }
    }
}



void Tester::saveTest()
{

}

void Tester::showResults(std::vector< std::string > recogsNames, std::vector< std::string > processorsNames, std::vector< std::tuple<int, int, int, int> > resultTests)
{
    std::cout << "<-- All Resultados -->" << std::endl;

    std::map< std::string, std::tuple<int, int, int, int, int> > medias;

    int VP, FP, FN, VN, testes;

    auto itRecogs = recogsNames.begin();
    auto itProcs = processorsNames.begin();
    auto itResults = resultTests.begin();

    while (itRecogs != recogsNames.end()) {
        auto recogName = *itRecogs;
        auto procName  = *itProcs;
        auto results   = *itResults;

        std::tie(VP, FP, FN, VN) = results;

        std::string testName = "<" + procName + " > " + recogName;

        if (medias.find(testName)  == medias.end()) {
            medias[testName] = std::make_tuple(VP, FP, FN, VN, 1);
        } else {
            std::get<0>(medias[testName]) += VP;
            std::get<1>(medias[testName]) += FP;
            std::get<2>(medias[testName]) += FN;
            std::get<3>(medias[testName]) += VN;
            std::get<4>(medias[testName]) += 1;
        }

        std::cout << testName << " : " << VP << " " << FP << " " << FN << " " << VN << std::endl;

        itRecogs++;
        itProcs++;
        itResults++;
    }

    std::cout << "<-- Avarage Resultados -->" << std::endl;
    std::map< std::string, std::tuple<int, int, int, int, int> >::iterator itMedia = medias.begin();
    while (itMedia != medias.end()) {
        std::tie(VP, FP, FN, VN, testes) = itMedia->second;

        std::cout << itMedia->first << " : " << (VP/testes) << " " << (FP/testes) << " " << (FN/testes) << " " << (VN/testes) <<  std::endl;

        itMedia++;
    }

}
