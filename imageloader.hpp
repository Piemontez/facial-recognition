#ifndef IMAGELOADER_HPP
#define IMAGELOADER_HPP

#include "vector"
#include "string"

namespace cv {
    class Mat;
}

enum ImageLoaderFlags {
    FRONTAL = 1,
    LEFT    = 2,
    RITH    = 4,
    TOP     = 8,
    DOWN    = 16,
    RANDOM  = 32,

    RECOG_TRAIN   = 64,
    RECOG_TEST    = 128,
    COMPARE_MAIN_TRAIN  = 256,
    COMPARE_TRAIN       = 512, //Trained with COMPARE_MAIN_TRAIN
    COMPARE_TEST        = 1024,


    RBG   = 2048,
    IR    = 4096,
    DEPTH = 8192,
};

class ImageLoader
{
public:
    virtual std::vector<cv::Mat> images() = 0;
    virtual std::vector<int> labels() = 0;
    virtual std::vector<int> flags() = 0;
    virtual std::string name() = 0;
};

#endif // IMAGELOADER_HPP
