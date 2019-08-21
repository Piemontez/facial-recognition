#include "dct.hpp"

#include "opencv4/opencv2/opencv.hpp"

cv::Mat DCT::proccess(const cv::Mat &image)
{
    //cv::Mat imagePad(image.rows/2*2, image.cols/2*2, image.type());
    cv::Mat padded;
    cv::Mat out;

    int nrows = cv::getOptimalDFTSize(image.rows);
    int ncols = cv::getOptimalDFTSize(image.cols);

    cv::copyMakeBorder(image, padded,
                       0, nrows-image.rows,
                       0, ncols-image.cols,
                       cv::BORDER_CONSTANT, cv::Scalar::all(0));

    cv::Mat planes[padded.channels()];
    cv::split(padded, planes);

    for (int x = 0; x < padded.channels(); x++)
    {
        planes[x].convertTo(planes[x], CV_32FC1);
        cv::dct(planes[x], out);
        planes[x] = out;
    }

    cv::merge(planes, image.channels(), out);

    return out;
}
