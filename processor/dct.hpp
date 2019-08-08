#ifndef DCT_HPP
#define DCT_HPP

#include "../imageprocessor.hpp"

namespace cv {
    class Mat;
}
class DCT : public ImageProcessor
{
    virtual cv::Mat proccess(cv::Mat &image) override;
};

#endif // DCT_HPP
