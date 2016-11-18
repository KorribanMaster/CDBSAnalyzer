#ifndef CDBPIXEL_H
#define CDBPIXEL_H

#include <QObject>

#include <abstractpixel.h>

class CdbPixel : public AbstractPixel
{
public:
    CdbPixel(Eigen::Vector2f corners[],double counts) : AbstractPixel(corners),mCounts(counts){}
    QList<CdbPixel*> split();
    double mCounts;
};

#endif // CDBPIXEL_H
