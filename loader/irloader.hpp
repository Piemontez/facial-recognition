#ifndef IRLOADER_HPP
#define IRLOADER_HPP

#include "../imageloader.hpp"
#include "json/value.h"

class IrLoader : public ImageLoader
{
    Json::Value root;
    std::vector<std::string> _files;
    std::vector<std::string> _labels;
public:
    IrLoader();

    virtual std::vector<cv::Mat> images() override;
    virtual std::vector<int> labels() override;
    virtual std::vector<int> flags() override;
};

#endif // IRLOADER_HPP
