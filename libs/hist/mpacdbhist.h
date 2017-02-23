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
    void setBinContent(int nBin, double counts ,double binWidth, double incomplete);
    void setRoiInformation(double roiWidth,double roiLength,double binWidth);
    void setCalibration(float channelToEnergy,float peakEnergy,float peakPosition);
    double autoCalibration(double binWidth);
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
    Eigen::VectorXd mIncomplete;
    Eigen::VectorXd mSingleBinWidth;
    Eigen::VectorXd mProjectionHist;
    Eigen::VectorXd mNormHist;
    Eigen::VectorXd mNormFoldoverHist;
    Eigen::VectorXd mFoldoverHist;
    Eigen::VectorXd mProjectionHistError;
    Eigen::VectorXd mNormHistError;
    Eigen::VectorXd mNormFoldoverHistError;
    Eigen::VectorXd mFoldoverHistError;
    Eigen::VectorXd mEnergyScale;
    Eigen::VectorXd mEnergyScaleFoldover;
};

//MpaRefHist operator /(MpaCdbHist const& lhs,MpaCdbHist const& rhs);
//MpaCdbHist operator+(MpaCdbHist const& lhs,MpaCdbHist const& rhs);

#endif // MPACDBHIST_H
