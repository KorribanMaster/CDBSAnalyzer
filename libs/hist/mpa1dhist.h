#ifndef MPA1DHIST_H
#define MPA1DHIST_H

#include <QObject>
#include <Eigen/Dense>

class Mpa1dHist
{
public:
    Mpa1dHist(QString name);
    Mpa1dHist(QString name,int size);
    ~Mpa1dHist();
    void setSize(int nBins);
    void setBinContent(int nBin,int counts);
    void setCalibration(float channelToEnergy,float peakEnergy,float peakPosition);
    QString mName;
    int mSize;
    Eigen::VectorXf mRawHist;
    Eigen::VectorXf mEnergyScale;
};

#endif // MPA1DHIST_H
