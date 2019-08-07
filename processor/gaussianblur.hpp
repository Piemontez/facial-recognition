#ifndef THREEGAUSSIANBLUR_HPP
#define THREEGAUSSIANBLUR_HPP

#include "../imageprocessor.hpp"

class GaussianBlur : public ImageProcessor
{
    virtual cv::Mat proccess(cv::Mat &image) override;
};

#endif // THREEGAUSSIANBLUR_HPP
