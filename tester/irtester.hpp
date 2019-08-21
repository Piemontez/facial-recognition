#ifndef IRTESTER_HPP
#define IRTESTER_HPP

#include "../tester.hpp"

class IRTester : public Tester
{
public:
    IRTester();

    std::string name() override;

    void resetTrain() override;
    void train(std::vector<cv::Mat> train, std::vector<int> trainLabels) override;
    int test(cv::Mat test) override;
    bool test(cv::Mat source, cv::Mat targe) override;
};

#endif // IRTESTER_HPP
