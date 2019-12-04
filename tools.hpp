#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <opencv4/opencv2/core/core.hpp>

namespace tools {
//    const float SCALE_XY = (1.0/75.0);
    const float SCALE_XY = (1.0/20.0);
    const float SCALE_Z = 5.0;

    void moveToRigth(cv::Mat &inout);
    void depthImgToPointCloud(const cv::Mat &depth, cv::Mat &pc, const cv::Vec<bool,3> &scale = {true, true, false}, float greatthen = .0, const cv::Vec3f &tvec = {.0,.0,.0});
    void pointCloudToDepthImg(const cv::Mat &pc, cv::Mat &depth, const cv::Vec<bool,3> &scale = {true, true, false}, const cv::Vec3f &tvec = {.0,.0,.0});

    cv::Mat eulerAnglesToRotationMatrix(const cv::Vec3d &theta);
    cv::Matx44d rotationMatrixTo44d(cv::Mat r);
};

#endif // TOOLS_HPP
