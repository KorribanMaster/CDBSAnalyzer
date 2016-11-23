#include "mpa2dhist.h"

Mpa2dHist::Mpa2dHist(QString name)
{
    setSize(0,0);
    mName = name;

}

Mpa2dHist::Mpa2dHist(QString name, int xbins, int ybins){
    setSize(xbins,ybins);
    mName = name;

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

void Mpa2dHist::findCenter(){
    //This is a temporary solution with a fixed centerpoint
    mCenter(0) = 512;
    mCenter(1) = 512;
}

void Mpa2dHist::updateRoi(){
    //Roi Border points. See drawing in lab report.
    Eigen::Vector2f p1;
    Eigen::Vector2f p2;
    Eigen::Vector2f c1;
    Eigen::Vector2f c2;
    Eigen::Vector2f corners[4];
    Eigen::Vector2f v1(1,-1);
    Eigen::Vector2f v2(1,1);
    Eigen::Vector2f v3(1,-1);
    v1 = v1*mRoiLength;
    v2 = v2*mRoiWidth;
    v3 = v3*mEnergyBinWidth;
    p1 = mEnergyCenter.matrix()-v1;
    p2 = mEnergyCenter.matrix()+v1;
    corners[0] = p1-v2;
    corners[1] = p1+v2;
    corners[2] = p2+v2;
    corners[3] = p2-v2;
    //mRoiBorder = new RoiPixel(corners);
    //RoiPixels. Assume the binWidth*n=roiLength
    //should be generalized
    c1 = corners[0];
    c2 = corners[1];
    for(int i=0;i<mRoiLength/mEnergyBinWidth-1;i++){
       corners[0] = c1+i*v3;
       corners[1] = c2+i*v3;
       corners[2] = c2+(i+1)*v3;
       corners[3] = c1+(i+1)*v3;
       //RoiPixel *px = new RoiPixel(corners);
       //mRoiGrid.push_back(px);
    }

}

void Mpa2dHist::updateMap(){
    mEnergyMap.clear();
    for(int i=0;i<mCenteredHist.cols();i++){
       for(int j=0;j<mCenteredHist.rows();j++){
           Eigen::Array2f center(i,j);
           Eigen::Array2f tmp;
           Eigen::Array2f odd;
           std::vector<Eigen::Array2f> corners;
           center = mEnergyCenter + (center-mCenter)*mCal;
           odd << -0.5,0.5;
           tmp = center-0.5*mCal;
           corners.push_back(tmp);
           tmp = center-odd*mCal;
           corners.push_back(tmp);
           tmp = center+0.5*mCal;
           corners.push_back(tmp);
           tmp = center+(-1*odd*mCal);
           corners.push_back(tmp);
           //CdbPixel *px = new CdbPixel(corners,mCenteredHist(i,j));
           //mEnergyMap.push_back(px);
       }
    }

}

Mpa1dHist* Mpa2dHist::projectCDBS(){
    findCenter();
    updateMap();
    updateRoi();
    Mpa1dHist *projection = new Mpa1dHist(mName);
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
