#ifndef POSECORRECTION_HPP
#define POSECORRECTION_HPP

#include "../imageprocessor.hpp"

#include "../imageloader.hpp"
#include "../libs/afm.hpp"

class PoseCorrection : public ImageProcessor
{
    virtual cv::Mat proccess(const cv::Mat &image) override;
    virtual std::string name() override { return "PoseCorrection"; };

    AFM *afm;
public:
    PoseCorrection(ImageLoader *loader);

};

#endif // POSECORRECTION_HPP
