#ifndef LBP_HPP
#define LBP_HPP

#include "../imageprocessor.hpp"

namespace cv {
    class Mat;
}
class LBP : public ImageProcessor
{
    virtual cv::Mat proccess(const cv::Mat &image, int pos = 0, ImageLoader* imgLoader = nullptr) override;
    virtual std::string name() override { return "LBP"; };
};

#endif // LBP_HPP
