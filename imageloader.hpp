#ifndef IMAGELOADER_HPP
#define IMAGELOADER_HPP

class ImageLoader
{
public:
    virtual ~ImageLoader() = 0;

    virtual void loadTrainingImages() = 0;
    virtual void loadTestImages() = 0;
};

#endif // IMAGELOADER_HPP
