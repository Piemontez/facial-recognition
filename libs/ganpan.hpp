#ifndef GANPAN_HPP
#define GANPAN_HPP

#include "../imageprocessor.hpp"
#include "../libs/facedetection.hpp"

#include <opencv2/face.hpp>

/**
 * @brief The Iterative closest point
 */
class GanPan : public ImageProcessor
{
    FaceDetection *faceD;
    cv::Ptr<cv::face::Facemark> facemark;
public:
    GanPan();

    virtual cv::Mat proccess(const cv::Mat &image, int pos = 0, ImageLoader* imgLoader = nullptr) override;
    virtual void estimatePoseDirection(const cv::Mat &image, const std::vector<std::vector<cv::Point2f> > &shapes, cv::Mat &rvec, cv::Mat &tvec);
    virtual void rotateImage(const cv::Mat &image, cv::Mat &output, cv::Mat &rvec, cv::Mat &tvec);
    virtual std::string name() override { return "GanPan"; };
};

#endif // GANPAN_HPP
