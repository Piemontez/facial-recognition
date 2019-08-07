#ifndef IMAGELOADER_HPP
#define IMAGELOADER_HPP

#include "vector";

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


    RBG   = 1024,
    IR    = 2048,
    DEPTH = 4096,
};

class ImageLoader
{
public:
    virtual std::vector<cv::Mat> images() = 0;
    virtual std::vector<int> labels() = 0;
    virtual std::vector<int> flags() = 0;
};

#endif // IMAGELOADER_HPP
