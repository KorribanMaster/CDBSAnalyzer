#include "abstractpixel.h"
#include "cdbpixel.h"
#include "roipixel.h"

#include <QDebug>
#include <Eigen/Dense>

int main(int argc, char *argv[])
{
    std::vector<Eigen::Vector2d> corner(4);
    corner[0] = Eigen::Vector2d(0,0);
    corner[1] = Eigen::Vector2d(0,1);
    corner[2] = Eigen::Vector2d(1,1);
    corner[3] = Eigen::Vector2d(1,0);
    RoiPixel *gridPx = new RoiPixel(corner);
    CdbPixel *splitPx = new CdbPixel(corner,100);

    corner[0] = Eigen::Vector2d(0.5,0.5);
    corner[1] = Eigen::Vector2d(0.5,0.75);
    corner[2] = Eigen::Vector2d(0.75,0.75);
    corner[3] = Eigen::Vector2d(0.75,0.5);
    CdbPixel *insidePx = new CdbPixel(corner,100);

    corner[0] = Eigen::Vector2d(-0.5,-0.5);
    corner[1] = Eigen::Vector2d(-0.5,0.5);
    corner[2] = Eigen::Vector2d(0.5,0.5);
    corner[3] = Eigen::Vector2d(0.5,-0.5);
    AbstractPixel *partialPx = new AbstractPixel(corner);

    corner[0] = Eigen::Vector2d(-0.5,-0.5);
    corner[1] = Eigen::Vector2d(-1.5,0.5);
    corner[2] = Eigen::Vector2d(1.5,3.5);
    corner[3] = Eigen::Vector2d(0.5,2.5);
    AbstractPixel *rotPx = new AbstractPixel(corner);

    qDebug() << "This should be zero\n";
    qDebug() << gridPx->inside(insidePx);
    qDebug() << "This should be four\n";
    qDebug() << insidePx->inside(gridPx);
    qDebug() << gridPx->inside(gridPx);
    qDebug() << "This should be one";
    qDebug() << partialPx->inside(gridPx);
    qDebug() << "This should be one";
    qDebug() << gridPx->inside(partialPx);
    qDebug() << partialPx->inside(rotPx);

    std::vector<CdbPixel*> subPixels = splitPx->split();
    for(int i = 0; i<4;i++){
        qDebug() << "The Corners of sub pixel" <<i<< " are:";
        CdbPixel *subPx = subPixels.at(i);
        for (int j=0;j<4;j++){
            qDebug() << subPx->mCorners[j](0) << ", " << subPx->mCorners[j](1);
        }
        qDebug() << "Sub pixel counts:";
        qDebug() << subPx->counts();
    }


    return 1;
}
