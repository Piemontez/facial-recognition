#include "lbp.hpp"

#include "opencv4/opencv2/opencv.hpp"

template <typename _Tp>
void ELBP_(const cv::Mat& src, cv::Mat& dst, int radius, int neighbors);

template <typename _Tp>
void OLBP_(const cv::Mat& src, cv::Mat& dst);

cv::Mat LBP::proccess(const cv::Mat &image, int pos, ImageLoader* imgLoader)
{
    cv::Mat out;

    if (image.type() == CV_32FC1) {
        if (image.channels() == 1)
            image.convertTo(out, CV_8UC1, 255);
        else
            image.convertTo(out, CV_8UC3, 255);
    } else
        out = image.clone();

    cv::Mat planes[out.channels()];
    cv::split(out, planes);

//    OLBP_<unsigned char>(planes[2], out);
//    return out;

    for (int x = 0; x < out.channels(); x++)
    {
        OLBP_<unsigned char>(planes[x], out);
        planes[x] = out;
    }

    cv::merge(planes, out.channels(), out);

    return out;
}

template <typename _Tp>
void OLBP_(const cv::Mat& src, cv::Mat& dst) {
    dst = cv::Mat::zeros(src.rows-2, src.cols-2, CV_8UC1);
    for(int i=1;i<src.rows-1;i++) {
        for(int j=1;j<src.cols-1;j++) {
            _Tp center = src.at<_Tp>(i,j);
            unsigned char code = 0;
            code |= (src.at<_Tp>(i-1,j-1) > center) << 7;
            code |= (src.at<_Tp>(i-1,j) > center) << 6;
            code |= (src.at<_Tp>(i-1,j+1) > center) << 5;
            code |= (src.at<_Tp>(i,j+1) > center) << 4;
            code |= (src.at<_Tp>(i+1,j+1) > center) << 3;
            code |= (src.at<_Tp>(i+1,j) > center) << 2;
            code |= (src.at<_Tp>(i+1,j-1) > center) << 1;
            code |= (src.at<_Tp>(i,j-1) > center) << 0;
            dst.at<unsigned char>(i-1,j-1) = code;
        }
    }
}

template <typename _Tp>
void ELBP_(const cv::Mat& src, cv::Mat& dst, int radius, int neighbors) {
    neighbors = cv::max(cv::min(neighbors,31),1); // set bounds...
    // Note: alternatively you can switch to the new OpenCV Mat_
    // type system to define an unsigned int matrix... I am probably
    // mistaken here, but I didn't see an unsigned int representation
    // in OpenCV's classic typesystem...
    dst = cv::Mat::zeros(src.rows-2*radius, src.cols-2*radius, CV_32SC1);
    for(int n=0; n<neighbors; n++) {
        // sample points
        float x = static_cast<float>(radius) * cos(2.0*M_PI*n/static_cast<float>(neighbors));
        float y = static_cast<float>(radius) * -sin(2.0*M_PI*n/static_cast<float>(neighbors));
        // relative indices
        int fx = static_cast<int>(floor(x));
        int fy = static_cast<int>(floor(y));
        int cx = static_cast<int>(ceil(x));
        int cy = static_cast<int>(ceil(y));
        // fractional part
        float ty = y - fy;
        float tx = x - fx;
        // set interpolation weights
        float w1 = (1 - tx) * (1 - ty);
        float w2 =      tx  * (1 - ty);
        float w3 = (1 - tx) *      ty;
        float w4 =      tx  *      ty;
        // iterate through your data
        for(int i=radius; i < src.rows-radius;i++) {
            for(int j=radius;j < src.cols-radius;j++) {
                float t = w1*src.at<_Tp>(i+fy,j+fx) + w2*src.at<_Tp>(i+fy,j+cx) + w3*src.at<_Tp>(i+cy,j+fx) + w4*src.at<_Tp>(i+cy,j+cx);
                // we are dealing with floating point precision, so add some little tolerance
                dst.at<unsigned int>(i-radius,j-radius) += ((t > src.at<_Tp>(i,j)) && (abs(t-src.at<_Tp>(i,j)) > std::numeric_limits<float>::epsilon())) << n;
            }
        }
    }
}
