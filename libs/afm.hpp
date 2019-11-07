#ifndef AFM_HPP
#define AFM_HPP

#include <opencv4/opencv2/core/mat.hpp>

#include "../imageprocessor.hpp"

#include "../imageloader.hpp"


/**
 * @brief The Average Face Model
 */
class AFM : public ImageProcessor
{
    cv::Mat afm;
public:
    /**
     * @param loader: lista de imagens para criar rosto genérico
     */
    AFM(ImageLoader *loader);

    virtual cv::Mat proccess(const cv::Mat &image) override;
    virtual std::string name() override { return "AFM"; };
};

#endif // AFM_HPP
