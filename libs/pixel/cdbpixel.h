#ifndef CDBPIXEL_H
#define CDBPIXEL_H

#include <QObject>

#include <abstractpixel.h>

class CdbPixel : public AbstractPixel
{
public:
    CdbPixel(const Eigen::Vector2f corners[],double counts) : AbstractPixel(corners),mCounts(counts){}
    CdbPixel(const std::vector<Eigen::Array2f> corners,double counts) : AbstractPixel(corners),mCounts(counts){}
    std::vector<CdbPixel*> split();
    double mCounts;
};

#endif // CDBPIXEL_H
