#include "icp.hpp"

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/surface_matching.hpp>
#include <opencv4/opencv2/surface_matching/ppf_helpers.hpp>

//https://docs.opencv.org/4.1.2/d9/d25/group__surface__matching.html

ICP::ICP(const cv::Mat &frontalFace)
{
    this->frontalFace = frontalFace.clone();

    icp = new cv::ppf_match_3d::ICP(200, 0.001f, 2.5f, 8);
    //icp = new cv::ppf_match_3d::ICP(100);
    //int64 t1 = cv::getTickCount();


    int x, y, pointsFrontal = 0;
    for (int y = 0; y < frontalFace.rows; y++) {
        for (int x = 0; x < frontalFace.cols; x++) {
            if (frontalFace.at<float>(y, x) < 0.0001 || this->frontalFace.at<float>(y, x) > 0.9990)
                continue;
            pointsFrontal ++;
        }
    }

    frontalFaceCloud = cv::Mat(pointsFrontal, 3, CV_32FC1, cv::Scalar(0));

    pointsFrontal = 0;
    for (int y = 0; y < frontalFace.rows; y++) {
        for (int x = 0; x < frontalFace.cols; x++) {
            if (frontalFace.at<float>(y, x) < 0.0001 || frontalFace.at<float>(y, x) > 0.9990)
                continue;

            //i = static_cast< std::vector<uint16_t>::size_type >((y * frontalFace.cols) + x);

            frontalFaceCloud.at<float>(pointsFrontal, 0) = y / 150.0;
            frontalFaceCloud.at<float>(pointsFrontal, 1) = x / 150.0;
            frontalFaceCloud.at<float>(pointsFrontal, 2) = frontalFace.at<float>(y, x);

            pointsFrontal++;
        }
    }

    // Now train the model
    //int64 tick1 = cv::getTickCount();
    cv::ppf_match_3d::PPF3DDetector detector(0.06, 0.08, 20);
    detector.trainModel(frontalFaceCloud);
    //int64 tick2 = cv::getTickCount();

}

cv::Mat ICP::proccess(const cv::Mat &imageCache)
{
    assert(this->frontalFace.type() == CV_32F);
    assert(imageCache.type() == CV_32F);

    int points = 0;
    for (int y = 0; y < imageCache.rows; y++) {
        for (int x = 0; x < imageCache.cols; x++) {
            if (imageCache.at<float>(y, x) < 0.0001 || imageCache.at<float>(y, x) > 0.9990)
                continue;
            points ++;
        }
    }

    cv::Mat image = cv::Mat(points, 3, CV_32FC1, cv::Scalar(0));

    points = 0;
    std::vector<uint16_t>::size_type i;
    for (int y = 0; y < imageCache.rows; y++) {
        for (int x = 0; x < imageCache.cols; x++) {
            if (imageCache.at<float>(y, x) < 0.0001 || imageCache.at<float>(y, x) > 0.9990)
                continue;

            //i = static_cast< std::vector<uint16_t>::size_type >((y * imageCache.cols) + x);

            image.at<float>(points, 0) = y / 150.0;
            image.at<float>(points, 1) = x / 150.0;
            image.at<float>(points, 2) = imageCache.at<float>(y, x);

            points++;
        }
    }

    std::cout << "------------" << std::endl;
    //std::cout << points << std::endl;
    //std::cout << pointsFrontal << std::endl;

    std::cout << "------------" << std::endl;

    // Match the model to the scene and get the pose
    std::vector<cv::ppf_match_3d::Pose3DPtr>  posesDetect;
    cv::Mat newpose;
    //tick1 = cv::getTickCount();
    detector.match(image, posesDetect, 1.0 / 5.0, 0.05);
    //tick2 = cv::getTickCount();


    std::cout << "------------" << std::endl;

    std::cout << posesDetect.size() << std::endl;

    // Register for all selected poses
    icp->registerModelToScene(image, frontalFaceCloud, posesDetect);

    std::cout << posesDetect.size() << std::endl;

    for (size_t i = 0; i < posesDetect.size(); i++)
    {
        auto result = posesDetect[i];
        std::cout << "Pose Result " << i << std::endl;
        result->printPose();
        if (i == 0)
        {
            newpose = cv::ppf_match_3d::transformPCPose(image, result->pose);
        }
    }

    std::cout << newpose.rows << std::endl << newpose.cols << std::endl;
    std::cout << newpose.channels() << std::endl << newpose.type() << std::endl << std::endl;

    std::cout << "..........." << std::endl;

    cv::Matx44d poses;
    double residual = 0;
    icp->registerModelToScene(image, frontalFaceCloud, residual, poses);

    //newpose = cv::ppf_match_3d::transformPCPose(image, poses);

//    std::cout << image.rows << std::endl << image.cols << std::endl;
//    std::cout << image.channels() << std::endl << image.type() << std::endl << std::endl;

//    std::cout << imageCache.rows << std::endl << imageCache.cols << std::endl;
//    std::cout << imageCache.channels() << std::endl << imageCache.type() << std::endl << std::endl;

//    std::cout << "..........." << std::endl;

    cv::Mat newposeFm(imageCache.rows * 2, imageCache.cols * 2, CV_32FC1, cv::Scalar(1));
    for (int j = 0; j < newpose.rows; j++) {
        i = static_cast< std::vector<uint16_t>::size_type >(j * 3);

        int y = newpose.at<float>(i + 0);
        int x = newpose.at<float>(i + 1);

        std::cout << x << " " << y  << " | " ;

        if (y < newposeFm.rows && x < newposeFm.cols && x  && x > 0 && y > 0)
            newposeFm.at<float>(y, x) = newpose.at<float>(i + 2);
    }
    std::cout << std::endl;

//    for (int y = 0; y < imageCache.rows; y++) {
//        for (int x = 0; x < imageCache.cols; x++) {
//            i = static_cast< std::vector<uint16_t>::size_type >((y * imageCache.cols) + x) * 3;

//            newposeFm.at<float>(y, x) = newpose.at<float>(i + 2);
//        }
//    }

    cv::imshow("frontal", imageCache);
    //cv::imshow("frontal", frontalFace);
    //cv::imshow("processed", image);
    cv::imshow("new pose", newposeFm);
    cv::waitKey(100);

    return image;
}
