#include "mpahistfile.h"

#include <QDebug>


MpaHistFile::MpaHistFile(QString fileName, QString settingsFileName,QString name){
    mFileName =fileName;
    mSettingsFileName = settingsFileName;
    mSettings = new QSettings(mSettingsFileName,QSettings::IniFormat);
    createHists();
}

MpaHistFile::~MpaHistFile(){
    for(int i=0;i<m2dHists.size();i++){
        delete m2dHists[i];
    }
    for(int i=0;i<m1dHists.size();i++){
        delete m1dHists[i];
    }
    for(int i=0;i<mCdbHists.size();i++){
        delete mCdbHists[i];
    }

}

void MpaHistFile::createHists(){
   for (int i = 0; i < mSettings->value("NumDet").toInt();i++){
        QString name = QStringLiteral("DATA%1").arg(i);
        Mpa1dHist *hist = new Mpa1dHist(name);
        QString calKey = QStringLiteral("Calibration/Single%1").arg(i);
        double cal = mSettings->value(calKey).toDouble();
        QString peakKey = QStringLiteral("Calibration/Peak%1").arg(i);
        double peak = mSettings->value(peakKey).toDouble();
        hist->setCalibration(cal,511,peak);
        m1dHists.append(hist);
    }
    for (int i = 0; i < (mSettings->value("NumDet").toInt()/2);i++){
        QString name = QStringLiteral("CDAT%1").arg(i);
        Mpa2dHist *hist2 = new Mpa2dHist(name);
        QString cal = QStringLiteral("Calibration/Coinc%1").arg(i);
        QList<QVariant> calList = mSettings->value(cal).toList();
        float xcal = calList.at(0).toFloat();
        float ycal = calList.at(1).toFloat();
        hist2->setCalibration(xcal,ycal);
        m2dHists.append(hist2);
    }
}

void MpaHistFile::setFileName(QString fileName){
    mFileName = fileName;

}

void MpaHistFile::setSettingsFileName(QString fileName){
    mSettingsFileName = fileName;
    mSettings->deleteLater();
    mSettings = new QSettings(mSettingsFileName,QSettings::IniFormat);

}

void MpaHistFile::setName(QString name){

}

int MpaHistFile::loadFile(){
    QFile mpaFile(mFileName);
    if(!mpaFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<< "Could not open file";
        return -1;
    }
    QTextStream in(&mpaFile);
    bool in1dHist;
    bool in2dHist;
    int num1dHist = 0;
    int num2dHist = 0;
    int valuesRead = 0;
    int entriesRemaining=0;
    int lineLength = 1024; //this should be computed dynamically
    mHeader += in.readLine()+"\n";
    while(in.atEnd()==false){
        QString line = in.readLine();
        if(!in1dHist && !in2dHist){
            if(line.startsWith("[DATA")){
                num1dHist++;
                in1dHist = true;
                QStringList tmpList = line.split(",");
                QString tmp = tmpList.at(1);
                tmp.replace("]","");
                entriesRemaining = tmp.toInt();
                m1dHists.at(num1dHist-1)->setSize(entriesRemaining);

            }
            else if (line.startsWith("[CDAT")) {
                num2dHist++;
                in2dHist = true;
                QStringList tmpList = line.split(",");
                QString tmp = tmpList.at(1);
                tmp.replace("]","");
                entriesRemaining = tmp.toInt();
                m2dHists[num2dHist-1]->setSize(lineLength,lineLength);//this should be computed dynamically


            }
            else {
                mHeader.append(line);
                mHeader.append("\n");
            }

        }
        else if (in1dHist){
            m1dHists.at(num1dHist-1)->setBinContent(valuesRead,line.toInt());
            valuesRead++;
            entriesRemaining--;
            if(entriesRemaining==0){
                in1dHist = false;
                valuesRead = 0;
            }
        }
        else if (in2dHist){
            entriesRemaining--;
            int x = valuesRead%lineLength;
            int y = valuesRead/lineLength;
            valuesRead++;
            m2dHists[num2dHist-1]->setBinContent(x,y,line.toInt());
            if(entriesRemaining==0){
                in2dHist = false;
                valuesRead = 0;
            }
        }
    }
    mpaFile.close();

}

void MpaHistFile::computeCdbHists(double roiWidth,double roiLength,double binWidth){
    for (int i=0;i<m2dHists.size();i++){
        Mpa2dHist *hist = m2dHists.at(i);
        hist->setRoi(roiWidth,roiLength);
        hist->setEnergyBinWidth(binWidth);
        MpaCdbHist *cdbHist = hist->projectCDBS();
        mCdbHists.push_back(cdbHist);
    }



}
