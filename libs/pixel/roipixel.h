#ifndef ROIPIXEL_H
#define ROIPIXEL_H

#include <QObject>

#include <abstractpixel.h>

class RoiPixel : public AbstractPixel
{
public:
    RoiPixel(Eigen::Vector2f corners[]): AbstractPixel(corners), mContent(0) {}
    int mContent;
};

#endif // ROIPIXEL_H
