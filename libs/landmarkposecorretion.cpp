#include "landmarkposecorretion.hpp"
#include "../tools.hpp"
#include "../loader/3dloader.hpp"

#include "math.h"
#include <opencv4/opencv2/face.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/surface_matching/ppf_helpers.hpp>
#include <opencv4/opencv2/surface_matching/pose_3d.hpp>

//https://levelup.gitconnected.com/facial-landmark-detection-in-opencv4-616f9c1737a5

LandMarkPoseCorrection::LandMarkPoseCorrection()
{

    const std::string facemark_filename = "data/lbfmodel.yaml";
    facemark = cv::face::createFacemarkLBF();

    facemark->loadModel(facemark_filename);

    faceD = new FaceDetection();
}

cv::Mat LandMarkPoseCorrection::proccess(const cv::Mat &image, int pos, ImageLoader* imgLoader)
{
//    cv::imshow("teste", image);
//    cv::moveWindow("teste",  0,300);

    cv::Mat rgb = ((ThreeLoader*)imgLoader)->imagesRGB()[pos];

    //cv::imshow("processed", rgb);
    //cv::waitKey();

    std::vector< std::vector<cv::Point2f> > shapes;

    std::vector<cv::Rect> faces = faceD->getfaces(rgb);
    if (faces.size() > 0) {
        rgb = rgb(faces.front()).clone();

        /*{
            cv::Mat newposeFm = cv::Mat(image.rows, image.cols, CV_8UC3, cv::Scalar(0));
            rgb.copyTo(newposeFm(faces.front()));
            tools::saveImgProc(newposeFm, "-ROI-RGB", pos, 1, true);
        }*/

        /*Normaliza pose extraida com o processo ROI*/
        cv::Rect front = faces.front();
        front.x = (image.cols / 2) - (+ front.width / 2);
        front.y = (image.rows / 2) - (+ front.height / 2);
        /**/

        cv::Mat roi = image(front).clone();

        faces.front().x = 0;
        faces.front().y = 0;

        if (facemark->fit(rgb, faces, shapes)) {
            cv::Mat rvec, tvec;
            this->estimatePoseDirection(rgb, shapes, rvec, tvec);

            //cv::face::drawFacemarks(rgb, shapes[0], cv::Scalar(255, 255, 255));
//            cv::imshow("rgbpoints", rgb);
//            cv::imshow("roi", roi);
//            cv::waitKey();

            this->rotateImage(roi.clone(), rgb, roi, rvec, tvec);

            /*
            cv::Mat rs = cv::Mat(image.rows, image.cols, image.type(), cv::Scalar(255,255,255));

            front.x = (rs.cols / 2) - (+ front.width / 2);
            front.y = (rs.rows / 2) - (+ front.height / 2);

            roi.copyTo(rs(front));
            return rs;
            */

            return roi;
        }
    }
    return image.clone();
}

void LandMarkPoseCorrection::estimatePoseDirection(const cv::Mat &image, const std::vector< std::vector<cv::Point2f> > &shapes, cv::Mat &rvec, cv::Mat &tvec)
{
    std::vector<cv::Point3f> objectPoints {
      {8.27412, 1.33849, 10.63490},    //left eye corner
      {-8.27412, 1.33849, 10.63490},   //right eye corner
      {0, -4.47894, 17.73010},         //nose tip
      {-4.61960, -10.14360, 12.27940}, //right mouth corner
      {4.61960, -10.14360, 12.27940},  //left mouth corner
      {0, -4.47894, 17.73010},         //nose tip
      //{0, -14.47894, 10.63490},         //chin
    };
    for (auto && p3f: objectPoints) {
        p3f.x += 0;
        p3f.y += 4.47894;
        p3f.z += -17.73010;
    }

    //std::vector<int> landmarksIDsFor3DPoints {45, 36, 30, 48, 54, 9};
    std::vector<int> landmarksIDsFor3DPoints {45, 36, 30, 48, 54, 30};

    std::vector<cv::Point2f> points2d;
    for (int pId : landmarksIDsFor3DPoints) {
      points2d.push_back(shapes[0][pId]);
    }

    cv::Point2d center = cv::Point2d(image.cols/2, image.rows/2);
    cv::Mat cameraMatrix = (cv::Mat_<double>(3,3) << image.cols, 0, center.x, 0 , image.cols, center.y, 0, 0, 1);

    cv::solvePnP(cv::Mat(objectPoints), cv::Mat(points2d), cameraMatrix, cv::Mat(), rvec, tvec, false, cv::SOLVEPNP_DLS);
    //cv::solvePnPRansac(cv::Mat(objectPoints), cv::Mat(points2d), cameraMatrix, cv::Mat(), rvec, tvec, false);


    //cv::Point3f nosePos = {0, -4.47894, 17.73010};
    /*
    cv::Point3f nosePos = {0, -0, 0};
    std::vector<cv::Point3f> objectPointsForReprojection {
      nosePos,                   // tip of nose
      nosePos + cv::Point3f(0,0,20), // nose and Z-axis
      nosePos + cv::Point3f(0,20,0), // nose and Y-axis
      nosePos + cv::Point3f(20,0,0),  // nose and X-axis
    };

    std::vector<cv::Point2f> projectionOutput(objectPointsForReprojection.size());
    cv::projectPoints(objectPointsForReprojection, rvec, tvec, cameraMatrix, cv::Mat(), projectionOutput);
    cv::arrowedLine(image, projectionOutput[0], projectionOutput[3], cv::Scalar(255,0,0),1); //b
    cv::arrowedLine(image, projectionOutput[0], projectionOutput[2], cv::Scalar(0,255,0),1); //g
    cv::arrowedLine(image, projectionOutput[0], projectionOutput[1], cv::Scalar(0,0,255),1); //r

    cv::imshow("rgbarrow", image);
    cv::moveWindow("rgbarrow",600,0);
    */
}

void LandMarkPoseCorrection::rotateImage(const cv::Mat &depth, const cv::Mat &color, cv::Mat &output, cv::Mat &rvec, cv::Mat &tvec)
{
    cv::Mat imageCloud;
    cv::Mat rgbF;
    if (color.type() == CV_8UC3)
        color.convertTo(rgbF, CV_32FC3, 1 / 255.0);
    else
        rgbF = color;
    tools::depthImgToPointCloud(depth, rgbF, imageCloud, {true, true, true}, 0.1, {.0, .0, .0});

//    for (float l = 0; l < 7; l += 0.01)
//    {
//        rvec.at<double>(0,0) = 0;
//        rvec.at<double>(1,0) = 0;
//        rvec.at<double>(2,0) = l;

        rvec.at<double>(0,0) = 0;//-rvec.at<double>(0,0);
        std::swap(rvec.at<double>(2,0), rvec.at<double>(0,0));
        rvec.at<double>(0,0) = -rvec.at<double>(0,0);
        cv::Mat r = tools::eulerAnglesToRotationMatrix(cv::Vec3d(rvec));
        std::swap(rvec.at<double>(2,0), rvec.at<double>(0,0));

        cv::Matx44d pose = tools::rotationMatrixTo44d(r);
        //cv::Mat newpose = cv::ppf_match_3d::transformPCPose(imageCloud, pose);
        cv::Mat newpose = tools::transformPCPose(imageCloud, pose);

        output = cv::Mat(newpose.rows, newpose.cols, newpose.type(), cv::Scalar(0));
        /*
        tools::moveToRigth(newpose);

        output = cv::Mat(depth.rows, depth.cols, CV_32FC1, cv::Scalar(0));
        tools::pointCloudToDepthImg(newpose, output, {true, true, true});
        cv::imshow("output", output);

        output = cv::Mat(depth.rows, depth.cols, CV_32FC3, cv::Scalar(0));
        tools::pointCloudToRGBImg(newpose, output, {true, true});
        cv::imshow("rgb", output);
        cv::waitKey();

        {
            tools::pointCloudToDepthImg(newpose, output, {true, true, true});
            tools::depthImgToPointCloud(output, newpose, {true, true, true}, 0.1, {.0, .0, .0});
        }
        */

    //        if (i == 0)
    //        {
    //            cv::Point3f nosePos = {0, -4.47894, 17.73010};
    //            std::vector<cv::Point3f> objectPointsForReprojection {
    //              nosePos,                   // tip of nose
    //              nosePos + cv::Point3f(0,0,20), // nose and Z-axis
    //              nosePos + cv::Point3f(0,20,0), // nose and Y-axis
    //              nosePos + cv::Point3f(20,0,0),  // nose and X-axis
    //            };

    //            cv::Point2d center = cv::Point2d(image.cols/2, image.rows/2);
    //            cv::Mat cameraMatrix = (cv::Mat_<double>(3,3) << image.cols, 0, center.x, 0 , image.cols, center.y, 0, 0, 1);

    //            std::vector<cv::Point2f> projectionOutput(objectPointsForReprojection.size());
    //            cv::projectPoints(objectPointsForReprojection, rvec, tvec, cameraMatrix, cv::Mat(), projectionOutput);
    //            cv::arrowedLine(image, projectionOutput[0], projectionOutput[1], cv::Scalar(255,255,255),1);
    //            cv::arrowedLine(image, projectionOutput[0], projectionOutput[2], cv::Scalar(255,255,255),1);
    //            cv::arrowedLine(image, projectionOutput[0], projectionOutput[3], cv::Scalar(255,255,255),1);
    //        }
        //std::cout << objectPointsForReprojection << std::endl;
        //std::cout << projectionOutput << std::endl;

        //cv::imshow("imageCloud", imageCloud);

//        cv::moveWindow("image",600,0);
//        cv::moveWindow("newpose",0,200);
//        cv::waitKey();
//    }
//    cv::waitKey();
    newpose.copyTo(output);
}
