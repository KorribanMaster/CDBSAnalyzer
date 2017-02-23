#include <QtCore>
#include <QtSql>

#include <Eigen/Dense>
#include "mpacdbhist.h"

int main(int argc, char *argv[]){
    //create hist
    MpaCdbHist *hist = new MpaCdbHist("dbtest");
    hist->setSize(10);
    for(int i=0;i<10;i++){
        hist->setBinContent(i,i,0,0);
        }
    hist->calculateFoldover();
    Eigen::VectorXd v;
    hist->mEnergyScale = v.setLinSpaced(10,510e3,520e3);
    hist->mEnergyScaleFoldover = v.setLinSpaced(5,515e3,520e3);
    hist->setRoiInformation(1,1,1);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/home/eicke/git/CDBSAnalyzer/database/results.db");
    if (!db.open())
    {
        qDebug() << "Could not open database file:";
        qDebug() << db.lastError();
        return -1;
    }
    qDebug() << "Opened Database";
    QSqlQuery query(db);
    QByteArray energyScaleData,projectionData,projectionErrorData,normData,normErrorData;
    QDataStream out1(&energyScaleData,QIODevice::ReadWrite);
    QDataStream out2(&projectionData,QIODevice::ReadWrite);
    QDataStream out3(&projectionErrorData,QIODevice::ReadWrite);
    QDataStream out4(&normData,QIODevice::ReadWrite);
    QDataStream out5(&normErrorData,QIODevice::ReadWrite);
    for(int i=0;i<hist->mSize;i++){
        out1 << hist->mEnergyScale(i);
        out2 << hist->mProjectionHist(i);
        out3 << hist->mProjectionHistError(i);
        out4 << hist->mNormHist(i);
        out5 << hist->mNormHistError(i);
    }
    QByteArray energyScaleFoldoverData,projectionFoldoverData,projectionFoldoverErrorData,normFoldoverData,normFoldoverErrorData;
    QDataStream out6(&energyScaleFoldoverData,QIODevice::ReadWrite);
    QDataStream out7(&projectionFoldoverData,QIODevice::ReadWrite);
    QDataStream out8(&projectionFoldoverErrorData,QIODevice::ReadWrite);
    QDataStream out9(&normFoldoverData,QIODevice::ReadWrite);
    QDataStream out10(&normFoldoverErrorData,QIODevice::ReadWrite);
    for(int i=0;i<hist->mSize/2;i++){
        out6 << hist->mEnergyScaleFoldover(i);
        out7 << hist->mFoldoverHist(i);
        out8 << hist->mFoldoverHistError(i);
        out9 << hist->mNormFoldoverHist(i);
        out10 << hist->mNormFoldoverHistError(i);
    }

    query.prepare("INSERT INTO measurements (recordDate, name, size, roiWidth, roiLength, binWidth, counts,"
                  " energyScaleData, projectionData, projectionErrorData, normData, normErrorData,foldoverData, foldoverErrorData, normFoldoverData, normFoldoverErrorData)"
                  " VALUES (:recordDate, :name, :size, :roiWidth, :roiLength,  :binWidth, :counts, :energyScaleData, :projectionData, :projectionErrorData,"
                  " :normData, :normErrorData, :foldoverData, :foldoverErrorData, :normFoldoverData, :normFoldoverErrorData)");
    query.bindValue(":recordDate",QDateTime::currentDateTime());
    query.bindValue(":name", hist->mName);
    query.bindValue(":size",hist->mSize);
    query.bindValue(":roiWidth",hist->mRoiWidth);
    query.bindValue(":roiLength",hist->mRoiLength);
    query.bindValue(":binWidth",hist->mEnergyBinWidth);
    query.bindValue(":counts",hist->mNorm);
    query.bindValue(":energyScaleData",energyScaleData);
    query.bindValue(":projectionData",projectionData);
    query.bindValue(":projectionErrorData",projectionErrorData);
    query.bindValue(":normData", normData);
    query.bindValue(":normErrorData",normErrorData);
    query.bindValue(":foldoverData",projectionFoldoverData);
    query.bindValue(":foldoverErrorData",projectionFoldoverErrorData);
    query.bindValue(":normFoldoverData", normFoldoverData);
    query.bindValue(":normFoldoverErrorData",normFoldoverErrorData);
    query.exec();
    qDebug() << query.lastError();
    query.exec("SELECT * FROM measurements");
    while(query.next()){
        qDebug() << query.value(1);
        QByteArray data = query.value("energyScaleData").toByteArray();
        QDataStream in(&data,QIODevice::ReadOnly);
        double tmp;
        while(!in.atEnd()){
            in >> tmp;
            qDebug() << tmp;
        }

        if (data==energyScaleData) qDebug() << "success";

    }
    qDebug() << query.record().count() << query.record().fieldName(1);

    db.close();


    return 0;
}
