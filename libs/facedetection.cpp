#include "facedetection.hpp"
#include <iostream>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/opencv.hpp>

FaceDetection::FaceDetection()
{
    const std::string cascade_name = "data/haarcascades/haarcascade_frontalface_default.xml";

    if (not face_cascade.load(cascade_name)) {
         std::cerr << "Cannot load cascade classifier from file: " <<    cascade_name << std::endl;
    }
}

cv::Mat FaceDetection::proccess(const cv::Mat &image, int pos, ImageLoader* imgLoader)
{
    std::vector<cv::Rect> faces = getfaces(image);

    if (faces.size() > 1) {
        std::cerr << "Detect many faces in the image." << std::endl;
    }
    else if (faces.size() == 0) {
        return cv::Mat();
    }
    return cv::Mat(image, faces.front());
}
std::vector<cv::Rect> FaceDetection::getfaces(const cv::Mat &image)
{
    // ... obtain an image in img
    std::vector<cv::Rect> faces;

    cv::Mat gray;
    // The cascade classifier works best on grayscale images
    if (image.channels() > 1) {
      cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    } else {
      gray = image.clone();
      image.convertTo(gray, CV_8UC1, 255);
    }

    // Histogram equalization generally aids in face detection
    cv::equalizeHist(gray, gray);

    // Run the cascade classifier
    face_cascade.detectMultiScale(
      gray,
      faces,
      1.1, // pyramid scale factor
      3,   // lower thershold for neighbors count
           // here we hint the classifier to only look for one face
      cv::CASCADE_SCALE_IMAGE + cv::CASCADE_FIND_BIGGEST_OBJECT),
      cv::Size(20, 20);


    //cv::face::faceDetector(img, faces, face_cascade);
    // Check if any faces were detected or not
    if (faces.size() == 0) {
        std::cerr << "Cannot detect any faces in the image." << std::endl;
    }
//    else {
//        cv::imshow("processed", cv::Mat(image, faces.front()));
//        cv::waitKey(100);
//    }

    return faces;
}
