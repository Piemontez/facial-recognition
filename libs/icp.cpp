#include "icp.hpp"

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/surface_matching.hpp>
#include <opencv4/opencv2/surface_matching/ppf_helpers.hpp>

//https://docs.opencv.org/4.1.2/d9/d25/group__surface__matching.html

ICP::ICP(const cv::Mat &frontalFace)
{
    this->frontalFace = frontalFace.clone();

    icp = new cv::ppf_match_3d::ICP(2000, 0.001f, 2.5f, 8);
    //icp = new cv::ppf_match_3d::ICP(1000);
    //int64 t1 = cv::getTickCount();
}

cv::Mat ICP::proccess(const cv::Mat &imageCache)
{
    assert(this->frontalFace.type() == CV_32F);
    assert(imageCache.type() == CV_32F);

    cv::Rect a(0, 0, imageCache.cols, imageCache.rows/5*4);
    cv::Mat frontalFace = cv::Mat(this->frontalFace.rows * this->frontalFace.cols, 1, CV_32FC3, cv::Scalar(0,0,0));
    cv::Mat image = cv::Mat(imageCache.rows * imageCache.cols, 1, CV_32FC3, cv::Scalar(0,0,0));

    /*
    // Now train the model
    //int64 tick1 = cv::getTickCount();
    cv::ppf_match_3d::PPF3DDetector detector(0.025, 0.05);
    detector.trainModel(frontalFace.clone());
    //int64 tick2 = cv::getTickCount();

    // Match the model to the scene and get the pose
    std::vector<cv::ppf_match_3d::Pose3DPtr>  poses;
    //tick1 = cv::getTickCount();
    detector.match(image, poses, 1.0 / 40.0, 0.05);
    //tick2 = cv::getTickCount();

    // Register for all selected poses
    icp->registerModelToScene(image, frontalFace, poses);

    cv::Mat newpose;
    for (size_t i = 0; i < poses.size(); i++)
    {
        auto result = poses[i];
        result->printPose();
        if (i == 0)
        {
            newpose = cv::ppf_match_3d::transformPCPose(image, result->pose);
        }
    }*/

    //    std::cout << poses.size() << std::endl;
    //    if (poses.size())
    //        poses[0]->printPose();


    cv::Matx44d poses;
    double residual = 0;
    icp->registerModelToScene(image, frontalFace.clone(), residual, poses);

    cv::Mat newpose = cv::ppf_match_3d::transformPCPose(image, poses);

    cv::imshow("frontal", imageCache);
//    cv::imshow("frontal", frontalFace);
//    cv::imshow("processed", image);
//    cv::imshow("new pose", newpose);
    cv::waitKey(100);

    return image;
}
