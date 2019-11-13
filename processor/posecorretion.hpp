#ifndef POSECORRECTION_HPP
#define POSECORRECTION_HPP

#include "../imageprocessor.hpp"

#include "../imageloader.hpp"
#include "../libs/afm.hpp"
#include "../libs/icp.hpp"
#include "../libs/ganpan.hpp"

class PoseCorrection : public ImageProcessor
{
    virtual cv::Mat proccess(const cv::Mat &image, int pos = 0, ImageLoader* imgLoader = nullptr) override;
    virtual std::string name() override { return "PoseCorrection"; };

    AFM *afm;
    ICP *icp;
    GanPan *ganpan;
public:
    PoseCorrection(ImageLoader *loader);

};

#endif // POSECORRECTION_HPP
