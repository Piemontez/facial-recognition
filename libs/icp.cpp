#include "icp.hpp"

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/surface_matching.hpp>
#include <opencv4/opencv2/surface_matching/ppf_helpers.hpp>

//https://docs.opencv.org/4.1.2/d9/d25/group__surface__matching.html

#define REDUTOR 149.0
#define MULTIPLICADOR 1.0

ICP::ICP(const cv::Mat &frontalFace)
{
    this->frontalFace = frontalFace.clone();

    icp = new cv::ppf_match_3d::ICP(1000, 0.000001f, 0.001f, 6);
    //icp = new cv::ppf_match_3d::ICP(100);
    //int64 t1 = cv::getTickCount();


    int x, y, pointsFrontal = 0;
    {//Total de pontos da malha 3D facial.
        for (int y = 0; y < frontalFace.rows; y++) {
            for (int x = 0; x < frontalFace.cols; x++) {
                if (frontalFace.at<float>(y, x) > 0.9990)
                    continue;
                pointsFrontal ++;
            }
        }
    }

    frontalFaceCloud = cv::Mat(pointsFrontal, 3, CV_32FC1, cv::Scalar(0));

    std::cout << "ICP: Criando modelo 3D do rosto genérico." << std::endl;
    pointsFrontal = 0;
    {//Cria image3D do rosto a partir da imagem de profundidade do rosto genérico
        for (int y = 0; y < frontalFace.rows; y++) {
            for (int x = 0; x < frontalFace.cols; x++) {
                if (frontalFace.at<float>(y, x) > 0.99990)
                    continue;

                frontalFaceCloud.at<float>(pointsFrontal, 0) = y / REDUTOR;
                frontalFaceCloud.at<float>(pointsFrontal, 1) = x / REDUTOR;
                frontalFaceCloud.at<float>(pointsFrontal, 2) = frontalFace.at<float>(y, x) * MULTIPLICADOR;

                pointsFrontal++;
            }
        }
    }
    std::cout << "ICP: Modelo 3D criado." << std::endl;

    std::cout << "ICP: Trainando detector de posição ." << std::endl;
    // Now train the model
    //int64 tick1 = cv::getTickCount();
    //detector = new cv::ppf_match_3d::PPF3DDetector(0.06, 0.08, 20);
    detector = new cv::ppf_match_3d::PPF3DDetector(0.06, 0.08, 25);
    detector->trainModel(frontalFaceCloud);

    //int64 tick2 = cv::getTickCount();
    std::cout << "ICP: Detector trainado." << std::endl;
}

cv::Mat ICP::proccess(const cv::Mat &imageCache, int pos, ImageLoader* imgLoader)
{
    assert(this->frontalFace.type() == CV_32F);
    assert(imageCache.type() == CV_32F);

    int points = 0;
    {//Total de pontos da malha de rosto
        for (int y = 0; y < imageCache.rows; y++) {
            for (int x = 0; x < imageCache.cols; x++) {
                if (imageCache.at<float>(y, x) > 0.99990 || !imageCache.at<float>(y, x))
                    continue;
                points ++;
            }
        }
    }

    cv::Mat imageCloud = cv::Mat(points, 3, CV_32FC1, cv::Scalar(0));
    {//Cria image3D do rosto a partir da imagem de profundidade
        points = 0;
        std::vector<uint16_t>::size_type i;
        for (int y = 0; y < imageCache.rows; y++) {
            for (int x = 0; x < imageCache.cols; x++) {
                if (imageCache.at<float>(y, x) > 0.99990 || !imageCache.at<float>(y, x))
                    continue;

                //i = static_cast< std::vector<uint16_t>::size_type >((y * imageCache.cols) + x);

                imageCloud.at<float>(points, 0) = y / REDUTOR;
                imageCloud.at<float>(points, 1) = x / REDUTOR;
                imageCloud.at<float>(points, 2) = imageCache.at<float>(y, x) * MULTIPLICADOR;

                points++;
            }
        }
    }

    // Match the model to the scene and get the pose
    std::vector<cv::ppf_match_3d::Pose3DPtr>  posesDetect;
    cv::Mat newpose;
    //tick1 = cv::getTickCount();
    detector->match(imageCloud, posesDetect, 1.0 / 5.0, 0.05);
    //tick2 = cv::getTickCount();


    std::cout << "------------" << std::endl;

    std::cout << posesDetect.size() << std::endl;

    // Register for all selected poses
//    icp->registerModelToScene(imageCloud, frontalFaceCloud, posesDetect);

//    std::cout << posesDetect.size() << std::endl;
//    std::cout << "------------" << std::endl;

    for (size_t k = 0; k < posesDetect.size(); k++)
    {
        auto result = posesDetect[k];
//        result->printPose();
//        result->pose(3,3) = 0.5;
//        result->printPose();

        newpose = cv::ppf_match_3d::transformPCPose(imageCloud, result->pose);

//        std::cout << std::endl;
//        std::cout << newpose.rows << std::endl << newpose.cols << std::endl;
//        std::cout << newpose.channels() << std::endl << newpose.type() << std::endl;
//        std::cout << "..........." << std::endl;

//        cv::Matx44d pose;
//        double residual = 0;
//        icp->registerModelToScene(imageCloud, frontalFaceCloud.clone(), residual, pose);
//        newpose = cv::ppf_match_3d::transformPCPose(imageCloud, pose);


        //newpose = cv::ppf_match_3d::transformPCPose(image, poses);

    //    std::cout << image.rows << std::endl << image.cols << std::endl;
    //    std::cout << image.channels() << std::endl << image.type() << std::endl << std::endl;

    //    std::cout << imageCache.rows << std::endl << imageCache.cols << std::endl;
    //    std::cout << imageCache.channels() << std::endl << imageCache.type() << std::endl << std::endl;

    //    std::cout << "..........." << std::endl;

        cv::Mat newposeFm(imageCache.rows, imageCache.cols, CV_32FC1, cv::Scalar(1));
        for (int j = 0; j < newpose.rows; j++) {
            int i = static_cast< std::vector<uint16_t>::size_type >(j * 3);

            float y = newpose.at<float>(i + 0) * REDUTOR;
            float x = newpose.at<float>(i + 1) * REDUTOR;

            //std::cout << x << " " << y  << " " << newpose.at<float>(i + 2) / MULTIPLICADOR<< " | " ;

            if (y < newposeFm.rows && x < newposeFm.cols && x  && x > 0 && y > 0)
                newposeFm.at<float>((int)y, (int)x) = newpose.at<float>(i + 2) / MULTIPLICADOR;
        }
        //std::cout << std::endl;

        cv::imshow("frontal", this->frontalFace);
        cv::imshow("teste", imageCache);
        //cv::imshow("processed", image);
        cv::imshow("new pose", newposeFm);
        cv::waitKey();
    }
    return imageCloud;
}
