#ifndef THREEGAUSSIANBLUR_HPP
#define THREEGAUSSIANBLUR_HPP

#include "../imageprocessor.hpp"

class GaussianBlur : public ImageProcessor
{
    virtual cv::Mat proccess(cv::Mat &image) override;
    virtual std::string name() override { return "GaussianBlur"; };
};

#endif // THREEGAUSSIANBLUR_HPP
