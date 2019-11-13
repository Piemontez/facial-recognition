#ifndef IMAGEPROCESSOR_HPP
#define IMAGEPROCESSOR_HPP

#include "string"

namespace cv {
    class Mat;
}
class ImageLoader;

class ImageProcessor
{
public:
    virtual cv::Mat proccess(const cv::Mat &image, int pos = 0, ImageLoader* imgLoader = nullptr) = 0;
    virtual std::string name() { return "No ImageProcessor Name"; };
};

#endif // IMAGEPROCESSOR_HPP
