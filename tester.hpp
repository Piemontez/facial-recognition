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

    std::vector<cv::Mat> images();
    std::vector<int> labels();

    std::vector<AlgorithmTest*> algorithms();
    void addAlgorithm(AlgorithmTest *test);

    std::vector<ImageProcessor *> preProcessor();
    void addPreProcessor(ImageProcessor *processor);

    ImageLoader *imageLoader();
    void setImageLoader(ImageLoader *processor);

    void selectAndLoadImages();
    void saveTest();
    void showResults();
    void run();

private:
    std::vector<std::vector<ImageProcessor *>> permutations(const std::vector<ImageProcessor *> &processors);

    std::vector<std::vector<int>> leaveOneOutGroups(int imageSize);
    void testSensitivitiesSpecificity(bool inTrain, int predictedLabel, int realLabel, int &VP, int &FP, int &FN, int &VN);
};

#endif // TESTER_HPP
