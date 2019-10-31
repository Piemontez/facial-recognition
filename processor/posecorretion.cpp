#include "posecorretion.hpp"

#include "opencv4/opencv2/opencv.hpp"

PoseCorrection::PoseCorrection(ImageLoader *loader)
{
    afm = new AFM(loader);
    cv::Mat afmImg =  afm->proccess(cv::Mat());

    icp = new ICP(afmImg);
    ganpan = new GanPan();
}

cv::Mat PoseCorrection::proccess(const cv::Mat &image)
{
    icp->proccess(
        ganpan->proccess(image)
    );
    return image;
}
