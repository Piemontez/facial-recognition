#ifndef PCA_HPP
#define PCA_HPP

#include "../facerecognizer.hpp"
#ifdef HASCONTRIB
#include "opencv4/opencv2/contrib/contrib.hpp"
#endif

class PCA: public Recognizer {
#ifdef HASCONTRIB
    cv::Ptr<FaceRecognizer> model;
#endif
public:
    std::string algorithmName() override;

    void train(const std::vector<cv::Mat> &images, const std::vector<int> &labels) override;
    void resetTrain() override;

    void predict(const cv::Mat &image, int &label, int &confidence) override;
    int predict(const cv::Mat &image) override;
    int compare(const cv::Mat &imageA, const cv::Mat &imageB) override;

    void save(const std::string &filePath) override;
    void load(const std::string &filePath) override;

};

#endif // PCA_HPP
