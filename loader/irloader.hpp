#ifndef IRLOADER_HPP
#define IRLOADER_HPP

#include "../imageloader.hpp"
#include "json/value.h"

class IrLoader : public ImageLoader
{
    Json::Value root;
    std::vector<std::string> _files;
    std::vector<int> _labels;
    std::vector<int> _flags;
public:
    IrLoader();

    virtual std::vector<cv::Mat> images() override;
    virtual std::vector<int> labels() override;
    virtual std::vector<int> flags() override;
    virtual std::string name() override;
};

#endif // IRLOADER_HPP
