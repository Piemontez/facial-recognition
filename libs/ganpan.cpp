#include "ganpan.hpp"

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

cv::Mat GanPan::proccess(const cv::Mat &image)
{
    /*
    std::vector< std::vector<cv::Point2f> > shapes;

    //std::vector<cv::Rect> faces = faceD->getfaces(image);
    std::vector<cv::Rect> faces;faces.push_back(cv::Rect(0, 0, image.cols, image.rows));

    cv::imshow("processed", image);
    cv::waitKey();

    if (facemark->fit(image, faces, shapes)) {
        cv::face::drawFacemarks(image, shapes[0], cv::Scalar(0, 0, 255));
    }

    cv::imshow("processed", image);
    cv::waitKey();
    */

    return image;
}
