#include "vistester.hpp"

#include "../loader/visloader.hpp"
#include "../processor/gaussianblur.hpp"
#include "../processor/lbp.hpp"
#include "../processor/dct.hpp"

#include "opencv4/opencv2/opencv.hpp"

VisTester::VisTester() {
    setImageLoader(new VisLoader);

    addPreProcessor(new GaussianBlur);
    //addPreProcessor(new LBP);
    //addPreProcessor(new DCT);
}

std::string VisTester::name()
{
    return "VIS Tester";
}

void VisTester::resetTrain()
{

}

void VisTester::train(std::vector<cv::Mat> train, std::vector<int> trainLabels)
{

}

int VisTester::test(cv::Mat test)
{
    return -1;
}

bool VisTester::test(cv::Mat source, cv::Mat targe)
{
    return false;
}
