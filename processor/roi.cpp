#include "roi.hpp"
#include "../loader/3dloader.hpp"

#include "opencv4/opencv2/opencv.hpp"


ROI::ROI()
{
    /*const std::string facemark_filename = "data/lbfmodel.yaml";
    facemark = cv::face::createFacemarkLBF();

    facemark->loadModel(facemark_filename);*/

    faceD = new FaceDetection();
}

int detected = 0;
int ndetected = 0;
cv::Mat ROI::proccess(const cv::Mat &image, int pos, ImageLoader* imgLoader)
{
    cv::Mat rgb = ((ThreeLoader*)imgLoader)->imagesRGB()[pos];

//    cv::imshow("image", image);
//    cv::imshow("rgb", rgb);
//    cv::waitKey();

    std::vector<cv::Rect> faces = faceD->getfaces(rgb);
    //cv::Mat face = faceD->proccess(rgb);
    if (faces.size() > 0) {
        cv::Mat roi = cv::Mat(image.rows, image.cols, image.type(), cv::Scalar(1));
        image(faces.front()).copyTo(roi(faces.front()));


//        std::vector< std::vector<cv::Point2f> > shapes;
//        if (facemark->fit(rgb, faces, shapes)) {
//            cv::face::drawFacemarks(rgb, shapes[0], cv::Scalar(0, 0, 255));
//        }

//        cv::imshow("rgb", rgb(faces.front()));
//        cv::imshow("roi", roi);
//        cv::waitKey();
        //std::cout << "+" << detected++ << std::endl;
        return roi;
    } else {
        //std::cout << "-" << ndetected++ << std::endl;
        /*cv::imshow("rgb", rgb);
        cv::imshow("roi", roi);
        cv::waitKey();*/
        return image.clone();
    }
}