#include "icp.hpp"
#include "../tools.hpp"

#include "icp/icpPointToPlane.h"
#include "icp/icpPointToPoint.h"

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/surface_matching.hpp>
#include <opencv4/opencv2/surface_matching/ppf_helpers.hpp>

#define BY_OPENCV_ICP false

//https://docs.opencv.org/4.1.2/d9/d25/group__surface__matching.html

ICP::ICP(const cv::Mat &frontalFace)
{
    this->frontalFace = frontalFace.clone();

    icp = new cv::ppf_match_3d::ICP(200, 0.04, 2.5f, 4, cv::ppf_match_3d::ICP::ICP_SAMPLING_TYPE_UNIFORM, 60);
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

    cv::Mat pc;
    frontalFaceCloud.convertTo(pc, CV_64FC1);
    icpExternal = new IcpPointToPlane((double *)pc.data, pc.rows, 3, 50, 5.0);
    frontalFaceCloud_0_9.convertTo(pc, CV_64FC1);
    icpExternal_0_9 = new IcpPointToPlane((double *)pc.data, pc.rows, 3, 50, 5.0);
    frontalFaceCloud_1_1.convertTo(pc, CV_64FC1);
    icpExternal_1_1 = new IcpPointToPlane((double *)pc.data, pc.rows, 3, 50, 5.0);
    //IcpPointToPoint icp;
//    icpExternal->setMinDeltaParam(0.001);
//    icpExternal_0_9->setMinDeltaParam(0.001);
//    icpExternal_1_1->setMinDeltaParam(0.001);

    std::cout << "ICP: Modelo 3D criado." << std::endl;

    /*
    for (float j = .0; j < 12.0; j += 0.1)
    {
        cv::Matx44d pose = tools::rotationMatrixTo44d(
            tools::eulerAnglesToRotationMatrix(cv::Vec3d(0.0, j, 0.0))
        );

        cv::Mat newpose = cv::ppf_match_3d::transformPCPose(frontalFaceCloud, pose);

        cv::Mat newposeFm = cv::Mat(frontalFace.rows*2, frontalFace.cols*3, CV_32FC1, cv::Scalar(0));
        tools::pointCloudToDepthImg(newpose, newposeFm, {true, true, true}, {1.0, 1.0, 0.0});

        cv::imshow("rotation", newposeFm);
        cv::waitKey(80);
    }
    */



//    std::cout << "ICP: Trainando detector de posição ." << std::endl;
//     Now train the model
//    int64 tick1 = cv::getTickCount();
//    detector = new cv::ppf_match_3d::PPF3DDetector(0.06, 0.08, 20);
//    detector = new cv::ppf_match_3d::PPF3DDetector(0.06, 0.08, 25);
//    detector->trainModel(frontalFaceCloud);
//    int64 tick2 = cv::getTickCount();
//    std::cout << "ICP: Detector trainado." << std::endl;
}

cv::Mat ICP::proccess(const cv::Mat &imageCache, int pos, ImageLoader* imgLoader)
{
//    cv::Mat imageCache = _imageCache.clone();
//    cv::Mat imageCache = frontalFaceCloud.clone();
    cv::Mat imageCloud;
    cv::Mat teste;

    assert(this->frontalFace.type() == CV_32F);
    //assert(imageCache.type() == CV_32F);

    if (imageCache.cols == 3 || imageCache.cols == 6) {
        imageCache.copyTo(imageCloud);
        teste = cv::Mat(frontalFace.rows, frontalFace.cols, CV_32FC1, cv::Scalar(0));

        /*if (imageCache.cols == 6) {
            tools::moveToCenter(imageCloud, {true, true}, {static_cast<float>(frontalFace.rows), static_cast<float>(frontalFace.cols)});
            cv::Mat newposeFm = cv::Mat(frontalFace.rows, frontalFace.cols, CV_32FC3, cv::Scalar(0));
            tools::pointCloudToRGBImg(imageCloud, newposeFm, {true, true, true});
            tools::saveImgProc(newposeFm, "-ROI-RGB", pos, 1);
            imageCache.copyTo(imageCloud);
        }*/

        tools::moveToRigth(imageCloud);
        tools::pointCloudToDepthImg(imageCloud, teste, {true, true, true});
        if (imageCache.cols == 6)
            tools::depthImgToPointCloud(teste, imageCloud, {true, true, true}, 0.001);
//        cv::imshow("frontala", imageCache);
    } else {
//        cv::imshow("frontalb", imageCache);
        teste = imageCache;
        tools::depthImgToPointCloud(imageCache, imageCloud, {true, true, true}, 0.001);
    }

//    cv::waitKey();

//    cv::imshow("frontal", this->frontalFace);
//    cv::moveWindow("frontal",0,0);
//    cv::imshow("teste", teste);
//    cv::moveWindow("teste",  0,300);

    cv::Matx44d bestPose;
    double lastResidual = 0;
    if (BY_OPENCV_ICP)
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

        if (residual > 0 && (cv::abs(residual) < lastResidual || 0 == lastResidual)) {
            bestPose = pose;
            lastResidual = cv::abs(residual);
        }

        //cv::Mat newpose = cv::ppf_match_3d::transformPCPose(imageCloud, pose);
        //std::cout << "residual:"  << residual << std::endl;
        //std::printf("residual: %f\n", residual);

        /*
        cv::Mat newposeFm;
        if (imageCache.cols == 3) {
             newposeFm = cv::Mat(frontalFace.rows*2, frontalFace.cols*3, CV_32FC1, cv::Scalar(0));
        } else {
            newposeFm = cv::Mat(imageCache.rows, imageCache.cols, CV_32FC1, cv::Scalar(0));
        }
        tools::pointCloudToDepthImg(newpose, newposeFm, {true, true, true}, {1.0, 1.0, 0.0});

        cv::imshow("new pose" + l, newposeFm);
        cv::moveWindow("new pose" + l, 220 + (l%5 * 180), 150 + (l < 5 ? 0 : 200));
        */
    }

    Matrix R = Matrix::eye(3);
    Matrix t(3,1);
    double residual = 0;

    try {
        if (!BY_OPENCV_ICP)
        for (int l =0; l < 3; l++) {

            imageCloud.convertTo(teste, CV_64FC1);

            try {
                switch (l) {
                case 0: residual = icpExternal->fit((double *)teste.data, teste.rows, R, t, -1); break;
                case 1: residual = icpExternal_0_9->fit((double *)teste.data, teste.rows, R, t, -1); break;
                case 2: residual = icpExternal_1_1->fit((double *)teste.data, teste.rows, R, t, -1); break;
                }
            }  catch (...) {
                residual = lastResidual;
            }

            std::printf("ext. residual: %f\n", residual);

            cv::Matx44d pose = tools::rotationMatrixTo44d((cv::Mat_<double>(3, 3) <<
                R.val[0][0], R.val[0][1], R.val[0][2],
                R.val[1][0], R.val[1][1], R.val[1][2],
                R.val[2][0], R.val[2][1], R.val[2][2]
            ));

            if (residual > 0 && (cv::abs(residual) < lastResidual || 0 == lastResidual)) {
                bestPose = pose;
                lastResidual = cv::abs(residual);
            }

            /*
            cv::Mat newpose = cv::ppf_match_3d::transformPCPose(imageCloud, pose);

            cv::Mat newposeFm = cv::Mat(frontalFace.rows*2, frontalFace.cols*3, CV_32FC1, cv::Scalar(0));
            tools::pointCloudToDepthImg(newpose, newposeFm, {true, true, true}, {1.5, 1.5, 0.0});

            cv::imshow("external" + l, newposeFm);
            cv::moveWindow("external" + l,  220 + (l%5 * 180), 150 + (l < 5 ? 0 : 200));
            */
        }
    } catch (...) {}

    cv::Mat newposeFm;
    if (residual == 0) {
        tools::pointCloudToDepthImg(imageCloud, newposeFm, {true, true, true});
        cv::imshow("cache", imageCache);
        cv::imshow("depth", newposeFm);
        cv::waitKey();
    } else {
        cv::Mat newpose = tools::transformPCPose(imageCloud, bestPose);
        tools::moveToCenter(newpose, {true, true}, {static_cast<float>(frontalFace.rows), static_cast<float>(frontalFace.cols)});

        /*if (imageCache.cols == 6) {
            newposeFm = cv::Mat(frontalFace.rows, frontalFace.cols, CV_32FC3, cv::Scalar(0));
            tools::pointCloudToRGBImg(newpose, newposeFm, {true, true, true});
            tools::saveImgProc(newposeFm, "-ROI-PoseCorrection-RGB", pos, 2, true);
            //cv::imshow("rgb", newposeFm);
        }*/

        if (imageCache.cols == 3 || imageCache.cols == 6) {
            newposeFm = cv::Mat(frontalFace.rows, frontalFace.cols, CV_32FC1, cv::Scalar(0));
        } else {
            newposeFm = cv::Mat(imageCache.rows, imageCache.cols, CV_32FC1, cv::Scalar(0));
        }

        tools::pointCloudToDepthImg(newpose, newposeFm, {true, true, true});
        /*cv::imshow("depth", newposeFm);
        cv::waitKey(500);*/
    }

    return newposeFm;
}
