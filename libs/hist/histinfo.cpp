#include "histinfo.h"

HistInfo::HistInfo()
{


}



QString HistInfo::binWidth() {
    return mBinWidth;
}

QString HistInfo::roiLength() {
    return mRoiLength;
}

QString HistInfo::roiWidth() {
    return mRoiWidth;
}

QString HistInfo::name() {
    return mName;
}

QString HistInfo::index() {
    return mIndex;
}

QString HistInfo::ref() {
    return mRefName;
}

QString HistInfo::counts() {
    return mCounts;
}

QString HistInfo::size() {
    return mSize;
}

void HistInfo::fillInfo(Mpa1dHist *hist){
    //mName = hist->mName;
    mRefName = "NA";
    mRoiLength = "NA";
    mRoiWidth = "NA";
    mBinWidth = "NA";
    int counts = 0;
    for(int i=0; i< hist->mSize; i++){
        counts += hist->mRawHist(i);
    }
    mCounts = QString("%1").arg(counts);
    mSize = QString("%1").arg(hist->mSize);


}

void HistInfo::fillInfo(Mpa2dHist *hist){
    mName = hist->mName;
    mRefName = "NA";
    mRoiLength = QString("%1").arg(hist->mRoiWidth);
    mRoiWidth = QString("%1").arg(hist->mRoiLength);
    mBinWidth = QString("%1").arg(hist->mEnergyBinWidth);
    mCounts = QString("%1").arg(hist->mRawHist.sum());
    mSize = QString("%1 x %2").arg(hist->mXSize).arg(hist->mYSize);


}

void HistInfo::fillInfo(MpaCdbHist *hist){
    mName = hist->mName;
    mRefName = "NA";
    mRoiLength = QString("%1").arg(hist->mRoiWidth);
    mRoiWidth = QString("%1").arg(hist->mRoiLength);
    mBinWidth = QString("%1").arg(hist->mEnergyBinWidth);
    mCounts = QString("%1").arg(hist->mNorm);
    mSize = QString("%1").arg(hist->mSize);


}

void HistInfo::fillInfo(MpaRefHist *hist){
    mName = hist->mName;
    mRefName = hist->mRefName;
    mRoiLength = QString("%1").arg(hist->mRoiWidth);
    mRoiWidth = QString("%1").arg(hist->mRoiLength);
    mBinWidth = QString("%1").arg(hist->mEnergyBinWidth);
    mCounts = QString("1");
    mSize = QString("%1").arg(hist->mSize);


}

void HistInfo::setIndex(int index){
    mIndex = QString("%1").arg(index);
}
