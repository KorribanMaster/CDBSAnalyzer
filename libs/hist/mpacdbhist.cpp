#include "mpacdbhist.h"

#include <Eigen/Core>

MpaCdbHist::MpaCdbHist(QString name ,QObject *parent): QObject(parent)
{
    setSize(0);
    mName = name;
}

MpaCdbHist::MpaCdbHist(QString name,int size,QObject *parent): QObject(parent)
{
    mName = name;
    setSize(size);
}

MpaCdbHist::~MpaCdbHist(){

}

void MpaCdbHist::setSize(int size){
    mSize = size;
    mProjectionHist.resize(mSize);
    mNormHist.resize(mSize);
    mFoldoverHist.resize(mSize/2);
    mNormFoldoverHist.resize(mSize/2);
    mEnergyScale.resize(mSize);
}

void MpaCdbHist::setBinContent(int nBin,double counts){
    mProjectionHist(nBin) = counts;
}

void MpaCdbHist::setCalibration(float channelToEnergy, float peakEnergy, float peakPosition){
    mEnergyScale.setLinSpaced(mSize,peakEnergy-peakPosition*channelToEnergy,peakEnergy+peakPosition*channelToEnergy);
}

void MpaCdbHist::setRoiInformation(double roiWidth, double roiLength, double binWidth){
    mRoiWidth = roiWidth;
    mRoiLength = roiLength;
    mEnergyBinWidth = binWidth;
}

void MpaCdbHist::calculateNorm(){
    mNorm = 0;
    for(int i=0;i<mSize;i++){
        mNorm += mProjectionHist(i);
    }
    for(int i=0;i<mSize;i++){
        mNormHist(i) = mProjectionHist(i)/mNorm;
    }
}

void MpaCdbHist::calculateFoldover(){
    calculateNorm();
    if(mSize%2 == 1){
        qDebug() << "Odd size is not supposed to happen";

    }
    else{
        for(int i=0;i<mSize/2;i++){
            mFoldoverHist(i) = mProjectionHist(mSize/2+i) + mProjectionHist(mSize/2-1-i);
            mNormFoldoverHist(i) = mNormHist(mSize/2+i) + mNormHist(mSize/2-1-i);
        }
    }
}

//MpaCdbHist operator+(MpaCdbHist const& lhs,MpaCdbHist const& rhs){
//    if(lhs.mSize != rhs.mSize){
//        qDebug()<< "Error: Hist sizes do not match";
//        throw std::invalid_argument("Histogramms need to have the same size");
//    }
//    if(lhs.mRoiLength != rhs.mRoiLength) qDebug()<< "Warning: Different roi length";
//    if(lhs.mRoiWidth != rhs.mRoiWidth) qDebug()<< "Warning: Different roi width";
//    if(lhs.mEnergyBinWidth != rhs.mEnergyBinWidth) qDebug()<< "Warning: Different bin width";
//    QString histName = lhs.mName+"+"+rhs.mName;
//    MpaCdbHist hist(histName);
//    hist.setSize(std::max(lhs.mSize,rhs.mSize));
//    hist.setRoiInformation(lhs.mRoiWidth,lhs.mRoiLength,lhs.mEnergyBinWidth);
//    hist.mEnergyScale = lhs.mEnergyScale;
//    for(int i=0;i<hist.mSize;i++)
//    {
//        double bin = lhs.mProjectionHist(i)+rhs.mProjectionHist(i);
//        hist.setBinContent(i,bin);
//    }
//    hist.calculateFoldover();
//    return hist;
//}

//MpaRefHist* MpaCdbHist::operator /(MpaCdbHist const& lhs,MpaCdbHist const& rhs){
//    if(lhs_>mSize != rhs->mSize){
//        qDebug()<< "Error: Hist sizes do not match";
//        throw std::invalid_argument("Histogramms need to have the same size");
//    }
//    if(lhs->mRoiLength != rhs->mRoiLength) qDebug()<< "Warning: Different roi length";
//    if(lhs->mRoiWidth != rhs->mRoiWidth) qDebug()<< "Warning: Different roi width";
//    if(lhs->mEnergyBinWidth != rhs->mEnergyBinWidth) qDebug()<< "Warning: Different bin width";
//}

