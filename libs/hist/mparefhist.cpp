#include "mparefhist.h"
#include <QDebug>

MpaRefHist::MpaRefHist(MpaCdbHist *hist, MpaCdbHist *norm)
{
    hist->calculateFoldover();
    norm->calculateFoldover();
    if(hist->mSize != norm->mSize){
        throw std::invalid_argument("Histogramms need to have the same size");
    }
    if(hist->mEnergyBinWidth != norm->mEnergyBinWidth) qDebug()<< "Warning: Different bin width";
    if(hist->mRoiLength != norm->mRoiLength) qDebug() << "Warning: Different roi length";
    if(hist->mRoiWidth != norm->mRoiWidth) qDebug() << "Warning: Different roi width";
    mName = hist->mName;
    mRefName = norm->mName;
    setRoiInformation(hist->mRoiWidth,hist->mRoiLength,hist->mEnergyBinWidth);
    mSize = hist->mSize;
    mRefHist.resize(mSize);
    mEnergyScale.resize(mSize);
    mRefFoldoverHist.resize(mSize/2);
    mFoldoverEnergyScale.resize(mSize/2);
    for(int i=0; i< mSize;i++){
        mRefHist(i) = hist->mNormHist(i)/norm->mNormHist(i);
        mEnergyScale(i) = hist->mEnergyScale(i);
    }
    for(int i=0; i< mSize/2;i++){
        mRefFoldoverHist(i) = hist->mNormFoldoverHist(i)/norm->mNormFoldoverHist(i);
        mFoldoverEnergyScale(i) = 511e3+i*mEnergyBinWidth;
    }
}

MpaRefHist::~MpaRefHist(){

}

void MpaRefHist::setRoiInformation(double roiWidth, double roiLength, double binWidth){
    mRoiWidth = roiWidth;
    mRoiLength = roiLength;
    mEnergyBinWidth = binWidth;
}
