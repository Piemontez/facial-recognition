#ifndef ICP_HPP
#define ICP_HPP

#include <opencv4/opencv2/core/mat.hpp>
#include "../imageprocessor.hpp"

namespace cv {
    namespace ppf_match_3d {
        class ICP;
        class PPF3DDetector;
    }
}

/**
 * @brief The Iterative closest point
 */
class ICP : public ImageProcessor
{
    cv::Mat frontalFace;
    cv::Mat frontalFaceCloud_0_8;
    cv::Mat frontalFaceCloud_0_9;
    cv::Mat frontalFaceCloud_0_95;
    cv::Mat frontalFaceCloud_0_975;
    cv::Mat frontalFaceCloud;
    cv::Mat frontalFaceCloud_1_025;
    cv::Mat frontalFaceCloud_1_05;
    cv::Mat frontalFaceCloud_1_1;
    cv::Mat frontalFaceCloud_1_2;

    cv::ppf_match_3d::ICP* icp;
    cv::ppf_match_3d::PPF3DDetector* detector;

public:
    ICP(const cv::Mat &frontalFace);

    virtual cv::Mat proccess(const cv::Mat &image, int pos = 0, ImageLoader* imgLoader = nullptr) override;
    virtual std::string name() override { return "ICP"; };
};

#endif // ICP_HPP
