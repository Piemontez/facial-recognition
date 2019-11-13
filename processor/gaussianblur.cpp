#include "gaussianblur.hpp"

#include "opencv4/opencv2/opencv.hpp"

cv::Mat GaussianBlur::proccess(const cv::Mat &image, int pos, ImageLoader* imgLoader)
{
    cv::Mat out;
    cv::GaussianBlur(image, out, cv::Size(3,3), 0);
    return out;
}
