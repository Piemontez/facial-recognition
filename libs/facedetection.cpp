#include "facedetection.hpp"

FaceDetection::FaceDetection()
{

}

cv::Mat FaceDetection::proccess(const cv::Mat &image)
{
    /*
    const string cascade_name = "$OPENCV_ROOT/data/haarcascades/haarcascade_frontalface_default.xml";
    CascadeClassifier face_cascade;
    if (not face_cascade.load(cascade_name)) {
         cerr << "Cannot load cascade classifier from file: " <<    cascade_name << endl;
    return -1;
    }
    // ... obtain an image in img
    vector<Rect> faces;
    faceDetector(img, faces, face_cascade);
    // Check if any faces were detected or not
    if (faces.size() == 0) {
        cerr << "Cannot detect any faces in the image." << endl;
        return -1;
    }
    */
}
