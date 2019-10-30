#ifndef AFM_HPP
#define AFM_HPP

#include "../imageprocessor.hpp"

namespace cv {
    class Mat;
}

/**
 * @brief The Iterative closest point
 */
class AFM : public ImageProcessor
{
    virtual cv::Mat proccess(const cv::Mat &image) override;
    virtual std::string name() override { return "AFM"; };
};

#endif // AFM_HPP
