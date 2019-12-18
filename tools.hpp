#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <opencv4/opencv2/core/core.hpp>

namespace tools {
//    const float SCALE_XY = (1.0/75.0);
    const float SCALE_XY = (1.0/20.0);
    const float SCALE_Z = 5.0;

    void moveToRigth(cv::Mat &inout);
    void moveToCenter(cv::Mat &inout, const cv::Vec<bool, 2> &scaled, const cv::Vec2f &tvec);

    void depthImgToPointCloud(const cv::Mat &depth, const cv::Mat &color, cv::Mat &pc, const cv::Vec<bool,3> &scale = {true, true, false}, float greatthen = .0, const cv::Vec3f &tvec = {.0,.0,.0});
    inline void depthImgToPointCloud(const cv::Mat &depth, cv::Mat &pc, const cv::Vec<bool,3> &scale = {true, true, false}, float greatthen = .0, const cv::Vec3f &tvec = {.0,.0,.0}) {
        depthImgToPointCloud(depth, cv::Mat(), pc, scale, greatthen, tvec);
    }

    void pointCloudToDepthImg(const cv::Mat &pc, cv::Mat &depth, const cv::Vec<bool,3> &scale = {true, true, false}, const cv::Vec3f &tvec = {.0,.0,.0});
    void pointCloudToRGBImg(const cv::Mat &pc, cv::Mat &rgb, const cv::Vec<bool,2> &scale = {true, true, false}, const cv::Vec3f &tvec = {.0,.0,.0});

    cv::Mat eulerAnglesToRotationMatrix(const cv::Vec3d &theta);
    cv::Matx44d rotationMatrixTo44d(cv::Mat r);

    void saveImgProc(cv::Mat img, std::string permutation, int imgId, int permPos, bool blackToWhite = false);
    void appendCsv(std::string filename, std::vector<std::string > cols);

    cv::Mat transformPCPose(cv::Mat pc, const cv::Matx44d& Pose);
};

#endif // TOOLS_HPP
