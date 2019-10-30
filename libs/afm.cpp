#include "afm.hpp"

#include <opencv4/opencv2/opencv.hpp>


AFM::AFM(ImageLoader *loader)
{
    std::vector<cv::Mat> imgs = loader->images();
    std::vector<int> flags = loader->flags();

    std::vector<cv::Mat>::iterator imgIt = imgs.begin();
    std::vector<int>::iterator flagIt = flags.begin();

    std::cout << "AFM: Coletando imagens."  << std::endl;

    //Coleta as imagens definidas para treinamento
    std::vector<cv::Mat> imgsTrain;
    while(imgIt != imgs.end())
    {
        if ((*flagIt) & (RECOG_TRAIN | COMPARE_MAIN_TRAIN )) {
            imgsTrain.push_back(*imgIt);
        }

        ++imgIt;
        ++flagIt;
    }

    std::cout << "AFM: Criando face genérica."  << std::endl;
    //Realiza a média
    if (imgsTrain.size()) {
        cv::Mat temp;
        cv::Mat m(imgs[0].rows, imgs[0].cols, CV_64FC3);
        m.setTo(cv::Scalar(0,0,0,0));

        std::vector<cv::Mat>::iterator imgIt = imgsTrain.begin();
        while(imgIt != imgsTrain.end())
        {
            (*imgIt).convertTo(temp, CV_64FC3);
            m += temp;

            ++imgIt;
        }
        m.convertTo(m, CV_8U, 1. / imgsTrain.size());

        cv::imshow("processed", m);
        cv::waitKey();
    }
}

cv::Mat AFM::proccess(const cv::Mat &image)
{
    return image;
}
