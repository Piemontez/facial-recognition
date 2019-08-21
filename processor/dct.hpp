#ifndef DCT_HPP
#define DCT_HPP

#include "../imageprocessor.hpp"

namespace cv {
    class Mat;
}
class DCT : public ImageProcessor
{
    virtual cv::Mat proccess(const cv::Mat &image) override;
    virtual std::string name() override { return "DCT"; };
};

#endif // DCT_HPP
