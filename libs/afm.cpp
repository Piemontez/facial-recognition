#include "afm.hpp"

#include "../processor/roi.hpp"
#include <opencv4/opencv2/opencv.hpp>


AFM::AFM(ImageLoader *loader)
{
    ROI* roi = new ROI();

    std::vector<cv::Mat> imgs = loader->images();
    std::vector<int> flags = loader->flags();

    std::vector<cv::Mat>::iterator imgIt = imgs.begin();
    std::vector<int>::iterator flagIt = flags.begin();

    std::cout << "AFM: Coletando imagens."  << std::endl;

    //Coleta as imagens definidas para treinamento
    int pos = 0;
    std::vector<cv::Mat> imgsTrain;
    while(imgIt != imgs.end())
    {
        if ((*flagIt) & (RECOG_TRAIN | COMPARE_MAIN_TRAIN )) {
            imgsTrain.push_back(roi->proccess(*imgIt, pos, loader));
        }

        ++imgIt;
        ++flagIt;
        ++pos;
    }

    std::cout << "AFM: Criando face genérica." << std::endl;
    std::cout << "AFM: Total de imagens: " << imgsTrain.size() << std::endl;
    //Realiza a média
    if (imgsTrain.size()) {
        cv::Mat temp;
        cv::Mat m(imgs[0].rows, imgs[0].cols, CV_64FC1);
        m.setTo(cv::Scalar(0,0,0,0));

        std::vector<cv::Mat>::iterator imgIt = imgsTrain.begin();
        while(imgIt != imgsTrain.end())
        {
            (*imgIt).convertTo(temp, CV_64FC1);

            for(int i=0; i<temp.rows; i++)
                for(int j=0; j<temp.cols; j++)
                    if (temp.at<double>(i,j) < 0.1)
                        temp.at<double>(i,j) = 0.1;
            m += temp;

            ++imgIt;
        }
        m.convertTo(afm, CV_32FC1, 1. / imgsTrain.size());

        for(int i=0; i<temp.rows; i++)
            for(int j=0; j<temp.cols; j++)
                if (afm.at<float>(i,j) <= 0.17)
                    afm.at<float>(i,j) = 0;

    }
    std::cout << "AFM: Face genérica criada." << std::endl;

    cv::imshow("afm",afm);
    cv::moveWindow("afm",800,0);
    //cv::waitKey();

    delete roi;
}

cv::Mat AFM::proccess(const cv::Mat &image, int pos, ImageLoader* imgLoader)
{
    return afm;
}
