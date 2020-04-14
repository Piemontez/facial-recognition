#include "tester.hpp"
#include "facerecognizer.hpp"
#include "algorithmfactory.hpp"
#include "imageloader.hpp"
#include "imageprocessor.hpp"
#include "tools.hpp"

#include "loader/3dloader.hpp"

#include <iostream>
#include <map>
#include <opencv4/opencv2/opencv.hpp>

#define COMPARE_EQ 1
#define COMPARE_NEQ 2

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

    std::cout << "  Total de imagens:" << this->d_ptr->images.size() << std::endl;
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

        cv::Mat load;
        {//Realiza os pré-processamentos da imagem
            int pos = 0;
            for (auto && tp: this->d_ptr->images) {
                if ((this->name().compare("3D Tester") == 0) && (pos == 381 || pos == 280)) {
                    pos++;
                    continue;
                }

                std::cout << "    Posição: " << pos << std::endl;
                auto img = tp.image;

                load = tools::loadImgProc("-Original-" + this->name(), pos, 0);
                if (load.empty())
                    tools::saveImgProc(img, "-Original-" + this->name(), pos, 0, false);

                std::string name;
                for (auto && pre: perms)
                    name += "-" + pre->name();
                load = tools::loadImgProc(name + "-" + this->name(), pos, perms.size());

                if (load.empty()) {
                    int64_t timeTrainig;
                    int permPos = 0;
                    for (auto && pre: perms) {
                        permPos++;

                        timeTrainig = cv::getTickCount();
                        img = pre->proccess(img.clone(), pos, imageLoader());
                        timeTrainig = cv::getTickCount() - timeTrainig;
                        this->saveTest("processor", this->name(), this->name(), pre->name(), timeTrainig, 0, std::make_tuple(0, 0, 0, 0));
                    }

//                    cv::Mat rgb = ((ThreeLoader*)imageLoader())->imagesRGB()[pos];
//                    cv::imshow("processada", img);
//                    cv::waitKey();
//                    cv::imshow("rgb", rgb);
                    tools::saveImgProc(img, name + "-" + this->name(), pos, permPos);
                } else
                    img = load;

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

                //int groupPos = 0;
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
                cv::Mat compareTrain, compareTrain2, compareTrain3, compareTrain4, compareTrain5, randomTrain;
                int lastLabel = -1, randomLabel = -1;

                for (auto tp: this->d_ptr->images) {
                    if (tp.processed.rows == 0)
                        continue;

                    bool test = false;
                    int groupPos = 0;
                    for (auto labelsTest: testGroups) {/*grupos de testes*/
                        if (groupPos == testPos) {
                            test = std::find(labelsTest.begin(), labelsTest.end(), tp.label) != labelsTest.end();
                            if (test)
                                break;
                        }
                        groupPos++;
                    }

                    if (test || tp.flags & RECOG_TEST) {
                        //Selecionado para teste e imagens não frontais para teste
                        _recogTestImgs.push_back(tp.processed);
                        _recogTestLabels.push_back(tp.label);
                    } else if (tp.flags & RECOG_TRAIN) {
                        //Selecionado para treinamento
                        _recogTrainImgs.push_back(tp.processed);
                        _recogTrainLabels.push_back(tp.label);
                    }

                    if (tp.flags & COMPARE_MAIN_TRAIN) {
                        compareTrain5 = compareTrain4;
                        compareTrain4 = compareTrain3;
                        compareTrain3 = compareTrain2;
                        compareTrain2 = compareTrain;
                        compareTrain = tp.processed;
                    }
                    if ((rand() > rand()) && tp.flags & COMPARE_TRAIN) {
                        randomTrain = tp.processed;
                        randomLabel = tp.label;
                    }
                    else
                    if ((rand() > rand()) && randomLabel == tp.label) {
                        _compareTrainImgs.push_back(Compare(randomTrain, tp.processed));
                        _compareTrainLabels.push_back(COMPARE_EQ);
                    }

                    if (lastLabel == tp.label) {
                        if ((tp.flags & COMPARE_TRAIN) && !test) {
                            _compareTrainImgs.push_back(Compare(compareTrain, tp.processed));
                            _compareTrainLabels.push_back(COMPARE_EQ);
                        }
                        if ((tp.flags & COMPARE_TEST) && test) {
                            _compareTestImgs.push_back(Compare(compareTrain, tp.processed));
                            _compareTestLabels.push_back(COMPARE_EQ);
                        }
                    } else if (lastLabel > -1) {
                        if ((tp.flags & (COMPARE_TRAIN | COMPARE_MAIN_TRAIN)) && !test) {
//                            _compareTrainImgs.push_back(Compare(compareTrain, tp.processed));
//                            _compareTrainLabels.push_back(COMPARE_NEQ);
                            if (compareTrain2.rows && compareTrain3.rows) {
                                if (rand() > rand()) {
                                    _compareTrainImgs.push_back(Compare(compareTrain2, tp.processed));
                                    _compareTrainLabels.push_back(COMPARE_NEQ);
                                } else {
                                    _compareTrainImgs.push_back(Compare(compareTrain3, tp.processed));
                                    _compareTrainLabels.push_back(COMPARE_NEQ);
                                }
                                }
                            if (compareTrain4.rows && compareTrain5.rows) {
                                if (rand() > rand()) {
                                    _compareTrainImgs.push_back(Compare(compareTrain4, tp.processed));
                                    _compareTrainLabels.push_back(COMPARE_NEQ);
                                } else {
                                    _compareTrainImgs.push_back(Compare(compareTrain5, tp.processed));
                                    _compareTrainLabels.push_back(COMPARE_NEQ);
                                }
                            }
                        }
                        else if ((tp.flags & COMPARE_TEST) && test) {
                            _compareTestImgs.push_back(Compare(compareTrain, tp.processed));
                            _compareTestLabels.push_back(COMPARE_NEQ);
                            if (compareTrain2.rows && compareTrain3.rows) {
                                if (rand() > rand()) {
                                    _compareTestImgs.push_back(Compare(compareTrain2, tp.processed));
                                    _compareTestLabels.push_back(COMPARE_NEQ);
                                } else {
                                    _compareTestImgs.push_back(Compare(compareTrain3, tp.processed));
                                    _compareTestLabels.push_back(COMPARE_NEQ);
                                }
                            }
                            if (compareTrain4.rows && compareTrain5.rows) {
                                if (rand() > rand()) {
                                    _compareTestImgs.push_back(Compare(compareTrain4, tp.processed));
                                    _compareTestLabels.push_back(COMPARE_NEQ);
                                } else {
                                    _compareTestImgs.push_back(Compare(compareTrain5, tp.processed));
                                    _compareTestLabels.push_back(COMPARE_NEQ);
                                }
                            }
                        }
                    }
                    lastLabel = tp.label;
                }

                std::cout << "    Realizando treinamento de reconhecimento." << std::endl;
                recog->resetTrain();
                timeTrainig = cv::getTickCount();
                recog->train(_recogTrainImgs, _recogTrainLabels);
                timeTrainig = cv::getTickCount() - timeTrainig;

                std::cout << "    Realizando teste de reconhecimento:" << std::endl;
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
                    bool inTrain = std::find(_recogTrainLabels.begin(), _recogTrainLabels.end(), realLabel) != _recogTrainLabels.end();

                    testSensitivitiesSpecificity(inTrain,
                                                 label,
                                                 inTrain ? realLabel : -1,
                                                 VP, FP, FN, VN);
                    posTest++;
                }
                timeRecog = timeRecog / (_recogTrainImgs.size() + _recogTestImgs.size());

                //Contabiliza o teste
                recogsNames.push_back(recog->algorithmName());
                processorsNames.push_back("Recog: " + processorName);
                resultTests.push_back(std::make_tuple(VP, FP, FN, VN));

                this->saveTest("recog", this->name(), recog->algorithmName(), processorName, timeTrainig, timeRecog, std::make_tuple(VP, FP, FN, VN));


                std::cout << "    Realizando treinamento de comparacao:" << std::endl;
                timeTrainig = 0;
                timeRecog = 0;
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

                std::cout << "    Realizando teste de comparacao:" << std::endl;

                posTest = 0;
                for (auto testImg: _compareTestImgs) {
                    int64_t time = cv::getTickCount();
                    int label = recog->compare(testImg.l, testImg.r);
                    timeRecog += cv::getTickCount() - time;

                    int realLabel = _compareTestLabels[posTest];

                    if (realLabel == COMPARE_EQ) {
                        if (realLabel == label)
                            VP++;
                        else
                            FN++;
                    } else {
                        if (label == COMPARE_EQ)
                            FP++;
                        else
                            VN++;
                    }

                    /*testSensitivitiesSpecificity(_compareTestLabels[posTest] ? true : false,
                                                 label,
                                                 _compareTestLabels[posTest],
                                                 VP, FP, FN, VN);*/
                    posTest++;
                }
                timeRecog = _compareTestImgs.size() ? timeRecog / _compareTestImgs.size() : 0;


                //Contabiliza o teste
                recogsNames.push_back(recog->algorithmName());
                processorsNames.push_back("Compa: " + processorName);
                resultTests.push_back(std::make_tuple(VP, FP, FN, VN));

                this->saveTest("compare", this->name(), recog->algorithmName(), processorName, timeTrainig, timeRecog, std::make_tuple(VP, FP, FN, VN));
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

void Tester::testSensitivitiesSpecificity(bool inTrain, int predictedLabel, int realLabel/*or -1*/, int &VP, int &FP, int &FN, int &VN)
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
        /*if (predictedLabel == realLabel && realLabel > -1) {
            VP++;
        } else */if (predictedLabel == -1) {
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
    cols.push_back("image type");
    cols.push_back("technique");
    cols.push_back("processors");

    // Tempos
    cols.push_back("cpu frequencia");
    cols.push_back("training time");
    cols.push_back("predict time");

    // Results
    cols.push_back("VP");
    cols.push_back("FP");
    cols.push_back("FN");
    cols.push_back("VN");

    tools::appendCsv("results", cols);
}


void Tester::saveTest(std::string testType,
                      std::string imageType,
                      std::string recogName,
                      std::string processorName,
                      int64_t timeTrainig,
                      int64_t timeRecog,
                      std::tuple<int, int, int, int> resultTest)
{
    int VP, FP, FN, VN;
    std::tie(VP, FP, FN, VN) = resultTest;

    std::vector<std::string > cols;
    cols.push_back(testType);
    cols.push_back(imageType);
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
