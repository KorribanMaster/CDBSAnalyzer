#include "mpahistfile.h"

#include <QDebug>

MpaHistFile::MpaHistFile()
{
    mSettings = new QSettings();
    createHists();

}


MpaHistFile::MpaHistFile(QString fileName, QString settingsFileName,QString name){
    mFileName =fileName;
    mSettingsFileName = settingsFileName;
    mSettings = new QSettings(mSettingsFileName,QSettings::IniFormat);
    createHists();
}

MpaHistFile::~MpaHistFile(){

}

void MpaHistFile::createHists(){
    for (int i = 0; i<8;i++){
        QString name = QStringLiteral("DATA%1").arg(i);
        Mpa1dHist *hist = new Mpa1dHist(name);
        m1dHists.append(hist);
    }
    for (int i = 0; i<4;i++){
        QString name = QStringLiteral("CDAT%1").arg(i);
        Mpa1dHist *hist = new Mpa1dHist(name);
        Mpa2dHist *hist2 = new Mpa2dHist();
        mCdbHists.append(hist);
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
                in2dHist = true;
                QStringList tmpList = line.split(";");
                QString tmp = tmpList.at(1);
                tmp.replace("]","");
                entriesRemaining = tmpList.at(1).toInt();
                num2dHist++;

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
            valuesRead++;
            entriesRemaining--;
            if(entriesRemaining==0){
                in2dHist = false;
                valuesRead = 0;
            }
        }
    }
    mpaFile.close();

}

void MpaHistFile::computeCdbHists(){

}
