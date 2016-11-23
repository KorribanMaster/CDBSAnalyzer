#ifndef ROIPIXEL_H
#define ROIPIXEL_H

#include <QObject>

#include <abstractpixel.h>

class RoiPixel : public AbstractPixel
{
public:
    RoiPixel(const Eigen::Vector2f corners[]): AbstractPixel(corners), mContent(0) {}
    RoiPixel(const std::vector<Eigen::Array2f> corners): AbstractPixel(corners), mContent(0) {}
    int mContent;
};

#endif // ROIPIXEL_H
