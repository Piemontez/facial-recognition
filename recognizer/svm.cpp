#include "svm.hpp"

#include "opencv4/opencv2/opencv.hpp"

SVMOpenCV::SVMOpenCV()
{
    model = cv::ml::SVM::create();
    model->setType(cv::ml::SVM::C_SVC);
    model->setKernel(cv::ml::SVM::LINEAR);
    model->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 500, 1e-6));
}

std::string SVMOpenCV::algorithmName()
{
    return "SVMOpenCV";
}

void SVMOpenCV::train(const std::vector<cv::Mat> &train, const std::vector<int> &trainLabels)
{
    if (!train.size())
        return;

    int size = train.front().cols * train.front().rows;

    cv::Mat trainingDataMat(cv::Size(size, train.size()), CV_32F, cv::Scalar(0));

    int pos = 0;
    for(auto && data: train) {
        cv::Mat output;
        data.convertTo(output, CV_32F);
        trainingDataMat.row(pos++) = cv::Mat(1, size, output.type(), output.data);
    }

    model->train(trainingDataMat,
                 cv::ml::ROW_SAMPLE,
                 trainLabels);
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
    cv::Mat output;
    image.convertTo(output, CV_32F);
    cv::Mat redims(1, image.rows*image.cols, output.type(), output.data);

    return model->predict(redims);
}

int SVMOpenCV::compare(const cv::Mat &source, const cv::Mat &targe)
{
    cv::Mat out;
    cv::hconcat(source, targe, out);

    return model->predict(out);
}

void SVMOpenCV::save(const std::string &filePath)
{

}

void SVMOpenCV::load(const std::string &filePath)
{

}
