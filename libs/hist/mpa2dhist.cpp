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

}

void Mpa2dHist::updateRoi(){

}

void Mpa2dHist::updateMap(){

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
