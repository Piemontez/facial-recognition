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
    model->clear();
}

void LDA::predict(const cv::Mat &image, int &label, int &confidence)
{
    throw "LDA::predict";
    //model->predict(image, label, confidence);
}

int LDA::predict(const cv::Mat &image)
{
    int label; double confidence;
    model->predict(image, label, confidence);
    if (confidence > predictMin)
        return label;
    else
        return -1;
}

int LDA::compare(const cv::Mat &source, const cv::Mat &targe)
{
    cv::Mat out;
    cv::hconcat(source, targe, out);

    return model->predict(out);
}

void LDA::save(const std::string &filePath)
{

}

void LDA::load(const std::string &filePath)
{

}
