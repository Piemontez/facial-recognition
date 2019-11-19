#include "facedetection.hpp"
#include <iostream>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/opencv.hpp>

FaceDetection::FaceDetection()
{
    //cascade_name = "data/haarcascades/haarcascade_frontalface_default.xml";
    //cascade_name = "data/haarcascades/haarcascade_frontalface_alt.xml";
    //cascade_name = "data/haarcascades/haarcascade_frontalface_alt2.xml";
    //cascade_name = "data/haarcascades/haarcascade_frontalface_alt_tree.xml";

    std::string cascade_name;

    cascade_name = "data/haarcascades/rl/haarcascade_frontalface_default.xml";
    if (not face_cascade.load(cascade_name)) {
         std::cerr << "Cannot load cascade classifier from file: " <<    cascade_name << std::endl;
    }

    cascade_name = "data/haarcascades/rl/haarcascade_frontalface_alt_tree.xml";
    if (not face_cascade_tree.load(cascade_name)) {
         std::cerr << "Cannot load cascade classifier from file: " <<    cascade_name << std::endl;
    }

    cascade_name = "data/haarcascades/haarcascade_profileface.xml";
    if (not profileface_cascade.load(cascade_name)) {
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

    // frontal false
    face_cascade_tree.detectMultiScale(
      gray,
      faces,
      1.1, // pyramid scale factor
      5,   // lower thershold for neighbors count
           // here we hint the classifier to only look for one face
      flags,
      cv::Size(20, 20));

    if (faces.size() > 0)
        return faces;

    // frontal false
    face_cascade.detectMultiScale(
      gray,
      faces,
      1.1, // pyramid scale factor
      3,   // lower thershold for neighbors count
           // here we hint the classifier to only look for one face
      flags,
      cv::Size(20, 20));

    if (faces.size() > 0)
        return faces;

    //Profile face
    profileface_cascade.detectMultiScale(
      gray,
      faces,
      1.1, // pyramid scale factor
      3,   // lower thershold for neighbors count
           // here we hint the classifier to only look for one face
      flags,
      cv::Size(20, 20));

    if (faces.size() > 0)
        return faces;

    //Revert profile face
    cv::flip(gray, gray, 1);

    profileface_cascade.detectMultiScale(
      gray,
      faces,
      1.1, // pyramid scale factor
      3,   // lower thershold for neighbors count
           // here we hint the classifier to only look for one face
      flags,
      cv::Size(20, 20));
    if (faces.size() > 0) {
        for (auto && face: faces) {
            face.x = image.cols - face.br().x;
        }
        return faces;
    }

    std::cerr << "Cannot detect any faces in the image." << std::endl;

    return faces;
}
