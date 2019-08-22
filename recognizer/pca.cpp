#include "pca.hpp"

#include "../loader/3dloader.hpp"
#include "../processor/gaussianblur.hpp"
#include "../processor/lbp.hpp"
#include "../processor/dct.hpp"

#include "opencv4/opencv2/opencv.hpp"

std::string PCA::algorithmName()
{
    return "PCA";
}

void PCA::train(const std::vector<cv::Mat> &train, const std::vector<int> &trainLabels)
{
#ifdef HASCONTRIB
    model->train(train, trainLabels);
#endif
}

void PCA::resetTrain()
{

}

void PCA::predict(const cv::Mat &image, int &label, int &confidence)
{
#ifdef HASCONTRIB
    model->predict(image, label, confidence);
#endif
}

int PCA::predict(const cv::Mat &image)
{
#ifdef HASCONTRIB
    return model->predict(test);
#endif
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
