#ifndef ROIPIXEL_H
#define ROIPIXEL_H

#include <QObject>

#include "abstractpixel.h"
#include "cdbpixel.h"

class RoiPixel : public AbstractPixel
{
public:
    RoiPixel(const std::vector<Eigen::Vector2d> corners,int depth): AbstractPixel(corners), mContent(0),mIncomplete(0),mMaxDepth(depth) {}
    RoiPixel(const std::vector<Eigen::Array2d> corners,int depth): AbstractPixel(corners), mContent(0),mIncomplete(0),mMaxDepth(depth) {}
    void addContent(double counts);
    void getContent(std::vector<CdbPixel*> pxList,int depth);
    double content();
    double incomplete();
    double mContent;
    double mIncomplete;
    int mMaxDepth;
};

#endif // ROIPIXEL_H
