#ifndef IMAGEPROCESSOR_HPP
#define IMAGEPROCESSOR_HPP

#include "string";

namespace cv {
    class Mat;
}

class ImageProcessor
{
public:
    virtual cv::Mat proccess(cv::Mat &image) = 0;
    virtual std::string name() { return "No ImageProcessor Name"; };
};

#endif // IMAGEPROCESSOR_HPP
