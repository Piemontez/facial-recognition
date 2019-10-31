#include "icp.hpp"

#include <opencv4/opencv2/opencv.hpp>

ICP::ICP(const cv::Mat &frontalFace)
{
    this->frontalFace = frontalFace;
}

cv::Mat ICP::proccess(const cv::Mat &image)
{
    return image;
}
