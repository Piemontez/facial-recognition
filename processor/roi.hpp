#ifndef ROI_HPP
#define ROI_HPP

#include "../imageprocessor.hpp"
#include "../libs/facedetection.hpp"

#include <opencv2/face.hpp>

/**
 * @brief The ROI class
 */
class ROI: public ImageProcessor
{
    FaceDetection *faceD;
    cv::Ptr<cv::face::Facemark> facemark;

public:
    ROI();

    virtual cv::Mat proccess(const cv::Mat &image, int pos = 0, ImageLoader* imgLoader = nullptr) override;
    virtual std::string name() override { return "ROI"; };
};

#endif // ROI_HPP
