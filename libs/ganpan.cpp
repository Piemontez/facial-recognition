#include "ganpan.hpp"
#include "../loader/3dloader.hpp"

#include "math.h"
#include <opencv4/opencv2/face.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/surface_matching/ppf_helpers.hpp>

//https://levelup.gitconnected.com/facial-landmark-detection-in-opencv4-616f9c1737a5
#define REDUTOR 70.0
#define MULTIPLICADOR 2.0

cv::Mat eulerAnglesToRotationMatrix(const cv::Vec3d &theta);
cv::Matx44d rotationMatrixTo44d(cv::Mat);

GanPan::GanPan()
{

    const std::string facemark_filename = "data/lbfmodel.yaml";
    facemark = cv::face::createFacemarkLBF();

    facemark->loadModel(facemark_filename);

    faceD = new FaceDetection();
}

cv::Mat GanPan::proccess(const cv::Mat &image, int pos, ImageLoader* imgLoader)
{
    cv::Mat rgb = ((ThreeLoader*)imgLoader)->imagesRGB()[pos];

    //cv::imshow("processed", rgb);
    //cv::waitKey();

    std::vector< std::vector<cv::Point2f> > shapes;

    std::vector<cv::Rect> faces = faceD->getfaces(rgb);
    if (faces.size() > 0) {
        rgb = rgb(faces.front()).clone();
        cv::Mat roi = image(faces.front()).clone();

        faces.front().x = 0;
        faces.front().y = 0;

        if (facemark->fit(rgb, faces, shapes)) {
            cv::Mat rvec, tvec;
            this->estimatePoseDirection(rgb, shapes, rvec, tvec);

            cv::face::drawFacemarks(rgb, shapes[0], cv::Scalar(255, 255, 255));
            /*
            cv::imshow("rgbpoints", rgb);
            cv::moveWindow("rgbpoints",300,0);
            */
            this->rotateImage(roi, rvec, tvec);

            cv::Mat rs = cv::Mat(image.rows, image.cols, image.type(), cv::Scalar(255,255,255));
            roi.copyTo(rs(faces.front()));

            return rs;
        }
    }
    return image.clone();
}

void GanPan::estimatePoseDirection(const cv::Mat &image, const std::vector< std::vector<cv::Point2f> > &shapes, cv::Mat &rvec, cv::Mat &tvec)
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

void GanPan::rotateImage(cv::Mat &image, cv::Mat &rvec, cv::Mat &tvec)
{
    std::cout << "-------" << std::endl;
    std::cout << rvec << std::endl;
    std::cout << tvec << std::endl;

    int points = 0;
    {//Total de pontos da malha de rosto
        for (int y = 0; y < image.rows; y++) {
            for (int x = 0; x < image.cols; x++) {
                if (image.at<float>(y, x) > 0.99990 || !image.at<float>(y, x))
                    continue;
                points ++;
            }
        }
    }

    cv::Mat imageCloud = cv::Mat(points, 3, CV_32FC1, cv::Scalar(0));
    {//Cria image3D do rosto a partir da imagem de profundidade
        points = 0;
        std::vector<uint16_t>::size_type i;
        for (int y = 0; y < image.rows; y++) {
            for (int x = 0; x < image.cols; x++) {
                if (image.at<float>(y, x) > 0.99990 || image.at<float>(y, x) < 0.0001)
                    continue;

                //i = static_cast< std::vector<uint16_t>::size_type >((y * imageCache.cols) + x);

                imageCloud.at<float>(points, 0) = y/REDUTOR;
                imageCloud.at<float>(points, 1) = x/REDUTOR;
                imageCloud.at<float>(points, 2) = image.at<float>(y, x);

                points++;
            }
        }
    }

    //cv::reprojectImageTo3D(image, imageCloud, Q);

    //cv::Mat R;
    //cv::Rodrigues(rvec, R);
    //cv::Mat _rvec = rvec.clone();
    double i = 0;//-3.1415926535897;
    //for (double i = 0.0; i < 3.14; i += 0.025)
    {
        rvec.at<double>(0,0) = 0;//-rvec.at<double>(0,0);
        std::swap(rvec.at<double>(2,0), rvec.at<double>(0,0));
        rvec.at<double>(0,0) = -rvec.at<double>(0,0);
        cv::Mat r = eulerAnglesToRotationMatrix(cv::Vec3d(rvec));
        std::swap(rvec.at<double>(2,0), rvec.at<double>(0,0));

        cv::Matx44d pose = rotationMatrixTo44d(r);

        cv::Mat newpose = cv::ppf_match_3d::transformPCPose(imageCloud, pose);

        float minY = newpose.at<float>(0);
        float minX = newpose.at<float>(1);
        {
            for (int j = 0; j < newpose.rows; j++) {
                int i = static_cast< std::vector<uint16_t>::size_type >(j * 3);
                if (minY > newpose.at<float>(i + 0))
                    minY = newpose.at<float>(i + 0);
                if (minX > newpose.at<float>(i + 1))
                    minX = newpose.at<float>(i + 1);
            }
        }

        cv::Mat newposeFm(image.rows, image.cols, CV_32FC1, cv::Scalar(1));
        for (int j = 0; j < newpose.rows; j++) {
            int i = static_cast< std::vector<uint16_t>::size_type >(j * 3);

            float y = (newpose.at<float>(i + 0)-minY)*REDUTOR;
            float x = (newpose.at<float>(i + 1)-minX)*REDUTOR;

            if (y < newposeFm.rows && x < newposeFm.cols && x  && x > 0 && y > 0)
                newposeFm.at<float>((int)y, (int)x) = imageCloud.at<float>(i + 2);
        }

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
        cv::imshow("image", image);
        cv::imshow("newpose", newposeFm);
        cv::moveWindow("image",0,0);
        cv::moveWindow("newpose",0,200);
        cv::waitKey(50);
    }
    cv::waitKey();
}

// Calculates rotation matrix given euler angles.
#define T_X 0
#define T_Y 1
#define T_Z 2
cv::Mat eulerAnglesToRotationMatrix(const cv::Vec3d &theta)
{

    // Calculate rotation about x axis
    cv::Mat R_x = (cv::Mat_<double>(3,3) <<
               1,       0,              0,
               0,       cos(theta[T_X]),   -sin(theta[T_X]),
               0,       sin(theta[T_X]),   cos(theta[T_X])
               );

    // Calculate rotation about y axis
    cv::Mat R_y = (cv::Mat_<double>(3,3) <<
               cos(theta[T_Y]),    0,      sin(theta[T_Y]),
               0,               1,      0,
               -sin(theta[T_Y]),   0,      cos(theta[T_Y])
               );

    // Calculate rotation about z axis
    cv::Mat R_z = (cv::Mat_<double>(3,3) <<
               cos(theta[T_Z]),    -sin(theta[T_Z]),      0,
               sin(theta[T_Z]),    cos(theta[T_Z]),       0,
               0,               0,                  1);


    // Combined rotation cv::Matrix
    cv::Mat R = R_z * R_y * R_x;

    return R;

}

cv::Matx44d rotationMatrixTo44d(cv::Mat r)
{
    return cv::Matx44d(r.at<double>(0,0), r.at<double>(0,1), r.at<double>(0,2), 0,
                     r.at<double>(1,0), r.at<double>(1,1), r.at<double>(1,2), 0,
                     r.at<double>(2,0), r.at<double>(2,1), r.at<double>(2,2), 0,
                     0, 0, 0, 1);
}
