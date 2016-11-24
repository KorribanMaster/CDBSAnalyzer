#ifndef ROIPIXEL_H
#define ROIPIXEL_H

#include <QObject>

#include "abstractpixel.h"

class RoiPixel : public AbstractPixel
{
public:
    RoiPixel(const std::vector<Eigen::Vector2d> corners): AbstractPixel(corners), mContent(0) {}
    RoiPixel(const std::vector<Eigen::Array2d> corners): AbstractPixel(corners), mContent(0) {}
    void addContent(double counts);
    double content();
    int mContent;
};

#endif // ROIPIXEL_H
