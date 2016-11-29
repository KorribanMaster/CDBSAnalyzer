#include "mpa2dhist.h"

Mpa2dHist::Mpa2dHist(QString name)
{
    setSize(0,0);
    mName = name;
    mMaxDepth = 8;
    mEnergyCenter = Eigen::Array2d(511,511);

}

Mpa2dHist::Mpa2dHist(QString name, int xbins, int ybins){
    setSize(xbins,ybins);
    mName = name;
    mMaxDepth =8;
    mEnergyCenter = Eigen::Array2d(511,511);


}

Mpa2dHist::~Mpa2dHist(){

}

void Mpa2dHist::setSize(int xbins, int ybins){
    mRawHist.resize(xbins,ybins);
    mXEnergyscale.resize(xbins);
    mYEnergyscale.resize(ybins);

}



void Mpa2dHist::setBinContent(int xbin, int ybin,int counts){
    mRawHist(xbin,ybin) = counts;
}

void Mpa2dHist::setRoi(double roiWidth, double roiLength){
    mRoiWidth = roiWidth;
    mRoiLength = roiLength;

}

void Mpa2dHist::setEnergyBinWidth(double binWidth){
    mEnergyBinWidth = binWidth;
}

void Mpa2dHist::setCalibration(float xcal,float ycal){
    mCal(0)=xcal;
    mCal(1)= ycal;
}

void Mpa2dHist::setCenter(float xcenter, float ycenter){
    //This is a temporary solution with a fixed centerpoint
    mCenter(0)=xcenter;
    mCenter(1)= ycenter;
}

void Mpa2dHist::findCenter(){
    //mCenteredHist = mRawHist.block<500,500>(mCenter(0)-250,mCenter(1)-250); //Commented for testing reasons
    mCenteredHist = mRawHist;
}

void Mpa2dHist::updateRoi(){
    //Roi Border points. See drawing in lab report.
    Eigen::Vector2d p1;
    Eigen::Vector2d p2;
    Eigen::Vector2d c1;
    Eigen::Vector2d c2;
    std::vector<Eigen::Vector2d> corners(4);
    Eigen::Vector2d v1(1/std::sqrt(2),-1/std::sqrt(2));
    Eigen::Vector2d v2(1/std::sqrt(2),1/std::sqrt(2));
    Eigen::Vector2d v3(1/std::sqrt(2),-1/std::sqrt(2));
    v1 = v1*mRoiLength;
    v2 = v2*mRoiWidth;
    v3 = v3*mEnergyBinWidth;
    p1 = mEnergyCenter.matrix()*1e3-v1;
    p2 = mEnergyCenter.matrix()*1e3+v1;
    corners[0] = p1-v2;
    corners[1] = p1+v2;
    corners[2] = p2+v2;
    corners[3] = p2-v2;
    mRoiBorder = new RoiPixel(corners);

    //RoiPixels. Assume the binWidth*n=roiLength
    //should be generalized
    c1 = corners[0];
    c2 = corners[1];
    for(int i=0;i<mRoiLength/mEnergyBinWidth*2;i++){
       corners[0] = c1+i*v3;
       corners[1] = c2+i*v3;
       corners[2] = c2+(i+1)*v3;
       corners[3] = c1+(i+1)*v3;
       RoiPixel *px = new RoiPixel(corners);
       mRoiGrid.push_back(px);
    }

}

void Mpa2dHist::updateMap(){
    mEnergyMap.clear();
    Eigen::Array2d leftTop;
    leftTop(0) = mEnergyCenter(0)*1e3-(mCenteredHist.rows()/2)*mCal(0);
    leftTop(1) = mEnergyCenter(1)*1e3+(mCenteredHist.cols()/2)*mCal(1);
    for(int y=0;y<mCenteredHist.cols();y++){
       for(int x=0;x<mCenteredHist.rows();x++){
           Eigen::Array2d center(x,y);
           Eigen::Array2d tmp;
           Eigen::Array2d odd;
           std::vector<Eigen::Array2d> corners;
           center(0) = leftTop(0)+x*mCal(0);
           center(1) = leftTop(1)-y*mCal(0);
           odd << -0.5,0.5;
           tmp = center-0.5*mCal;
           corners.push_back(tmp);
           tmp = center+odd*mCal;
           corners.push_back(tmp);
           tmp = center+0.5*mCal;
           corners.push_back(tmp);
           tmp = center-odd*mCal;
           corners.push_back(tmp);
           CdbPixel *px = new CdbPixel(corners,mCenteredHist(y,x));
           mEnergyMap.push_back(px);
       }
    }

}

Mpa1dHist* Mpa2dHist::projectCDBS(){
    findCenter();
    updateMap();
    updateRoi();
    Mpa1dHist *projection = new Mpa1dHist(mName);
    projection->setSize(mRoiGrid.size());
    std::vector<CdbPixel*> remaining;
    remaining.reserve(mEnergyMap.size());
    for(int i= 0;i<mEnergyMap.size();i++){
        CdbPixel *px = mEnergyMap[i];
        int nInside = px->inside(mRoiBorder);
        nInside += mRoiBorder->inside(px);
        if (nInside > 0){
            remaining.push_back(mEnergyMap[i]);
        }
    }
    mEnergyMapFiltered = remaining;
    for (int i=0;i<mRoiGrid.size();i++){
        mEnergyMapFiltered = mRoiGrid[i]->getContent(mEnergyMapFiltered,0);
        projection->setBinContent(i,mRoiGrid[i]->content());
    }
    return projection;
}



Mpa1dHist* Mpa2dHist::projectCDBS(double roiWidth, double roiLength, double binWidth){
    setRoi(roiWidth,roiLength);
    setEnergyBinWidth(binWidth);
    findCenter();
    updateMap();
    updateRoi();
    Mpa1dHist *projection = new Mpa1dHist(mName);
    return projection;

}
