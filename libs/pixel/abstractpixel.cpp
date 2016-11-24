#include "abstractpixel.h"

AbstractPixel::AbstractPixel(const std::vector<Eigen::Vector2d> corners)
{
    for (int i=0; i<4; i++){
        mCorners[i] = corners[i];
    }
    //mEdges = new Eigen::Vector2d[4];
    for (int i = 0; i<3 ;i++){
        mEdges[i] =  mCorners[i+1]-mCorners[i];//p2-p1
    }
    mEdges[3] = mCorners[0] - mCorners[3];

}

AbstractPixel::AbstractPixel(const std::vector<Eigen::Array2d> corners){
    for (int i=0; i<4; i++){
        mCorners[i](0) = corners[i](0);
        mCorners[i](1) = corners[i](1);
    }
    for (int i = 0; i<3 ;i++){
        mEdges[i] =  mCorners[i+1]-mCorners[i];//p2-p1
    }
    mEdges[3] = mCorners[0] - mCorners[3];
}

AbstractPixel::~AbstractPixel(){

}

int AbstractPixel::inside(AbstractPixel *otherPixel){
    int cornersInside = 0;
    float test[4];
    for (int i =0; i<4; i++){
        Eigen::Vector2d pt = mCorners[i];
        for (int j = 0; j<4; j++){
            int next = j+1;
            if (next ==4) next =0;
            Eigen::Vector2d e = otherPixel->mEdges[j];
            Eigen::Vector2d p1 = otherPixel->mCorners[j];
            Eigen::Vector2d p2 = otherPixel->mCorners[next];
            test[j] = e(0)*(pt(1)-p2(1)) -(pt(0)-p1(0))*e(1);
            /* If test > 0, the point is on the left-hand side.
               If test < 0, the point is on the right-hand side.
               If test = 0, the point is on the line.
            */
        }
        //If all tests are negative or zero point is inside
        if(test[0] <= 0 && test[1] <= 0 && test[2] <=0 && test[3] <= 0) cornersInside++;
    }
    return cornersInside;
}
