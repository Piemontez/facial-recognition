#include "pca.hpp"

#include "../loader/3dloader.hpp"
#include "../processor/gaussianblur.hpp"
#include "../processor/lbp.hpp"
#include "../processor/dct.hpp"

#include "opencv4/opencv2/opencv.hpp"
//#include "opencv4/opencv2/face/facerec.hpp"

PCA::PCA()
{
    model = cv::face::EigenFaceRecognizer::create();
    //model = cv::face::createFacemarkAAM();
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
    return false;
}

void PCA::save(const std::string &filePath)
{

}

void PCA::load(const std::string &filePath)
{

}
