#ifndef ABSTRACTPIXEL_H
#define ABSTRACTPIXEL_H

#include <Eigen/Dense>

class AbstractPixel
{
public:
    AbstractPixel(const std::vector<Eigen::Vector2d> corners);
    AbstractPixel(const std::vector<Eigen::Array2d> corners);
    ~AbstractPixel();

    int inside(AbstractPixel* otherPixel);

    Eigen::Vector2d mCorners[4];
    Eigen::Vector2d mEdges[4];

};

#endif // ABSTRACTPIXEL_H
