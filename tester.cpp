#include "tester.hpp"
#include "facerecognizer.hpp"
#include "algorithmfactory.hpp"
#include "imageloader.hpp"
#include "imageprocessor.hpp"
#include "tools.hpp"

#include <iostream>
#include <map>
#include <opencv4/opencv2/opencv.hpp>

struct Image {
    cv::Mat image;
    int label;
    int flags;
    cv::Mat processed;

    Image(cv::Mat& image, int label, int flags)
    {
        this->image = image;
        this->label = label;
        this->flags = flags;
    }
};

struct Compare {
    cv::Mat l;
    cv::Mat r;
    Compare(cv::Mat &l, cv::Mat &r)
    {
        this->l = l;
        this->r = r;
    }
};

struct TesterPrivate {
    int leaveOneOutGroupSize{5};

    std::vector< Image > images;
    //std::vector<cv::Mat> images;
    //std::vector<int> labels;
    //std::vector<int> flags;

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
    std::vector<cv::Mat> images;

    for (auto tp: this->d_ptr->images) {
        images.push_back(tp.image);
    }

    return images;
}

std::vector<int> Tester::labels()
{
    std::vector<int> labels;

    for (auto tp: this->d_ptr->images) {
        labels.push_back(tp.label);
    }

    return labels;
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
        auto images = imageLoader()->images();
        auto labels = imageLoader()->labels();
        auto flags = imageLoader()->flags();
        std::vector<cv::Mat>::iterator imIt = images.begin();
        std::vector<int>::const_iterator lbIt = labels.begin();
        std::vector<int>::iterator flIt = flags.begin();

        for(;imIt != images.end();)
        {
            this->d_ptr->images.push_back(
                Image(*imIt, *lbIt, *flIt)
            );

            imIt++;
            lbIt++;
            flIt++;
        }
    }

    if (!images().size()){
        std::cout << "  Nenhuma imagem carregada." << std::endl;
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

    this->addCsvHeader();

    //percorre as permutações de pré processamento
    for (auto && perms: _permutations) {
        std::cout << "    Realizando pré-processando." << std::endl;
        std::string processorName;
        std::vector<cv::Mat> _recogTrainImgs, _recogTestImgs;
        std::vector<Compare> _compareTrainImgs, _compareTestImgs;
        std::vector<int> _recogTrainLabels, _recogTestLabels,
                        _compareTrainLabels, _compareTestLabels;


        for (auto && pre: perms) {
            processorName.append(" ").append(pre->name());
        }
        std::cout << "    Permutação:" << processorName << std::endl;

        //cv::imwrite();
        {//Realiza os pré-processamentos da imagem
            int pos = 0;
            for (auto && tp: this->d_ptr->images) {
                auto img = tp.image;
                tools::saveImgProc(img, "-Original", pos, 0);

                int permPos = 1;
                std::string name;
                for (auto && pre: perms) {
                    img = pre->proccess(img.clone(), pos, imageLoader());

                    name += "-" + pre->name();
                    tools::saveImgProc(img, name, pos, permPos++);
                }

                if (img.channels() > 1) {
                    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
                }

                tp.processed = img;

                pos++;
            }
        }

        //Todo: remover para inicar treinamento


        //Percorre os algorítmos de reconhecimento e realiza os testes de reconhecimento
        for (auto && recog: recogs) {
            std::cout << "    Reconhecedor: " << recog->algorithmName() << std::endl;
            for (int testPos = 0; testPos < testGroups.size(); testPos++)
            {
                std::cout << "    Grupo teste: " << testPos << std::endl;

                int groupPos = 0;
                int64_t timeTrainig = 0;
                int64_t timeRecog = 0;
                _recogTrainImgs.clear();
                _recogTrainLabels.clear();
                _recogTestImgs.clear();
                _recogTestLabels.clear();
                _compareTrainImgs.clear();
                _compareTrainLabels.clear();
                _compareTestImgs.clear();
                _compareTestLabels.clear();

                std::cout << "    Separando imagens para trainamento." << std::endl;
                //percorre os grupos de testes e adiciona na lista de trainamento ou na lista para testes
                cv::Mat compareTrain;
                int lastLabel = -1;
                for (auto labelsTest: testGroups) {/*grupos de testes*/
                    for (auto labelTest: labelsTest) {/*labels do grupo*/
                        for (auto tp: this->d_ptr->images) {
                            if (labelTest == tp.label) {
                                if (tp.flags & COMPARE_MAIN_TRAIN) {
                                    compareTrain = tp.processed;
                                }

                                if (groupPos == testPos) {
                                    _recogTestImgs.push_back(tp.processed);
                                    _recogTestLabels.push_back(tp.label);
                                } else {
                                    _recogTrainImgs.push_back(tp.processed);
                                    _recogTrainLabels.push_back(tp.label);
                                }

                                if (lastLabel == tp.label) {
                                    if ((tp.flags & COMPARE_TRAIN) && groupPos != testPos) {
                                        _compareTrainImgs.push_back(Compare(compareTrain, tp.processed));
                                        _compareTrainLabels.push_back(0);
                                    }
                                    if ((tp.flags & COMPARE_TEST) && groupPos == testPos) {
                                        _compareTestImgs.push_back(Compare(compareTrain, tp.processed));
                                        _compareTestLabels.push_back(0);
                                    }
                                } else if (lastLabel > -1) {
                                    if ((tp.flags & COMPARE_TRAIN) && groupPos != testPos) {
                                        _compareTrainImgs.push_back(Compare(compareTrain, tp.processed));
                                        _compareTrainLabels.push_back(1);
                                    }
                                    else if ((tp.flags & COMPARE_TEST) && groupPos == testPos) {
                                        _compareTestImgs.push_back(Compare(compareTrain, tp.processed));
                                        _compareTestLabels.push_back(1);
                                    }
                                }
                                lastLabel = tp.label;
                            }
                        }
                    }
                    groupPos++;
                }

                std::cout << "    Realizando treinamento." << std::endl;
                recog->resetTrain();
                timeTrainig = cv::getTickCount();
                recog->train(_recogTrainImgs, _recogTrainLabels);
                timeTrainig = cv::getTickCount() - timeTrainig;

                std::cout << "    Realizando teste de reconhecimento:" << testPos << std::endl;
                int VP = 0, FP = 0, FN = 0, VN = 0;

                int posTest = 0;
                for (auto trainImg: _recogTrainImgs) {
                    int64_t time = cv::getTickCount();
                    int label = recog->predict(trainImg);
                    timeRecog += cv::getTickCount() - time;

                    testSensitivitiesSpecificity(true,
                                                 label,
                                                 _recogTrainLabels[posTest],
                                                 VP, FP, FN, VN);
                    posTest++;
                }

                posTest = 0;
                for (auto testImg: _recogTestImgs) {
                    int64_t time = cv::getTickCount();
                    int label = recog->predict(testImg);
                    timeRecog += cv::getTickCount() - time;

                    int realLabel = _recogTestLabels[posTest];

                    testSensitivitiesSpecificity(std::find(_recogTrainLabels.begin(), _recogTrainLabels.end(), realLabel) != _recogTrainLabels.end(),
                                                 label,
                                                 realLabel,
                                                 VP, FP, FN, VN);
                    posTest++;
                }
                timeRecog = timeRecog / (_recogTrainImgs.size() + _recogTestImgs.size());

                //Contabiliza o teste
                recogsNames.push_back(recog->algorithmName());
                processorsNames.push_back(processorName);
                resultTests.push_back(std::make_tuple(VP, FP, FN, VN));

                this->saveTest("recog", recog->algorithmName(), processorName, timeTrainig, timeRecog, std::make_tuple(VP, FP, FN, VN));

                std::cout << "    Realizando teste de comparacao:" << testPos << std::endl;
                VP = 0, FP = 0, FN = 0, VN = 0;

                recog->resetTrain();
                std::vector<cv::Mat> images;
                for (auto pair:_compareTrainImgs) {
                    cv::Mat out;
                    cv::hconcat(pair.l, pair.r, out);
                    images.push_back(out);
                }
                timeTrainig = cv::getTickCount();
                recog->train(images, _compareTrainLabels);
                timeTrainig = cv::getTickCount() - timeTrainig;

                posTest = 0;
                for (auto testImg: _compareTestImgs) {
                    int64_t time = cv::getTickCount();
                    int label = recog->compare(testImg.l, testImg.r);
                    timeRecog += cv::getTickCount() - time;

                    testSensitivitiesSpecificity(_compareTestLabels[posTest] ? true : false,
                                                 label,
                                                 _compareTestLabels[posTest],
                                                 VP, FP, FN, VN);
                    posTest++;
                }
                timeRecog = timeRecog / _compareTestImgs.size();

                this->saveTest("compare", recog->algorithmName(), processorName, timeTrainig, timeRecog, std::make_tuple(VP, FP, FN, VN));
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
    for (auto tp: this->d_ptr->images) {
        pos++;
        if ((tp.flags | RECOG_TRAIN) && !std::binary_search(sortedTrain.begin(), sortedTrain.end(), tp.label))
            sortedTrain.push_back(tp.label);
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

void Tester::addCsvHeader()
{
    std::vector<std::string > cols;
    cols.push_back("test type");
    cols.push_back("technique");
    cols.push_back("processors");

    // Tempos
    cols.push_back("cpu frequencia");
    cols.push_back("training time");
    cols.push_back("predict time");

    // Results
    cols.push_back("VP");
    cols.push_back("FP");
    cols.push_back("VN");
    cols.push_back("VN");

    tools::appendCsv("results", cols);
}


void Tester::saveTest(std::string type,
                      std::string recogName,
                      std::string processorName,
                      int64_t timeTrainig,
                      int64_t timeRecog,
                      std::tuple<int, int, int, int> resultTest)
{
    int VP, FP, FN, VN;
    std::tie(VP, FP, FN, VN) = resultTest;

    std::vector<std::string > cols;
    cols.push_back(type);
    cols.push_back(recogName);
    cols.push_back(processorName);

    // Tempos
    cols.push_back(std::to_string(static_cast<int64_t>(cv::getTickFrequency())));
    cols.push_back(std::to_string(timeTrainig));
    cols.push_back(std::to_string(timeRecog));

    // Results
    cols.push_back(std::to_string(VP));
    cols.push_back(std::to_string(FP));
    cols.push_back(std::to_string(FN));
    cols.push_back(std::to_string(VN));

    tools::appendCsv("results", cols);
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
