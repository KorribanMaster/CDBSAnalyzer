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
    for(int i=0;i<hist.cols();i++){
       for(int j=0;j<hist.rows();j++){
           Eigen::Array2d center(i,j);
           Eigen::Array2d tmp;
           Eigen::Array2d odd;
           std::vector<Eigen::Array2d> corners;
           center = 511*1e3 + (center-10)*mCal;
           odd << -0.5,0.5;
           tmp = center-0.5*mCal;
           corners.push_back(tmp);
           tmp = center+odd*mCal;
           corners.push_back(tmp);
           tmp = center+0.5*mCal;
           corners.push_back(tmp);
           tmp = center-odd*mCal;
           corners.push_back(tmp);
           CdbPixel *px = new CdbPixel(corners,hist(i,j));
           map.push_back(px);
       }
    }
    std::vector<Eigen::Vector2d> corner(4);
    corner[0] = Eigen::Vector2d(511e3-5*mCal,511e3-5*mCal);
    corner[1] = Eigen::Vector2d(511e3-5*mCal,511e3+5*mCal);
    corner[2] = Eigen::Vector2d(511e3+5*mCal,511e3+5*mCal);
    corner[3] = Eigen::Vector2d(511e3+5*mCal,511e3-5*mCal);
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
