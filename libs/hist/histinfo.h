#ifndef HISTINFO_H
#define HISTINFO_H

#include <QObject>

#include "mpa1dhist.h"
#include "mpa2dhist.h"
#include "mpacdbhist.h"
#include "mparefhist.h"

class HistInfo //: public QObject
{
    //Q_OBJECT
public:
    HistInfo();

    QString name() ;
    QString index();
    QString type() ;
    QString ref() ;
    QString binWidth();
    QString roiLength();
    QString roiWidth();
    QString counts();
    QString size();

    void fillInfo(Mpa1dHist* hist);
    void fillInfo(Mpa2dHist* hist);
    void fillInfo(MpaCdbHist* hist);
    void fillInfo(MpaRefHist* hist);
    void setIndex(int index);


private:



    QString mName;
    QString mIndex;
    QString mType;
    QString mRefName;
    QString mBinWidth;
    QString mRoiWidth;
    QString mRoiLength;
    QString mCounts;
    QString mSize;


signals:

public slots:
};

#endif // HISTINFO_H
