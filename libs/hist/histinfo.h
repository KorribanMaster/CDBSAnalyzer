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

    QString name() const;
    QString index() const;
    QString type() const;
    QString ref() const;
    QString binWidth()const;
    QString roiLength() const;
    QString roiWidth() const;
    QString counts() const;
    QString size()const;

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
