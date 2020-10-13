#ifndef TESTER_HPP
#define TESTER_HPP

#include <vector>
#include <string>

namespace cv {
    class Mat;
}

class AlgorithmTest;
class ImageProcessor;
class ImageLoader;

struct TesterPrivate;
class Tester {
    TesterPrivate *d_ptr;

public:
    Tester();

    virtual std::string name();

    //virtual void resetTrain() = 0;
    //virtual void train(std::vector<cv::Mat> train, std::vector<int> trainLabels) = 0;
    //virtual int test(cv::Mat test) = 0;
    //virtual bool test(cv::Mat source, cv::Mat targe) = 0;

    void disableCache();

    std::vector<cv::Mat> images();
    std::vector<int> labels();

    std::vector<AlgorithmTest*> algorithms();
    void addAlgorithm(AlgorithmTest *test);

    std::vector<ImageProcessor *> preProcessors();
    void addPreProcessorOrderFixed(ImageProcessor *processor);
    void addPreProcessor(ImageProcessor *processor);

    ImageLoader *imageLoader();
    void setImageLoader(ImageLoader *processor);

    void selectAndLoadImages();
    void run();

    void addCsvHeader();
    void saveTest(std::string testType,
                  std::string imageType,
                  std::string recogName,
                  std::string processorName,
                  int64_t timeTrainig,
                  int64_t timeRecog,
                  std::tuple<int, int, int, int> resultTest);

    void showResults(std::vector<std::string> recogsNames,
                     std::vector<std::string> processorsNames,
                     std::vector<std::tuple<int, int, int, int> > resultTests);
private:
    std::vector<std::vector<ImageProcessor *>> permutations(
            const std::vector<ImageProcessor *> &processorsOrderFixed,
            const std::vector<ImageProcessor *> &processors);

    std::vector<std::vector<int>> leaveOneOutGroups();
    void testSensitivitiesSpecificity(bool inTrain, int predictedLabel, int realLabel, int &VP, int &FP, int &FN, int &VN);
};

#endif // TESTER_HPP
