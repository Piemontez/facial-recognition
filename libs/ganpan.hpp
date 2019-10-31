#ifndef GANPAN_HPP
#define GANPAN_HPP

#include "../imageprocessor.hpp"

#include <opencv2/face.hpp>

/**
 * @brief The Iterative closest point
 */
class GanPan : public ImageProcessor
{
    cv::Ptr<cv::face::Facemark> facemark;
public:
    GanPan();

    virtual cv::Mat proccess(const cv::Mat &image) override;
    virtual std::string name() override { return "GanPan"; };
};

#endif // GANPAN_HPP
