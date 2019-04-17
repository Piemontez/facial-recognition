#ifndef TESTER_HPP
#define TESTER_HPP

#include <vector>

namespace cv {
    class Mat;
}

class AlgorithmTest;
class ImageProcessor;
class ImageLoader;

class Tester {
public:

    std::vector<cv::Mat> images();
    std::vector<int> labels();

    AlgorithmTest *algorithm();
    void setAlgorithm(AlgorithmTest *test);

    std::vector<ImageProcessor *> preProcessor();
    void addPreProcessor(ImageProcessor *processor);

    ImageLoader * imageLoader();
    void setImageLoader(ImageLoader *processor);

    void selectAndLoadImages();
    void saveTest();
    void showResults();
    void run();
};

#endif // TESTER_HPP
