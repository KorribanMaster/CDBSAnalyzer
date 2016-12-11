#ifndef MPACDBHIST_H
#define MPACDBHIST_H

#include <QObject>
#include <QtCore>

#include <Eigen/Dense>

#include <QObject>

//#include "mparefhist.h"

class MpaCdbHist : public QObject
{
    Q_OBJECT

public:
    MpaCdbHist(QString name, QObject *parent = 0);
    MpaCdbHist(QString name,int size, QObject *parent = 0);
    ~MpaCdbHist();

    void setSize(int nBins);
    void setBinContent(int nBin,double counts);
    void setRoiInformation(double roiWidth,double roiLength,double binWidth);
    void setCalibration(float channelToEnergy,float peakEnergy,float peakPosition);
    void calculateFoldover();
    void calculateNorm();

    //MpaCdbHist* operator=(const MpaCdbHist *other);

    QString mName;
    int mSize;
    double mNorm;
    double mRoiWidth;
    double mRoiLength;
    double mEnergyBinWidth;

    int mMaxDepth;

    Eigen::VectorXd mProjectionHist;
    Eigen::VectorXd mNormHist;
    Eigen::VectorXd mNormFoldoverHist;
    Eigen::VectorXd mFoldoverHist;
    Eigen::VectorXd mEnergyScale;
};

//MpaRefHist operator /(MpaCdbHist const& lhs,MpaCdbHist const& rhs);
//MpaCdbHist operator+(MpaCdbHist const& lhs,MpaCdbHist const& rhs);

#endif // MPACDBHIST_H
