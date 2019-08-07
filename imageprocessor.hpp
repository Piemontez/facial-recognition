#ifndef IMAGEPROCESSOR_HPP
#define IMAGEPROCESSOR_HPP

namespace cv {
    class Mat;
}

class ImageProcessor
{
public:
    virtual cv::Mat proccess(cv::Mat &image) = 0;
};

#endif // IMAGEPROCESSOR_HPP
