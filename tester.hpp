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
    std::vector<int> labels();
    std::vector<cv::Mat> imagesTrain();
    std::vector<cv::Mat> imagesTest();

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
};

#endif // TESTER_HPP
