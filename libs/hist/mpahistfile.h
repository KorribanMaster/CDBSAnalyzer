#ifndef MPAHISTFILE_H
#define MPAHISTFILE_H

#include <QObject>
#include <QtCore>
#include <QSettings>

#include "mpa1dhist.h"
#include "mpa2dhist.h"

class MpaHistFile
{
public:
    MpaHistFile();
    MpaHistFile(QString fileName,QString settingsFileName,QString name);
    ~MpaHistFile();

    void setFileName(QString fileName);
    void setSettingsFileName(QString fileName);
    void setName(QString name);
    int loadFile();
    void createHists();

    void computeCdbHists();

    double mRoiWidth;
    double mBinWidth;
    QString mName;

    QList<Mpa1dHist*> m1dHists;
    QList<Mpa2dHist*> m2dHists;
    QList<Mpa1dHist*> mCdbHists;

private:


    QString mFileName;
    QString mSettingsFileName;
    QString mHeader;
    QSettings *mSettings;

};

#endif // MPAHISTFILE_H
