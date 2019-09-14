#include "lpp.hpp"

#include "opencv4/opencv2/opencv.hpp"

LPP::LPP()
{

}

std::string LPP::algorithmName()
{
    return "LPP";
}

void LPP::train(const std::vector<cv::Mat> &train, const std::vector<int> &trainLabels)
{

}

void LPP::resetTrain()
{

}

void LPP::predict(const cv::Mat &image, int &label, int &confidence)
{
    throw "LPP::predict";
    //model->predict(image, label, confidence);
}

int LPP::predict(const cv::Mat &image)
{
    return false;
}

int LPP::compare(const cv::Mat &source, const cv::Mat &targe)
{
    return false;
}

void LPP::save(const std::string &filePath)
{

}

void LPP::load(const std::string &filePath)
{

}
