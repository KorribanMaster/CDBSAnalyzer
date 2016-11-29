#ifndef ROIPIXEL_H
#define ROIPIXEL_H

#include <QObject>

#include "abstractpixel.h"
#include "cdbpixel.h"

class RoiPixel : public AbstractPixel
{
public:
    RoiPixel(const std::vector<Eigen::Vector2d> corners): AbstractPixel(corners), mContent(0),mMaxDepth(11) {}
    RoiPixel(const std::vector<Eigen::Array2d> corners): AbstractPixel(corners), mContent(0),mMaxDepth(11) {}
    void addContent(double counts);
    std::vector<CdbPixel*> getContent(std::vector<CdbPixel*> pxList,int depth);
    double content();
    double mContent;
    int mMaxDepth;
};

#endif // ROIPIXEL_H
