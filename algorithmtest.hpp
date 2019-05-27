#ifndef ALGORITHMTEST_HPP
#define ALGORITHMTEST_HPP

#include <vector>

namespace cv {
    class Mat;
}

class FaceRecognizer;

class AlgorithmTest
{
public:
    AlgorithmTest() {}

    int totalImages();
    int successTests();
    int failTests();

    FaceRecognizer *recognizer();
    void setRecognizer(FaceRecognizer *recognizer);
    void setTrainImages(std::vector<cv::Mat> images, std::vector<int> labels);
    void setImagesToTest(std::vector<cv::Mat> images, std::vector<int> labels);
    void test();

};

#endif // ALGORITHMTEST_HPP
