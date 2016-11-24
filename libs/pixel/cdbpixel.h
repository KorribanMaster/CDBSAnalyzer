#ifndef CDBPIXEL_H
#define CDBPIXEL_H

#include <QObject>

#include "abstractpixel.h"

class CdbPixel : public AbstractPixel
{
public:
    CdbPixel(const std::vector<Eigen::Vector2d> corners,double counts) : AbstractPixel(corners),mCounts(counts){}
    CdbPixel(const std::vector<Eigen::Array2d> corners,double counts) : AbstractPixel(corners),mCounts(counts){}
    std::vector<CdbPixel*> split();
    double counts();
    double mCounts;
};

#endif // CDBPIXEL_H
