#ifndef LBP_HPP
#define LBP_HPP

#include "../imageprocessor.hpp"

namespace cv {
    class Mat;
}
class LBP : public ImageProcessor
{
    virtual cv::Mat proccess(cv::Mat &image) override;
};

#endif // LBP_HPP
