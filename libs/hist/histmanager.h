#ifndef HISTMANAGER_H
#define HISTMANAGER_H

#include <QObject>
#include <QtCore>
#include <QSettings>
#include <QtSql>

#include "mpa1dhist.h"
#include "mpa2dhist.h"
#include "mpacdbhist.h"
#include "mparefhist.h"
//#include "mpahistfile.h"
#include "histinfo.h"

class HistManager : public QObject
{
    Q_OBJECT
public:
    explicit HistManager(QObject *parent= 0);
    ~HistManager();

private:
    QList<Mpa1dHist*> m1dHists;
    QList<HistInfo> m1dHistInfos;
    QList<Mpa2dHist*> m2dHists;
    QList<HistInfo> m2dHistInfos;
    QList<MpaCdbHist*> mCdbHists;
    QList<HistInfo> mCdbHistInfos;
    QList<MpaRefHist*> mRefHists;
    QList<HistInfo> mRefHistInfos;
    QSettings *mSettings;
    QString mSettingsFileName;
    QSqlDatabase mDb;

public:

    bool exportToDb(MpaCdbHist hist);
    MpaCdbHist* importFromDb(QString date,double roiWidth,double roiLength, double binWidth);
    Mpa1dHist* get1dHist(int index);
    Mpa1dHist* get1dHist(QString name);
    QStringList get1dHistList();
    int num1dHists();

    Mpa2dHist* get2dHist(int index);
    Mpa2dHist* get2dHist(QString name);
    QStringList get2dHistList();
    int num2dHists();

    MpaCdbHist* getCdbHist(int index);
    MpaCdbHist* getCdbHist(QString name);
    QStringList getCdbHistList();
    int numCdbHists();

    MpaRefHist* getRefHist(int index);
    MpaRefHist* getRefHist(QString name);
    QStringList getRefHistList();
    int numRefHists();

public slots:
    void saveHists(QString saveFolderName);

    void loadHist(QString fileName,QString name);
    void loadSettings(QString settingsFileName);

    void projectCDBS(QString histName,double roiWidth, double roiLength, double binWidth,int depth);
    void projectCDBS(int histIndex,double roiWidth, double roiLength, double binWidth,int depth);
    void projectAllCDBS(double roiWidth, double roiLength, double binWidth,int depth);

    void referenceCDBS(QString referenceHistName,QStringList histNames);
    void referenceCDBS(int referenceHistIndex,QVector<int> histIndexes);



signals:
    void projectionReady();
    void referenceReady();

    void updated1dHistList(QList<HistInfo> list);
    void updated2dHistList(QList<HistInfo> list);
    void updatedCdbHistList(QList<HistInfo> list);
    void updatedRefHistList(QList<HistInfo> list);
};

#endif // HISTMANAGER_H
