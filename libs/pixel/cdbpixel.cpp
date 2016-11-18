#include "cdbpixel.h"

QList<CdbPixel*> CdbPixel::split(){
    Eigen::Vector2f corners[4];
    QList<CdbPixel> subPixels;
    corners[0] = mCorners[0];
    corners[1] = mCorners[0]+0.5*mEdges[0];
    corners[2] = mCorners[0]+0.5*mEdges[0]+0.5*mEdges[1];
    corners[3] = mCorners[0]+0.5*mEdges[1];
    CdbPixel subPixel = new CdbPixel(corners,mCounts/4);
    subPixels <<subPixel;
    corners[0] = mCorners[1]-0.5*mEdges[0];
    corners[1] = mCorners[1];
    corners[2] = mCorners[1]+0.5*mEdges[1];
    corners[3] = mCorners[1]+0.5*mEdges[1]-0.5*mEdges[0];
    subPixel = new CdbPixel(corners,mCounts/4);
    subPixels <<subPixel;
    corners[0] = mCorners[2] -0.5*mEdges[1]+0.5*mEdges[2];
    corners[1] = mCorners[2] -0.5*mEdges[1];
    corners[2] = mCorners[2];
    corners[3] = mCorners[2]+0.5*mEdges[2];
    subPixel = new CdbPixel(corners,mCounts/4);
    subPixels <<subPixel;
    corners[0] = mCorners[3] +0.5*mEdges[3];
    corners[1] = mCorners[3] +0.5*mEdges[3]-0.5*mEdges[2];
    corners[2] = mCorners[3] -0.5*mEdges[2];
    corners[3] = mCorners[3];
    subPixel = new CdbPixel(corners,mCounts/4);
    subPixels <<subPixel;
    return;

}
