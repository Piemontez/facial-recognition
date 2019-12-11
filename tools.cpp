#include "tools.hpp"

#include <opencv4/opencv2/opencv.hpp>

void tools::moveToRigth(cv::Mat &inout)
{
    //if (inout.channels() == 3)
    {
        float minY = inout.at<float>(0);
        float minX = inout.at<float>(1);
        float maxY = inout.at<float>(0);
        float maxX = inout.at<float>(1);
        {
            for (int j = 0; j < inout.rows; j++) {
                int i = static_cast< std::vector<uint16_t>::size_type >(j * 3);
                if (minY > inout.at<float>(i + 0))
                    minY = inout.at<float>(i + 0);
                else if (maxY < inout.at<float>(i + 0))
                    maxY = inout.at<float>(i + 0);

                if (minX > inout.at<float>(i + 1))
                    minX = inout.at<float>(i + 1);
                else if (maxX < inout.at<float>(i + 1))
                    maxX = inout.at<float>(i + 1);
            }
        }
        //        float diffY = (frontalFace.rows / 2 / REDUTOR) - ((maxY - minY) / 2) - minY;
        //        float diffX = (frontalFace.cols / 2 / REDUTOR) - ((maxX - minX) / 2) - minX;
        float diffY = - minY;
        float diffX = - minX;

        for (int j = 0; j < inout.rows; j++) {
            int i = static_cast< std::vector<uint16_t>::size_type >(j * 3);

            inout.at<float>(i + 0) += diffY;
            inout.at<float>(i + 1) += diffX;
        }
    }
}

//void tools::depthImgToPointCloud(const cv::Mat &depth, cv::Mat &pc, cv::Vec<bool, 3> scale, float greatthen, const cv::Vec3f &tvec)
void tools::depthImgToPointCloud(const cv::Mat &depth, cv::Mat &pc, const cv::Vec<bool,3> &scale, float greatthen, const cv::Vec3f &tvec)
{
    int points = 0;
    {//Total de pontos da malha de rosto
        for (int y = 0; y < depth.rows; y++) {
            for (int x = 0; x < depth.cols; x++) {
                if (depth.at<float>(y, x) < 0.1 /*|| imageCache.at<float>(y, x) > 0.99990*/)
                    continue;
                points ++;
            }
        }
    }

    pc = cv::Mat(points, 3, CV_32FC1, cv::Scalar(0));
    {//Cria image3D do rosto a partir da imagem de profundidade
        points = 0;
        std::vector<uint16_t>::size_type i;
        for (int y = 0; y < depth.rows; y++) {
            for (int x = 0; x < depth.cols; x++) {
                if (depth.at<float>(y, x) < greatthen /*|| imageCache.at<float>(y, x) > 0.99990*/)
                    continue;

                //i = static_cast< std::vector<uint16_t>::size_type >((y * imageCache.cols) + x);

                pc.at<float>(points, 0) = y + tvec[1];
                pc.at<float>(points, 1) = x + tvec[0];
                pc.at<float>(points, 2) = depth.at<float>(y, x) + tvec[2];

                if (scale[1])
                    pc.at<float>(points, 1) *= tools::SCALE_XY;
                if (scale[0])
                    pc.at<float>(points, 0) *= tools::SCALE_XY;
                if (scale[2])
                    pc.at<float>(points, 2) *= tools::SCALE_Z;
                points++;
            }
        }
    }

}

void tools::pointCloudToDepthImg(const cv::Mat &pc, cv::Mat &depth, const cv::Vec<bool,3> &scale, const cv::Vec3f &tvec)
{
    for (int j = 0; j < pc.rows; j++) {
        int i = static_cast< std::vector<uint16_t>::size_type >(j * 3);

        float x = pc.at<float>(i + 1)+ tvec[0];
        float y = pc.at<float>(i + 0)+ tvec[1];
        if (scale[0])
            x /= tools::SCALE_XY;
        if (scale[1])
            y /= tools::SCALE_XY;

        if (y < depth.rows && x < depth.cols && x  && x > 0 && y > 0) {
            depth.at<float>((int)y, (int)x) = pc.at<float>(i + 2) + tvec[2];
            if (scale[2])
                depth.at<float>((int)y, (int)x) /= SCALE_Z;
        }
    }
}

// Calculates rotation matrix given euler angles.
#define T_X 0
#define T_Y 1
#define T_Z 2
cv::Mat tools::eulerAnglesToRotationMatrix(const cv::Vec3d &theta)
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

cv::Matx44d tools::rotationMatrixTo44d(cv::Mat r)
{
//    return cv::Matx44d(r.at<double>(0,0), r.at<double>(1,0), r.at<double>(2,0), 0,
//                     r.at<double>(0,1), r.at<double>(1,1), r.at<double>(2,1), 0,
//                     r.at<double>(0,2), r.at<double>(1,2), r.at<double>(2,2), 0,
//                     0, 0, 0, 1);

    return cv::Matx44d(r.at<double>(0,0), r.at<double>(0,1), r.at<double>(0,2), 0,
                     r.at<double>(1,0), r.at<double>(1,1), r.at<double>(1,2), 0,
                     r.at<double>(2,0), r.at<double>(2,1), r.at<double>(2,2), 0,
                     0, 0, 0, 1);
}

void tools::saveImgProc(cv::Mat img, std::string permutation, int imgId, int permPos)
{
    return;
    cv::Mat rgb;
    if (img.channels() == 1)
        cv::cvtColor(img, rgb, cv::COLOR_GRAY2RGB);
    else
        rgb = img;

    if (img.type() == CV_32FC1) {
        cv::Mat tmp;
        rgb.convertTo(tmp, CV_8UC3, 255);
        rgb = tmp;
    }

    std::vector<int> qualityType;
    qualityType.push_back(cv::IMWRITE_JPEG_QUALITY);
    qualityType.push_back(95);

    cv::imwrite("../tmp/byimg_" + std::to_string(imgId) + "-" + std::to_string(permPos) + permutation + ".jpg", rgb, qualityType);
    cv::imwrite("../tmp/bystep_" + std::to_string(permPos) + "-" + std::to_string(imgId) + permutation + ".jpg", rgb, qualityType);
}
