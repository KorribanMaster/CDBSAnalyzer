#include "abstractpixel.h"

AbstractPixel::AbstractPixel(Eigen::Vector2f corners[])
{
    for (int i=0; i<4; i++){
        mCorners[i] = corners[i];
    }
    //mEdges = new Eigen::Vector2f[4];
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
        Eigen::Vector2f pt = mCorners[i];
        for (int j = 0; j<4; j++){
            int next = j+1;
            if (next ==4) next =0;
            Eigen::Vector2f e = otherPixel->mEdges[j];
            Eigen::Vector2f p1 = otherPixel->mCorners[j];
            Eigen::Vector2f p2 = otherPixel->mCorners[next];
            test[j] = e(1)*pt(0) + e(0)*pt(1) -(-e(1)*p2(0)+e(0)*p1(1));
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
