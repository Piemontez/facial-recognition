#include "icp.hpp"
#include "../tools.hpp"

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/surface_matching.hpp>
#include <opencv4/opencv2/surface_matching/ppf_helpers.hpp>

//https://docs.opencv.org/4.1.2/d9/d25/group__surface__matching.html

ICP::ICP(const cv::Mat &frontalFace)
{
    this->frontalFace = frontalFace.clone();

    icp = new cv::ppf_match_3d::ICP(100, 0.10, 2.5f, 4, cv::ppf_match_3d::ICP::ICP_SAMPLING_TYPE_UNIFORM);
    //icp = new cv::ppf_match_3d::ICP(100);

    tools::depthImgToPointCloud(frontalFace, frontalFaceCloud, {true, true, true}, 0.1);
    tools::moveToRigth(frontalFaceCloud);

    frontalFaceCloud_0_8   = frontalFaceCloud * 0.8;
    frontalFaceCloud_0_9   = frontalFaceCloud * 0.9;
    frontalFaceCloud_0_95  = frontalFaceCloud * 0.95;
    frontalFaceCloud_0_975 = frontalFaceCloud * 0.975;
    frontalFaceCloud_1_025 = frontalFaceCloud * 1.025;
    frontalFaceCloud_1_05  = frontalFaceCloud * 1.05;
    frontalFaceCloud_1_1   = frontalFaceCloud * 1.1;
    frontalFaceCloud_1_2   = frontalFaceCloud * 1.2;

    cv::Mat teste = cv::Mat(frontalFace.rows, frontalFace.cols, CV_32FC1, cv::Scalar(0));
    tools::pointCloudToDepthImg(frontalFaceCloud, teste, {true, true, true}, 0.1);

    std::cout << "ICP: Modelo 3D criado." << std::endl;

//    std::cout << "ICP: Trainando detector de posição ." << std::endl;
//     Now train the model
//    int64 tick1 = cv::getTickCount();
//    detector = new cv::ppf_match_3d::PPF3DDetector(0.06, 0.08, 20);
//    detector = new cv::ppf_match_3d::PPF3DDetector(0.06, 0.08, 25);
//    detector->trainModel(frontalFaceCloud);
//    int64 tick2 = cv::getTickCount();
//    std::cout << "ICP: Detector trainado." << std::endl;
}

cv::Mat ICP::proccess(const cv::Mat &_imageCache, int pos, ImageLoader* imgLoader)
{
    cv::Mat imageCache = _imageCache.clone();
    //cv::Mat imageCache = frontalFaceCloud.clone();
    cv::Mat imageCloud;
    cv::Mat teste;

    assert(this->frontalFace.type() == CV_32F);
    assert(imageCache.type() == CV_32F);

    if (imageCache.cols == 3) {
        imageCache.copyTo(imageCloud);
        teste = cv::Mat(frontalFace.rows, frontalFace.cols, CV_32FC1, cv::Scalar(0));

        tools::moveToRigth(imageCloud);
        tools::pointCloudToDepthImg(imageCloud, teste, {true, true, true});
    } else {
        teste = imageCache;
        tools::depthImgToPointCloud(imageCache, imageCloud, {true, true, true}, 0.1);
    }

    cv::imshow("frontal", this->frontalFace);
    cv::imshow("teste", teste);
    cv::moveWindow("frontal",0,0);
    cv::moveWindow("teste",300,0);

    for (int l =0; l < 9; l++) {

        cv::Matx44d pose;
        double residual = 0;

        switch (l) {
        case 0: icp->registerModelToScene(imageCloud, frontalFaceCloud.clone(), residual, pose); break;
        case 1: icp->registerModelToScene(imageCloud, frontalFaceCloud_0_8.clone(), residual, pose); break;
        case 2: icp->registerModelToScene(imageCloud, frontalFaceCloud_0_9.clone(), residual, pose); break;
        case 3: icp->registerModelToScene(imageCloud, frontalFaceCloud_0_95.clone(), residual, pose); break;
        case 4: icp->registerModelToScene(imageCloud, frontalFaceCloud_0_975.clone(), residual, pose); break;
        case 5: icp->registerModelToScene(imageCloud, frontalFaceCloud_1_025.clone(), residual, pose); break;
        case 6: icp->registerModelToScene(imageCloud, frontalFaceCloud_1_05.clone(), residual, pose); break;
        case 7: icp->registerModelToScene(imageCloud, frontalFaceCloud_1_1.clone(), residual, pose); break;
        case 8: icp->registerModelToScene(imageCloud, frontalFaceCloud_1_2.clone(), residual, pose); break;
        }
        cv::Mat newpose = cv::ppf_match_3d::transformPCPose(imageCloud, pose);
        //std::cout << "residual:"  << residual << std::endl;
        std::printf("residual: %f\n", residual);

        cv::Mat newposeFm;
        if (imageCache.cols == 3) {
             newposeFm = cv::Mat(frontalFace.rows*2, frontalFace.cols*3, CV_32FC1, cv::Scalar(0));
        } else {
            newposeFm = cv::Mat(imageCache.rows, imageCache.cols, CV_32FC1, cv::Scalar(0));
        }
        tools::pointCloudToDepthImg(newpose, newposeFm, {true, true, true}, {1.0, 1.0, 0.0});

        cv::imshow("new pose" + l, newposeFm);
        cv::moveWindow("new pose" + l, 220 + (l%5 * 180), 150 + (l < 5 ? 0 : 200));

    }
    std::cout << "-------" << std::endl;
    cv::waitKey();
    return imageCloud;
}


/*
        // Match the model to the scene and get the pose
    //    std::vector<cv::ppf_match_3d::Pose3DPtr>  posesDetect;
    //    detector->match(imageCloud, posesDetect, 1.0 / 5.0, 0.05);

    //    icp->registerModelToScene(imageCloud, frontalFaceCloud, posesDetect);
    //    for (size_t k = 0; k < posesDetect.size(); k++)
    //    {
    //        auto result = posesDetect[k];
    //        result->printPose();
    //        result->pose(3,3) = 0.5;
    //        result->printPose();

    //        newpose = cv::ppf_match_3d::transformPCPose(imageCloud, result->pose);
*/
