#include "abstractpixel.h"
#include "cdbpixel.h"
#include "roipixel.h"

#include <QDebug>
#include <Eigen/Dense>

int main(int argc, char *argv[])
{
    //Construct a small map
    Eigen::MatrixXf hist(20,20);
    std::vector<CdbPixel*> map;
    double mCal = 60;
    Eigen::Array2d leftTop;
    leftTop(0) = 511e3-10*mCal;
    leftTop(1) = 511e3+10*mCal;
    for(int y=0;y<hist.cols();y++){
       for(int x=0;x<hist.rows();x++){
           Eigen::Array2d center;
           Eigen::Array2d tmp;
           Eigen::Array2d odd;
           std::vector<Eigen::Array2d> corners;
           center(0) = leftTop(0)+x*mCal;
           center(1) = leftTop(1)-y*mCal;
           //center = 511*1e3 + (center+Eigen::Array2d(-10,10))*mCal;
           odd << -0.5,0.5;
           tmp = center-0.5*mCal;
           corners.push_back(tmp);
           tmp = center+odd*mCal;
           corners.push_back(tmp);
           tmp = center+0.5*mCal;
           corners.push_back(tmp);
           tmp = center-odd*mCal;
           corners.push_back(tmp);
           CdbPixel *px = new CdbPixel(corners,hist(y,x));
           map.push_back(px);
       }
    }
    std::vector<Eigen::Vector2d> corner(4);
    corner[0] = Eigen::Vector2d(511e3-5*mCal,511e3+4*mCal);
    corner[1] = Eigen::Vector2d(511e3-4*mCal,511e3+5*mCal);
    corner[2] = Eigen::Vector2d(511e3+5*mCal,511e3-4*mCal);
    corner[3] = Eigen::Vector2d(511e3+4*mCal,511e3-5*mCal);
    RoiPixel *gridPx = new RoiPixel(corner);
    int counter=0;
    for(int i = 0;i<map.size();i++){
        int inside = map[i]->inside(gridPx);
        //qDebug() <<inside;
        if(inside>0){
            counter++;
        }
    }
    qDebug() << counter;
    return 1;
}
