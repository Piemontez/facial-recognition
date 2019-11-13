#ifndef THREEGAUSSIANBLUR_HPP
#define THREEGAUSSIANBLUR_HPP

#include "../imageprocessor.hpp"

class GaussianBlur : public ImageProcessor
{
    virtual cv::Mat proccess(const cv::Mat &image, int pos = 0, ImageLoader* imgLoader = nullptr) override;
    virtual std::string name() override { return "GaussianBlur"; };
};

#endif // THREEGAUSSIANBLUR_HPP
