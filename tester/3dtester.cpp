#include "3dtester.hpp"

#include "../loader/3dloader.hpp"
#include "../processor/gaussianblur.hpp"
#include "../processor/lbp.hpp"
#include "../processor/dct.hpp"

#include "opencv4/opencv2/opencv.hpp"

ThreeDTester::ThreeDTester() {
    setImageLoader(new ThreeLoader);

    addPreProcessor(new GaussianBlur);
    //addPreProcessor(new LBP);
    //addPreProcessor(new DCT);
}

std::string ThreeDTester::name()
{
    return "3D Tester";
}


void ThreeDTester::resetTrain()
{

}

void ThreeDTester::train(std::vector<cv::Mat> train, std::vector<int> trainLabels)
{

}

int ThreeDTester::test(cv::Mat test)
{
    return -1;
}

bool ThreeDTester::test(cv::Mat source, cv::Mat targe)
{
    return false;
}
