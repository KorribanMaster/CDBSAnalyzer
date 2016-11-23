#ifndef ABSTRACTPIXEL_H
#define ABSTRACTPIXEL_H

#include <Eigen/Dense>

class AbstractPixel
{
public:
    AbstractPixel(const Eigen::Vector2f corners[]);
    AbstractPixel(const std::vector<Eigen::Array2f> corners);
    ~AbstractPixel();

    int inside(AbstractPixel* otherPixel);

    Eigen::Vector2f mCorners[4];
    Eigen::Vector2f mEdges[4];

};

#endif // ABSTRACTPIXEL_H
