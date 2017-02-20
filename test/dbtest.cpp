#include <QtCore>
#include <QtSql>

#include <Eigen/Dense>
#include "mpacdbhist.h"

int main(int argc, char *argv[]){
    //create hist
    MpaCdbHist *hist = new MpaCdbHist("dbtest");
    hist->setSize(10);
    for(int i=0;i<10;i++){
        hist->setBinContent(i,i,0);
        }
    hist->calculateFoldover();
    Eigen::VectorXd v;
    hist->mEnergyScale = v.setLinSpaced(10,510e3,520e3);
    hist->mEnergyScaleFoldover = v.setLinSpaced(5,515e3,520e3);
    hist->setRoiInformation(1,1,1);
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
    query.exec("CREATE TABLE IF NOT EXISTS TEST(DATE CHAR(19) NOT NULL,NAME TEXT NOT NULL, SIZE INT NOT NULL, ROIWIDTH REAL, ROILENGTH REAL NOT NULL, BINWIDTH REAL, COUNTS REAL, ENERGYSCALE_DATA VARBINARY, PROJECTION_DATA VARBINARY, PROJECTION_ERROR_DATA VARBINARY, NORM_DATA VARBINARY, NORM_ERROR_DATA VARBINARY, CONSTRAINT ID PRIMARY KEY(DATE, ROIWIDTH, ROILENGTH, BINWIDTH));");
    qDebug() << query.lastError();
    query.prepare("INSERT INTO TEST (DATE, NAME, SIZE, ROIWIDTH, ROILENGTH, BINWIDTH, COUNTS, ENERGYSCALE_DATA, PROJECTION_ERROR_DATA, NORM_DATA, NORM_ERROR_DATA) VALUES (:DATE, :NAME, :SIZE, :ROIWIDTH, :ROILENGTH, :BINWIDTH, :COUNTS, :ENERGYSCALE_DATA, :PROJECTION_ERROR_DATA, :NORM_DATA, :NORM_ERROR_DATA)");
    query.bindValue(":DATE",QString("1992-03-03_00-00-00"));
    query.bindValue(":NAME", hist->mName);
    query.bindValue(":SIZE",hist->mSize);
    query.bindValue(":ROIWIDTH",hist->mRoiWidth);
    query.bindValue(":ROILENGTH",hist->mRoiLength);
    query.bindValue(":BINWIDTH",hist->mEnergyBinWidth);
    query.bindValue(":COUNTS",hist->mNorm);
    query.bindValue(":ENERGYSCALE_DATA",energyScaleData);
    query.bindValue(":PROJECTION_DATA",projectionData);
    query.bindValue(":PROJECTION_ERROR_DATA",projectionErrorData);
    query.bindValue(":NORM_DATA", normData);
    query.bindValue(":NORM_ERROR_DATA",normErrorData);
    //query.exec("INSERT INTO CUSTOMERS (ID,NAME,AGE,ADDRESS,SALARY) VALUES (1, 'Ramesh', 32, 'Ahmedabad', 2000.00 );");
    query.exec();
    qDebug() << query.lastError();
    query.exec("SELECT * FROM test");
    while(query.next()){
        qDebug() << query.value(1);
        QByteArray data = query.value("ENERGYSCALE_DATA").toByteArray();
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
