#include "gaussianblur.hpp"

#include "opencv4/opencv2/opencv.hpp"

cv::Mat GaussianBlur::proccess(cv::Mat &image)
{
    cv::Mat out;
    cv::GaussianBlur(image, out, cv::Size(3,3), 0);
    return out;
}
