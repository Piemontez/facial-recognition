#ifndef FaceDetection_HPP
#define FaceDetection_HPP

#include <opencv4/opencv2/core/mat.hpp>
#include "../imageprocessor.hpp"

/**
 * @brief The Iterative closest point
 */
class FaceDetection : public ImageProcessor
{
    cv::Mat frontalFace;

public:
    FaceDetection();

    virtual cv::Mat proccess(const cv::Mat &image) override;
    virtual std::string name() override { return "FaceDetection"; };
};

#endif // FaceDetection_HPP
