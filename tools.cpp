#include "tools.hpp"

#include <fstream>
#include <opencv4/opencv2/opencv.hpp>

void tools::moveToRigth(cv::Mat &inout)
{
    //if (inout.channels() == 3)
    {
        float minY = inout.at<float>(0);
        float minX = inout.at<float>(1);
//        float maxY = inout.at<float>(0);
//        float maxX = inout.at<float>(1);
        {
            for (int j = 0; j < inout.rows; j++) {
                int i = static_cast< std::vector<uint16_t>::size_type >(j * inout.cols);
                if (minY > inout.at<float>(i + 0))
                    minY = inout.at<float>(i + 0);
//                else if (maxY < inout.at<float>(i + 0))
//                    maxY = inout.at<float>(i + 0);

                if (minX > inout.at<float>(i + 1))
                    minX = inout.at<float>(i + 1);
//                else if (maxX < inout.at<float>(i + 1))
//                    maxX = inout.at<float>(i + 1);
            }
        }
        //        float diffY = (frontalFace.rows / 2 / REDUTOR) - ((maxY - minY) / 2) - minY;
        //        float diffX = (frontalFace.cols / 2 / REDUTOR) - ((maxX - minX) / 2) - minX;
        float diffY = - minY;
        float diffX = - minX;

        for (int j = 0; j < inout.rows; j++) {
            int i = static_cast< std::vector<uint16_t>::size_type >(j * inout.cols);

            inout.at<float>(i + 0) += diffY;
            inout.at<float>(i + 1) += diffX;
        }
    }
}

void tools::moveToCenter(cv::Mat &inout, const cv::Vec<bool,2> &scaled, const cv::Vec2f &tvec)
{
    //if (inout.channels() == 3)
    {
        float minY = inout.at<float>(0);
        float minX = inout.at<float>(1);
        float maxY = inout.at<float>(0);
        float maxX = inout.at<float>(1);
        {
            for (int j = 0; j < inout.rows; j++) {
                int i = static_cast< std::vector<uint16_t>::size_type >(j * inout.cols);
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
            int i = static_cast< std::vector<uint16_t>::size_type >(j * inout.cols);

            inout.at<float>(i + 0) += diffY + (scaled[0] ? (tvec[0] / 2 * tools::SCALE_XY) : tvec[0]) - ((maxY - minY) / 2);
            inout.at<float>(i + 1) += diffX + (scaled[1] ? (tvec[1] / 2 * tools::SCALE_XY) : tvec[1]) - ((maxX - minX) / 2);
        }
    }
}

//void tools::depthImgToPointCloud(const cv::Mat &depth, cv::Mat &pc, cv::Vec<bool, 3> scale, float greatthen, const cv::Vec3f &tvec)
void tools::depthImgToPointCloud(const cv::Mat &depth, const cv::Mat &color, cv::Mat &pc, const cv::Vec<bool,3> &scale, float greatthen, const cv::Vec3f &tvec)
{
    if (color.channels() == 3) {
        assert(depth.rows == color.rows);
        assert(depth.cols == color.cols);
    }

    int points = 0;
    {//Total de pontos da malha de rosto
        for (int y = 0; y < depth.rows; y++) {
            for (int x = 0; x < depth.cols; x++) {
                if (depth.at<float>(y, x) < greatthen /*|| imageCache.at<float>(y, x) > 0.99990*/)
                    continue;
                points ++;
            }
        }
    }

    if (color.channels() == 3)
        pc = cv::Mat(points, 6, CV_32FC1, cv::Scalar(0));
    else
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

                if (color.channels() == 3) {
                    int i = static_cast< std::vector<uint16_t>::size_type >(y * 3) + x;

                    pc.at<float>(points, 3) = color.at<cv::Point3f>(y, x).x;
                    pc.at<float>(points, 4) = color.at<cv::Point3f>(y, x).y;
                    pc.at<float>(points, 5) = color.at<cv::Point3f>(y, x).z;
                }

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
        int i = static_cast< std::vector<uint16_t>::size_type >(j * pc.cols);

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

void tools::pointCloudToRGBImg(const cv::Mat &pc, cv::Mat &rgb, const cv::Vec<bool,2> &scale, const cv::Vec3f &tvec)
{
    assert(pc.cols == 6);

    for (int j = 0; j < pc.rows; j++) {
        int i = static_cast< std::vector<uint16_t>::size_type >(j * pc.cols);

        float x = pc.at<float>(i + 1)+ tvec[0];
        float y = pc.at<float>(i + 0)+ tvec[1];
        if (scale[0])
            x /= tools::SCALE_XY;
        if (scale[1])
            y /= tools::SCALE_XY;

        if (y < rgb.rows && x < rgb.cols && x  && x > 0 && y > 0) {
            rgb.at<cv::Point3f>((int)y, (int)x).x = pc.at<float>(i + 3);
            rgb.at<cv::Point3f>((int)y, (int)x).y = pc.at<float>(i + 4);
            rgb.at<cv::Point3f>((int)y, (int)x).z = pc.at<float>(i + 5);
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

cv::Mat tools::loadImgProc(std::string permutation, int imgId, int permPos)
{
    std::string path = "../tmp/byimg_" + std::to_string(imgId) + "-" + std::to_string(permPos) + permutation + ".jpg";
    std::cout << "load" << path << std::endl;

    return cv::imread(path);
}

void tools::saveImgProc(cv::Mat img, std::string permutation, int imgId, int permPos, bool blackToWhite)
{
    cv::Mat rgb;
    if (img.channels() == 1)
        cv::cvtColor(img, rgb, cv::COLOR_GRAY2RGB);
    else
        rgb = img.clone();

    if (img.type() == CV_32FC1 || img.type() == CV_32FC3) {
        cv::Mat tmp;
        rgb.convertTo(tmp, CV_8UC3, 255);
        rgb = tmp;
    }

    if (blackToWhite) {
        std::vector<cv::Mat> splited_frame;
        //Read your frame
        cv::split(rgb, splited_frame);

        for (int y = 0; y < rgb.rows; y++) {
            for (int x = 0; x < rgb.cols; x++) {
                if (!splited_frame[0].at<char>(y,x) && !splited_frame[1].at<char>(y,x) && !splited_frame[2].at<char>(y,x))
                {
                    splited_frame[0].at<char>(y,x) = static_cast<char>(255);
                    splited_frame[1].at<char>(y,x) = static_cast<char>(255);
                    splited_frame[2].at<char>(y,x) = static_cast<char>(255);
                }
            }
        }

        cv::merge(splited_frame, rgb);
    }

    std::vector<int> qualityType;
    qualityType.push_back(cv::IMWRITE_JPEG_QUALITY);
    qualityType.push_back(95);

    cv::imwrite("../tmp/byimg_" + std::to_string(imgId) + "-" + std::to_string(permPos) + permutation + ".jpg", rgb, qualityType);
    cv::imwrite("../tmp/bystep_" + std::to_string(permPos) + "-" + std::to_string(imgId) + permutation + ".jpg", rgb, qualityType);
}

void tools::appendCsv(std::string filename, std::vector<std::string> cols)
{

    std::ofstream outfile;

    outfile.open("../tmp/results.csv", std::ios_base::app); // append instead of overwrite
    for(std::string col: cols)
        outfile << col << ";";
    outfile << std::endl;
}

cv::Mat tools::transformPCPose(cv::Mat pc, const cv::Matx44d &Pose)
{
    cv::Mat pct = cv::Mat(pc.rows, pc.cols, CV_32F, cv::Scalar(0));

    cv::Matx33d R;
    cv::Vec3d t;

    cv::Mat(Pose).rowRange(0, 3).colRange(0, 3).copyTo(R);
    cv::Mat(Pose).rowRange(0, 3).colRange(3, 4).copyTo(t);

    for (int i=0; i<pc.rows; i++)
    {
      const float *pcData = pc.ptr<float>(i);
      const cv::Vec3f n1(&pcData[3]);

      cv::Vec4d p = Pose * cv::Vec4d(pcData[0], pcData[1], pcData[2], 1);
      cv::Vec3d p2(p.val);

      // p2[3] should normally be 1
//      if (fabs(p[3]) > cv::TermCriteria::EPS)
      //if (fabs(p[3]) > 0)
      {
        cv::Mat((1.0 / p[3]) * p2).reshape(1, 1).convertTo(pct.row(i).colRange(0, 3), CV_32F);
      }
      if (pc.cols > 3)
        pc.row(i).colRange(3,6).copyTo(pct.row(i).colRange(3, 6));
    }
    return pct;
}
