#include "irtester.hpp"

#include "../loader/irloader.hpp"
#include "../processor/gaussianblur.hpp"
#include "../processor/lbp.hpp"
#include "../processor/dct.hpp"

#include "opencv4/opencv2/opencv.hpp"

IRTester::IRTester() {
    setImageLoader(new IrLoader);

    addPreProcessor(new GaussianBlur);
    //addPreProcessor(new LBP);
    //addPreProcessor(new DCT);
}

std::string IRTester::name()
{
    return "IR Tester";
}

void IRTester::resetTrain()
{

}

void IRTester::train(std::vector<cv::Mat> train, std::vector<int> trainLabels)
{

}

int IRTester::test(cv::Mat test)
{
    return -1;
}

bool IRTester::test(cv::Mat source, cv::Mat targe)
{
    return false;
}
