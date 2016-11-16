#include "mpahistfile.h"

#include <QDebug>

MpaHistFile::MpaHistFile()
{
    mSettings = new QSettings();
    for (int i = 0; i<8;i++){
        Mpa1dHist *hist = new Mpa1dHist();
        m1dHists.append(hist);
    }
    for (int i = 0; i<4;i++){
        Mpa1dHist *hist = new Mpa1dHist();
        Mpa2dHist *hist2 = new Mpa2dHist();
        mCdbHists.append(hist);
        m2dHists.append(hist2);
    }

}
MpaHistFile::MpaHistFile(QString fileName, QString settingsFileName,QString name="default"){
    mFileName =fileName;
    mSettingsFileName = settingsFileName;
    mSettings = new QSettings(mSettingsFileName,QSettings::IniFormat);
    if (name=="default"){
        mName = mFileName.replace(".mpa","");
    }
}

MpaHistFile::~MpaHistFile(){

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
    if(!mpaFile.open(QIODevice::ReadWrite | QIODevice::Text)){
        qDebug()<< "Could not open file";
        return -1;
    }
    QTextStream in(&mpaFile);
    bool in1dHist;
    bool in2dHist;
    int num1dHist = 0;
    int num2dHist = 0;
    int entriesRemaining;
    while(!in.atEnd()){
        QString line = in.readLine();
        if(!in1dHist && !in2dHist){
            if(line.startsWith("[DATA")){
                in1dHist = true;
                QStringList tmpList = line.split(";");
                tmpList.at(1).replace("]","");
                entriesRemaining = tmpList.at(1).toInt();
                num1dHist++;
            }
            else if (line.startsWith("[CDAT")) {
                in2dHist = true;
                QStringList tmpList = line.split(";");
                tmpList.at(1).replace("]","");
                entriesRemaining = tmpList.at(1).toInt();
                num2dHist++;

            }
            else {
                mHeader.append(line);
                mHeader.append("\n");
            }

        }
        else if (in1dHist){
            m1dHists.at(num1dHist-1)->addBin(line.toInt());
            entriesRemaining--;
            if(entriesRemaining==0){
                in1dHist = false;
            }
        }
        else if (in2dHist){
            entriesRemaining--;
            if(entriesRemaining==0){
                in2dHist = false;
            }
        }
    }

}

void MpaHistFile::computeCdbHists(){

}
