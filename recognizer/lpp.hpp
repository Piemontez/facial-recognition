//https://github.com/jakevdp/lpproj
#ifndef LPP_HPP
#define LPP_HPP

#include "../facerecognizer.hpp"

class LPP: public Recognizer {

public:
    LPP();

    std::string algorithmName() override;

    void train(const std::vector<cv::Mat> &images, const std::vector<int> &labels) override;
    void resetTrain() override;

    void predict(const cv::Mat &image, int &label, int &confidence) override;
    int predict(const cv::Mat &image) override;
    int compare(const cv::Mat &imageA, const cv::Mat &imageB) override;

    void save(const std::string &filePath) override;
    void load(const std::string &filePath) override;

};

#endif // LPP_HPP

