#include "mpa1dhist.h"

#include <Eigen/Core>
Mpa1dHist::Mpa1dHist(QString name){
    setSize(0);
    mName = name;
}

Mpa1dHist::Mpa1dHist(QString name,int size){
    mName = name;
    setSize(size);
}

Mpa1dHist::~Mpa1dHist(){

}

void Mpa1dHist::setSize(int size){
    mSize = size;
    mRawHist.resize(mSize);
    mEnergyScale.resize(mSize);
}

void Mpa1dHist::setBinContent(int nBin,double counts){
    mRawHist(nBin) = counts;
}

void Mpa1dHist::setCalibration(float channelToEnergy, float peakEnergy, float peakPosition){
    mEnergyScale.setLinSpaced(mSize,peakEnergy-peakPosition*channelToEnergy,peakEnergy+peakPosition*channelToEnergy);
}
