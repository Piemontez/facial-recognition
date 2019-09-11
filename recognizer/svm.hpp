#ifndef SVMOpenCV_HPP
#define SVMOpenCV_HPP

#include "../facerecognizer.hpp"
#include "opencv4/opencv2/ml.hpp"

class SVMOpenCV: public Recognizer {
    cv::Ptr<cv::ml::SVM> model;

public:
    SVMOpenCV();

    std::string algorithmName() override;

    void train(const std::vector<cv::Mat> &images, const std::vector<int> &labels) override;
    void resetTrain() override;

    void predict(const cv::Mat &image, int &label, int &confidence) override;
    int predict(const cv::Mat &image) override;
    int compare(const cv::Mat &imageA, const cv::Mat &imageB) override;

    void save(const std::string &filePath) override;
    void load(const std::string &filePath) override;

};

#endif // SVMOpenCV_HPP
