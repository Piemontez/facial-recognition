#ifndef POSECORRECTION_HPP
#define POSECORRECTION_HPP

#include "../imageprocessor.hpp"

class PoseCorrection : public ImageProcessor
{
    virtual cv::Mat proccess(const cv::Mat &image) override;
    virtual std::string name() override { return "PoseCorrection"; };
};

#endif // POSECORRECTION_HPP
