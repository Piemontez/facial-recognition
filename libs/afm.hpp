#ifndef AFM_HPP
#define AFM_HPP

#include "../imageprocessor.hpp"

#include "../imageloader.hpp"

namespace cv {
    class Mat;
}

/**
 * @brief The Average Face Model
 */
class AFM : public ImageProcessor
{
public:
    /**
     * @param loader: lista de imagens para criar rosto genérico
     */
    AFM(ImageLoader *loader);

    virtual cv::Mat proccess(const cv::Mat &image) override;
    virtual std::string name() override { return "AFM"; };
};

#endif // AFM_HPP
