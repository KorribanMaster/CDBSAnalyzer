#ifndef MPA2DHIST_H
#define MPA2DHIST_H

#include <QObject>
#include <Eigen/Dense>

#include <cdbpixel.h>
#include <roipixel.h>
#include <mpa1dhist.h>


class Mpa2dHist
{
public:
    Mpa2dHist(QString name);
    Mpa2dHist(QString name, int xbins, int ybins);
    ~Mpa2dHist();

    void setSize(int xbins, int ybins);
    void setBinContent(int xbin, int ybin,int counts);
    void setRoi(double roiWidth, double roiLength);
    void setEnergyBinWidth(double binWidth);
    void setCalibration(float xcal,float ycal);

    void findCenter();
    void updateRoi();
    void updateMap();

    Mpa1dHist* projectCDBS();
    Mpa1dHist* projectCDBS(double roiWidth, double roiLength, double binWidth);


    Eigen::MatrixXi mRawHist;
    Eigen::MatrixXi mCenteredHist;
    Eigen::VectorXf mXEnergyscale;
    Eigen::VectorXf mYEnergyscale;

    Eigen::Vector2i mCenter;
    Eigen::Vector2f mEnergyCenter;
    Eigen::Vector2f mCal;

    QList<CdbPixel*> mEnergyMap;
    QList<RoiPixel*> mRoiGrid;
    RoiPixel *mRoiBorder;

    double mRoiWidth;
    double mRoiLength;
    double mEnergyBinWidth;

    QString mName;

};

#endif // MPA2DHIST_H
