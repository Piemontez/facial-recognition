#include "svm.hpp"

#include "opencv4/opencv2/opencv.hpp"

SVMOpenCV::SVMOpenCV()
{
    model = cv::ml::SVM::create();
    model->setType(cv::ml::SVM::C_SVC);
    model->setKernel(cv::ml::SVM::LINEAR);
    model->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6));
}

std::string SVMOpenCV::algorithmName()
{
    return "SVMOpenCV";
}

void SVMOpenCV::train(const std::vector<cv::Mat> &train, const std::vector<int> &trainLabels)
{
    model->train(train, cv::ml::ROW_SAMPLE, trainLabels);
}

void SVMOpenCV::resetTrain()
{

}

void SVMOpenCV::predict(const cv::Mat &image, int &label, int &confidence)
{
    throw "SVMOpenCV::predict";
    //model->predict(image, label, confidence);
}

int SVMOpenCV::predict(const cv::Mat &image)
{
    return model->predict(image);
}

int SVMOpenCV::compare(const cv::Mat &source, const cv::Mat &targe)
{
    return false;
}

void SVMOpenCV::save(const std::string &filePath)
{

}

void SVMOpenCV::load(const std::string &filePath)
{

}
