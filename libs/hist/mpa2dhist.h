#ifndef MPA2DHIST_H
#define MPA2DHIST_H

#include <QObject>
#include <Eigen/Dense>


#include <abstractpixel.h>
#include <cdbpixel.h>
#include <roipixel.h>
#include <mpacdbhist.h>


class Mpa2dHist : public QObject
{
    Q_OBJECT

public:
    Mpa2dHist(QString name);
    Mpa2dHist(QString name, int xbins, int ybins);
    ~Mpa2dHist();

    void setSize(int xbins, int ybins);
    void setBinContent(int xbin, int ybin,int counts);
    void setRoi(double roiWidth, double roiLength);
    void setEnergyBinWidth(double binWidth);
    void setCalibration(float xcal,float ycal);
    void setCenter(float xcenter,float ycenter);


    void findCenter();
    void updateRoi();
    void updateMap();

    MpaCdbHist* projectCDBS();
    MpaCdbHist* projectCDBS(double roiWidth, double roiLength, double binWidth);


    Eigen::MatrixXd mRawHist;
    Eigen::MatrixXd mCenteredHist;
    Eigen::VectorXd mXEnergyscale;
    Eigen::VectorXd mYEnergyscale;

    Eigen::Array2d mCenter;
    Eigen::Array2d mEnergyCenter;
    Eigen::Array2d mCal;

    std::vector<CdbPixel*> mEnergyMap;
    std::vector<CdbPixel*> mEnergyMapFiltered;
    std::vector<RoiPixel*> mRoiGrid;
    RoiPixel *mRoiBorder;


    double mRoiWidth;
    double mRoiLength;
    double mEnergyBinWidth;

    int mMaxDepth;
    int mXSize;
    int mYSize;
    int mCenteredHistSize;
    int mCdbCounter;


    QString mName;

};

#endif // MPA2DHIST_H
