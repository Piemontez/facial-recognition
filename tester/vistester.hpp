#ifndef VISTESTER_HPP
#define VISTESTER_HPP

#include "../tester.hpp"

class VisTester : public Tester
{
public:
    VisTester();

    std::string name() override;

    void resetTrain() override;
    void train(std::vector<cv::Mat> train, std::vector<int> trainLabels) override;
    int test(cv::Mat test) override;
    bool test(cv::Mat source, cv::Mat targe) override;
};

#endif // VISTESTER_HPP
