#ifndef THREELOADER_HPP
#define THREELOADER_HPP

#include "../imageloader.hpp"
#include "json/value.h"

class ThreeLoader : public ImageLoader
{
    Json::Value root;
    std::vector<std::string> _files;
    std::vector<std::string> _filesRBG;
    std::vector<int> _labels;
    std::vector<int> _flags;
public:
    ThreeLoader();

    virtual std::vector<cv::Mat> images() override;
    std::vector<cv::Mat> imagesRGB();
    virtual std::vector<int> labels() override;
    virtual std::vector<int> flags() override;
};

#endif // THREELOADER_HPP
