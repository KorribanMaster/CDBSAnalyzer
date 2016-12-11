#ifndef MPAREFHIST_H
#define MPAREFHIST_H

#include <QObject>

#include "mpacdbhist.h"


class MpaRefHist : public QObject
{
    Q_OBJECT

public:
    MpaRefHist(MpaCdbHist *hist, MpaCdbHist *norm);
    ~MpaRefHist();
    void setRoiInformation(double roiWidth, double roiLength, double binWidth);

    int mSize;
    QString mName;
    QString mRefName;
    double mEnergyBinWidth;
    double mRoiWidth;
    double mRoiLength;

    Eigen::VectorXd mRefHist;
    Eigen::VectorXd mEnergyScale;
    Eigen::VectorXd mRefFoldoverHist;
    Eigen::VectorXd mFoldoverEnergyScale;

private:

};

#endif // MPAREFHIST_H
