#include "ganpan.hpp"
#include "../loader/3dloader.hpp"

#include <opencv4/opencv2/opencv.hpp>
#include <opencv2/face.hpp>

//https://levelup.gitconnected.com/facial-landmark-detection-in-opencv4-616f9c1737a5

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
        cv::Mat roi = cv::Mat(image.rows, image.cols, rgb.type(), cv::Scalar(255,255,255));
        rgb(faces.front()).copyTo(roi(faces.front()));

        //cv::imshow("processed", roi);
        //cv::waitKey();
        if (facemark->fit(roi, faces, shapes)) {
            //cv::face::drawFacemarks(roi, shapes[0], cv::Scalar(255, 255, 255));

            cv::Mat rvec, tvec;
            this->estimatePoseDirection(roi, shapes, rvec, tvec);
            this->rotateImage(roi, rvec, tvec);
        }
    }
    return image;
}

void GanPan::estimatePoseDirection(const cv::Mat &roi, const std::vector< std::vector<cv::Point2f> > &shapes, cv::Mat &rvec, cv::Mat &tvec)
{
    std::vector<cv::Point3f> objectPoints {
      {8.27412, 1.33849, 10.63490},    //left eye corner
      {-8.27412, 1.33849, 10.63490},   //right eye corner
        {0, -4.47894, 17.73010},         //nose tip
      {-4.61960, -10.14360, 12.27940}, //right mouth corner
      {4.61960, -10.14360, 12.27940},  //left mouth corner
        {0, -14.47894, 10.63490},         //chin
    };
    std::vector<int> landmarksIDsFor3DPoints {45, 36, 30, 48, 54, 9};

    std::vector<cv::Point2f> points2d;
    for (int pId : landmarksIDsFor3DPoints) {
      points2d.push_back(shapes[0][pId]);
    }

    cv::Point2d center = cv::Point2d(roi.cols/2, roi.rows/2);
    cv::Mat cameraMatrix = (cv::Mat_<double>(3,3) << roi.cols, 0, center.x, 0 , roi.cols, center.y, 0, 0, 1);

    cv::solvePnP(cv::Mat(objectPoints), cv::Mat(points2d), cameraMatrix, cv::Mat(), rvec, tvec, false, cv::SOLVEPNP_DLS);
}

void GanPan::rotateImage(cv::Mat &roi, cv::Mat &rvec, cv::Mat &tvec)
{
    std::cout << "-------_" << std::endl;
    std::cout << rvec << std::endl;
    std::cout << tvec << std::endl;

    cv::Point3f nosePos = {0, -4.47894, 17.73010};
    std::vector<cv::Point3f> objectPointsForReprojection {
      nosePos,                   // tip of nose
      nosePos + cv::Point3f(0,0,15), // nose and Z-axis
      nosePos + cv::Point3f(0,15,0), // nose and Y-axis
      nosePos + cv::Point3f(15,0,0)  // nose and X-axis
    };

    cv::Point2d center = cv::Point2d(roi.cols/2, roi.rows/2);
    cv::Mat cameraMatrix = (cv::Mat_<double>(3,3) << roi.cols, 0, center.x, 0 , roi.cols, center.y, 0, 0, 1);

    std::vector<cv::Point2f> projectionOutput(objectPointsForReprojection.size());
    cv::projectPoints(objectPointsForReprojection, rvec, tvec, cameraMatrix, cv::Mat(), projectionOutput);
    cv::arrowedLine(roi, projectionOutput[0], projectionOutput[1], cv::Scalar(255,255,0),2);
    cv::arrowedLine(roi, projectionOutput[0], projectionOutput[2], cv::Scalar(0,255,255),2);
    cv::arrowedLine(roi, projectionOutput[0], projectionOutput[3], cv::Scalar(255,0,255),2);

    cv::imshow("processed", roi);
    cv::waitKey();
}
