#ifndef FaceDetection_HPP
#define FaceDetection_HPP

#include <opencv4/opencv2/face.hpp>
#include <opencv4/opencv2/core/mat.hpp>
#include "../imageprocessor.hpp"

/**
 * @brief The Iterative closest point
 */
class FaceDetection : public ImageProcessor
{
    cv::Mat frontalFace;
    cv::CascadeClassifier face_cascade;

public:
    FaceDetection();

    virtual cv::Mat proccess(const cv::Mat &image) override;
    std::vector<cv::Rect> getfaces(const cv::Mat &image);
    virtual std::string name() override { return "FaceDetection"; };
};

#endif // FaceDetection_HPP
