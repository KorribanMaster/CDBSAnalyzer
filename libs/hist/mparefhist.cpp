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
    mSize = hist->mSize;
    mRefHist.resize(mSize);
    mRefFoldoverHist.resize(mSize/2);
    for(int i=0; i< mSize;i++){
        mRefHist(i) = hist->mNormHist(i)/norm->mNormHist(i);
    }
    for(int i=0; i< mSize/2;i++){
        mRefFoldoverHist(i) = hist->mNormFoldoverHist(i)/norm->mNormFoldoverHist(i);
    }
    mName = hist->mName;
    mRefName = norm->mName;
    setRoiInformation(hist->mRoiWidth,hist->mRoiLength,hist->mEnergyBinWidth);
}

MpaRefHist::~MpaRefHist(){

}

void MpaRefHist::setRoiInformation(double roiWidth, double roiLength, double binWidth){
    mRoiWidth = roiWidth;
    mRoiLength = roiLength;
    mEnergyBinWidth = binWidth;
}
