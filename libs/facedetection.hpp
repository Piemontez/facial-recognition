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
    int flags{/*cv::CASCADE_DO_CANNY_PRUNING |*/ cv::CASCADE_SCALE_IMAGE | cv::CASCADE_FIND_BIGGEST_OBJECT /*| cv::CASCADE_DO_ROUGH_SEARCH*/};
    cv::CascadeClassifier face_cascade;
    cv::CascadeClassifier face_cascade_tree;
    cv::CascadeClassifier profileface_cascade;

public:
    FaceDetection();

    virtual cv::Mat proccess(const cv::Mat &image, int pos = 0, ImageLoader* imgLoader = nullptr) override;
    std::vector<cv::Rect> getfaces(const cv::Mat &image);
    virtual std::string name() override { return "FaceDetection"; };
};

#endif // FaceDetection_HPP
