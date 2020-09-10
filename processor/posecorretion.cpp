#include "posecorretion.hpp"

#include "opencv4/opencv2/opencv.hpp"

PoseCorrection::PoseCorrection(ImageLoader *loader)
{
    afm = new AFM(loader);
    cv::Mat afmImg =  afm->proccess(cv::Mat());

    icp = new ICP(afmImg);

    //icp = new ICP(cv::Mat());
    landmarkposecorretion = new LandMarkPoseCorrection();
}

cv::Mat PoseCorrection::proccess(const cv::Mat &image, int pos, ImageLoader* imgLoader)
{
    return icp->proccess(
        landmarkposecorretion->proccess(image, pos, imgLoader)
    , pos, imgLoader);
}
