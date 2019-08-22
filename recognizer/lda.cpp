#include "lda.hpp"

#include "opencv4/opencv2/opencv.hpp"

LDA::LDA()
{
    model = cv::face::FisherFaceRecognizer::create();
}

std::string LDA::algorithmName()
{
    return "LDA";
}

void LDA::train(const std::vector<cv::Mat> &train, const std::vector<int> &trainLabels)
{
    model->train(train, trainLabels);
}

void LDA::resetTrain()
{

}

void LDA::predict(const cv::Mat &image, int &label, int &confidence)
{
    throw "LDA::predict";
    //model->predict(image, label, confidence);
}

int LDA::predict(const cv::Mat &image)
{
    return model->predict(image);
}

int LDA::compare(const cv::Mat &source, const cv::Mat &targe)
{
    return false;
}

void LDA::save(const std::string &filePath)
{

}

void LDA::load(const std::string &filePath)
{

}
