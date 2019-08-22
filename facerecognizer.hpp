#ifndef FACERECOGNIZER_HPP
#define FACERECOGNIZER_HPP

#include <string>
#include <vector>

namespace cv {
    class Mat;
}

/*!
 * \brief The FaceRecognizer class
 */
class Recognizer {
public:

    /*!
     * \brief algorithmName retorna o nome do algorítmo. Utilizado na localização da técnica pelo algorithmFactory
     * \return
     */
    virtual std::string algorithmName() = 0;

    /*!
     * \brief train
     * \param images
     * \param labels
     */
    virtual void train(const std::vector<cv::Mat> &images, const std::vector<int> &labels) = 0;

    /**
     * @brief resetTrain
     */
    virtual void resetTrain() = 0;

    /*!
     * \brief predict
     * \param image
     * \param label
     * \param confidence
     */
    virtual void predict(const cv::Mat &image, int &label, int &confidence) = 0;

    /*!
     * \brief predict
     * \param image
     * \return
     */
    virtual int predict(const cv::Mat &image) = 0;

    /*!
     * \brief compare
     * \param imageA
     * \param imageB
     * \return
     */
    virtual int compare(const cv::Mat &imageA, const cv::Mat &imageB) = 0;

    /*!
     * \brief save
     * \param filePath, endereço e nome relativo
     */
    virtual void save(const std::string &filePath) = 0;

    /*!
     * \brief load
     * \param filePath, endereço e nome relativo
     */
    virtual void load(const std::string &filePath) = 0;
};

#endif // FACERECOGNIZER_HPP
