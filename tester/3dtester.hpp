#ifndef THREEDTESTER_HPP
#define THREEDTESTER_HPP

#include "../tester.hpp"

class ThreeDTester : public Tester
{
public:
    ThreeDTester();

    std::string name() override;

    void resetTrain() override;
    void train(std::vector<cv::Mat> train, std::vector<int> trainLabels) override;
    int test(cv::Mat test) override;
    bool test(cv::Mat source, cv::Mat targe) override;
};

#endif // THREEDTESTER_HPP
