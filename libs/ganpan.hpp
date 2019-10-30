#ifndef GANPAN_HPP
#define GANPAN_HPP

#include "../imageprocessor.hpp"

namespace cv {
    class Mat;
}

/**
 * @brief The Iterative closest point
 */
class GanPan : public ImageProcessor
{
public:
    virtual cv::Mat proccess(const cv::Mat &image) override;
    virtual std::string name() override { return "GanPan"; };
};

#endif // GANPAN_HPP
