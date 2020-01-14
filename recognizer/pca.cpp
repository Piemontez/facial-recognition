#include "pca.hpp"

#include "opencv4/opencv2/opencv.hpp"

PCA::PCA()
{
    model = cv::face::EigenFaceRecognizer::create();
}

std::string PCA::algorithmName()
{
    return "PCA";
}

void PCA::train(const std::vector<cv::Mat> &train, const std::vector<int> &trainLabels)
{
    model->train(train, trainLabels);
}

void PCA::resetTrain()
{
    model->clear();
}

void PCA::predict(const cv::Mat &image, int &label, int &confidence)
{
    throw "PCA::predict";
    //model->predict(image, label, confidence);
}

int PCA::predict(const cv::Mat &image)
{
    return model->predict(image);
}

int PCA::compare(const cv::Mat &source, const cv::Mat &targe)
{
    cv::Mat out;
    cv::hconcat(source, targe, out);

    return model->predict(out);
}

void PCA::save(const std::string &filePath)
{

}

void PCA::load(const std::string &filePath)
{

}
