#ifndef ICP_HPP
#define ICP_HPP

#include "../imageprocessor.hpp"

namespace cv {
    class Mat;
}

/**
 * @brief The Iterative closest point
 */
class ICP : public ImageProcessor
{
public:
    ICP(cv::Mat *frontalFace);

    virtual cv::Mat proccess(const cv::Mat &image) override;
    virtual std::string name() override { return "ICP"; };
};

#endif // ICP_HPP
