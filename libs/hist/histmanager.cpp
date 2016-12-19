#include "histmanager.h"

#include <QtConcurrent>

HistManager::HistManager(QObject *parent) : QObject(parent)
{

}

HistManager::~HistManager()
{

}

void HistManager::loadHist(QString fileName, QString name){
    QFile mpaFile(fileName);
    if(!mpaFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<< "Could not open file";
        throw std::runtime_error("Bad file name");
    }
    QTextStream in(&mpaFile);
    bool in1dHist;
    bool in2dHist;
    int num1dHist = 0;
    int num2dHist = 0;
    int valuesRead = 0;
    int entriesRemaining=0;
    int lineLength = 1024; //this should be computed dynamically
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
                tmp = tmpList.at(0);
                tmp.replace("[","");
                QString histName = name + "_" +tmp;
                Mpa1dHist *hist = new Mpa1dHist(histName);
                hist->setSize(entriesRemaining);
                QString calKey = QStringLiteral("/Calibration/Single%1").arg(num1dHist-1);
                double cal = mSettings->value(calKey).toDouble();
                QString peakKey = QStringLiteral("/Calibration/Peak%1").arg(num1dHist-1);
                double peak = mSettings->value(peakKey).toDouble();
                hist->setCalibration(cal,511,peak);
                m1dHists.append(hist);

            }
            else if (line.startsWith("[CDAT")) {
                num2dHist++;
                in2dHist = true;
                QStringList tmpList = line.split(",");
                QString tmp = tmpList.at(1);
                tmp.replace("]","");
                entriesRemaining = tmp.toInt();
                tmp = tmpList.at(0);
                tmp.replace("[","");
                QString histName = name + "_" +tmp;
                Mpa2dHist *hist = new Mpa2dHist(histName);
                hist->setSize(lineLength,lineLength);
                QString cal = QStringLiteral("Calibration/Coinc%1").arg(num2dHist-1);
                QList<QVariant> calList = mSettings->value(cal).toList();
                float xcal = calList.at(0).toFloat();
                float ycal = calList.at(1).toFloat();
                hist->setCalibration(xcal,ycal);
                m2dHists.append(hist);
            }


        }
        else if (in1dHist){
            m1dHists.last()->setBinContent(valuesRead,line.toInt());
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
            m2dHists.last()->setBinContent(x,y,line.toInt());
            if(entriesRemaining==0){
                in2dHist = false;
                valuesRead = 0;
            }
        }
    }
    mpaFile.close();
    for(int i = m1dHists.size()-num1dHist;i<m1dHists.size();i++){
        HistInfo info;
        info.fillInfo(m1dHists[i]);
        info.setIndex(i);
        m1dHistInfos.append(info);
    }
    for(int i = m2dHists.size()-num2dHist;i<m2dHists.size();i++){
        HistInfo info;
        info.fillInfo(m2dHists[i]);
        info.setIndex(i);
        m2dHistInfos.append(info);
    }
    if(num1dHist>0)
        emit updated1dHistList(m1dHistInfos);
    if(num2dHist>0)
        emit updated2dHistList(m2dHistInfos);
}

void HistManager::loadSettings(QString settingsFileName){
    //maybe delete if already existent ?
    QFileInfo check_file(settingsFileName);
       // check if file exists and if yes: Is it really a file and no directory?
       if (!check_file.exists()) {
           qDebug() << "Settings file does not exist";
           qDebug() << settingsFileName;
       }
       mSettingsFileName = settingsFileName;
       mSettings = new QSettings(mSettingsFileName,QSettings::IniFormat);
       qDebug() << mSettings->allKeys();
       qDebug() << mSettings->value("Calibration/Center0").toString();
       mSettings->setValue("Calibration/Center0","30,30");
       qDebug() << mSettings->value("Calibration/Center0").toString();
}

Mpa1dHist* HistManager::get1dHist(int index){
    if (index<m1dHists.size()){
        return m1dHists[index];
    }
    else {
        qDebug() << "Index out of range";
        return Q_NULLPTR;
    }
}

Mpa1dHist* HistManager::get1dHist(QString name){
    for(int i=0;i<m1dHists.size();i++){
        if (m1dHists[i]->mName == name)
            return m1dHists[i];
    }
    qDebug() << "Histname does not match any existing hist";
    return Q_NULLPTR;
}

QStringList HistManager::get1dHistList(){
    QStringList histList;
    for(int i=0;i<m1dHists.size();i++){
        histList << m1dHists[i]->mName;
    }
    return histList;
}

Mpa2dHist* HistManager::get2dHist(int index){
    if (index<m2dHists.size()){
        return m2dHists[index];
    }
    else {
        qDebug() << "Index out of range";
        return Q_NULLPTR;
    }
}

Mpa2dHist* HistManager::get2dHist(QString name){
    for(int i=0;i<m2dHists.size();i++){
        if (m2dHists[i]->mName == name)
            return m2dHists[i];
    }
    qDebug() << "Histname does not match any existing hist";
    return Q_NULLPTR;
}

QStringList HistManager::get2dHistList(){
    QStringList histList;
    for(int i=0;i<m2dHists.size();i++){
        histList << m2dHists[i]->mName;
    }
    return histList;
}

MpaCdbHist* HistManager::getCdbHist(int index){
    if (index<mCdbHists.size()){
        return mCdbHists[index];
    }
    else {
        qDebug() << "Index out of range";
        return Q_NULLPTR;
    }
}

MpaCdbHist* HistManager::getCdbHist(QString name){
    for(int i=0;i<mCdbHists.size();i++){
        if (mCdbHists[i]->mName == name)
            return mCdbHists[i];
    }
    qDebug() << "Histname does not match any existing hist";
    return Q_NULLPTR;
}

QStringList HistManager::getCdbHistList(){
    QStringList histList;
    for(int i=0;i<mCdbHists.size();i++){
        histList << mCdbHists[i]->mName;
    }
    return histList;
}

MpaRefHist* HistManager::getRefHist(int index){
    if (index<mRefHists.size()){
        return mRefHists[index];
    }
    else {
        qDebug() << "Index out of range";
        return Q_NULLPTR;
    }
}

MpaRefHist* HistManager::getRefHist(QString name){
    for(int i=0;i<mRefHists.size();i++){
        if (mRefHists[i]->mName == name)
            return mRefHists[i];
    }
    qDebug() << "Histname does not match any existing hist";
    return Q_NULLPTR;
}

QStringList HistManager::getRefHistList(){
    QStringList histList;
    for(int i=0;i<mRefHists.size();i++){
        histList << mRefHists[i]->mName;
    }
    return histList;
}

int HistManager::num1dHists(){
    return m1dHists.size();
}

int HistManager::num2dHists(){
    return m2dHists.size();
}

int HistManager::numCdbHists(){
    return mCdbHists.size();
}

int HistManager::numRefHists(){
    return mRefHists.size();
}

void HistManager::projectCDBS(int histIndex, double roiWidth, double roiLength, double binWidth,int depth){
    Mpa2dHist * hist = get2dHist(histIndex);
    //No threading support
    MpaCdbHist *projection = hist->projectCDBS(roiWidth,roiLength,binWidth,depth);

    //Threading support
//    QFuture<MpaCdbHist*> future;
//    future = QtConcurrent::run(hist,&Mpa2dHist::projectCDBS,roiWidth,roiLength,binWidth);
//    future.waitForFinished();
//    MpaCdbHist *projection =future.result();

    mCdbHists.append(projection);
    HistInfo info;
    info.fillInfo(projection);
    info.setIndex(mCdbHists.size()-1);
    mCdbHistInfos.append(info);
    emit updatedCdbHistList(mCdbHistInfos);
}

void HistManager::projectCDBS(QString histName, double roiWidth, double roiLength, double binWidth,int depth){
    Mpa2dHist * hist = get2dHist(histName);
    //No threading support
    MpaCdbHist *projection = hist->projectCDBS(roiWidth,roiLength,binWidth,depth);
    //Threading support
//    QFuture<MpaCdbHist*> future;
//    future = QtConcurrent::run(hist,&Mpa2dHist::projectCDBS,roiWidth,roiLength,binWidth);
//    future.waitForFinished();
//    MpaCdbHist *projection =future.result();

    mCdbHists.append(projection);
    HistInfo info;
    info.fillInfo(projection);
    info.setIndex(mCdbHists.size()-1);
    mCdbHistInfos.append(info);
    emit updatedCdbHistList(mCdbHistInfos);

}

void HistManager::projectAllCDBS(double roiWidth, double roiLength, double binWidth, int depth){
    for(int i=0;i<m2dHists.size();i++){
        //No threading support
        MpaCdbHist *projection = m2dHists[i]->projectCDBS(roiWidth,roiLength,binWidth,depth);
        /*
         * Threading support
        QFuture<MpaCdbHist*> future;
        future = QtConcurrent::run(hist,&Mpa2dHist::projectCDBS,roiWidth,roiLength,binWidth);
        future.waitForFinished();
        MpaCdbHist *projection =future.result();
        */
        projection->calculateFoldover();
        mCdbHists.append(projection);
        HistInfo info;
        info.fillInfo(projection);
        info.setIndex(mCdbHists.size()-1);
        mCdbHistInfos.append(info);
    }    
    emit updatedCdbHistList(mCdbHistInfos);
}

void HistManager::referenceCDBS(int referenceHistIndex, QVector<int> histIndexes){
    for(int i=0; i< histIndexes.size();i++){
        MpaRefHist *hist = new MpaRefHist(getCdbHist(referenceHistIndex),getCdbHist(histIndexes[i]));
        mRefHists.append(hist);
        HistInfo info;
        info.fillInfo(hist);
        info.setIndex(mCdbHists.size()-1);
        mRefHistInfos.append(info);
    }
    emit updatedRefHistList(mRefHistInfos);
}

void HistManager::referenceCDBS(QString referenceHistName, QStringList histNames){
    for(int i=0; i< histNames.size();i++){
        MpaRefHist *hist = new MpaRefHist(getCdbHist(referenceHistName),getCdbHist(histNames[i]));
        mRefHists.append(hist);
        HistInfo info;
        info.fillInfo(hist);
        info.setIndex(mCdbHists.size()-1);
        mRefHistInfos.append(info);
    }
    emit updatedRefHistList(mRefHistInfos);
}

void HistManager::saveHists(QString saveFolderName){
    QString dirName = saveFolderName+"/"+QDateTime::currentDateTime().toString();
    QDir().mkdir(dirName);
    foreach (MpaCdbHist *hist, mCdbHists) {
        QString fileName = dirName +"/"+hist->mName+".txt";
        QFile file(fileName);
        if(!file.open(QIODevice::ReadWrite)){
            qDebug() << "Failed to open file";
        }
        QTextStream out(&file);
        out << "#" << hist->mName << "\n";
        out << "# Projection Histogram\n";
        out << "'Energy; Counts\n";
        for(int i=0;i<hist->mProjectionHist.size();i++){
            out << hist->mEnergyScale(i) << "; " << hist->mProjectionHist(i) <<"\n";
        }
        out << "# Norm Histogram\n";
        out << "'Energy; Counts\n";
        for(int i=0;i<hist->mNormHist.size();i++){
            out << hist->mEnergyScale(i) << "; " << hist->mNormHist(i) <<"\n";
        }
        out << "# Foldover Histogram\n";
        out << "'Energy; Counts\n";
        for(int i=0;i<hist->mFoldoverHist.size();i++){
            out << hist->mEnergyScale(i+hist->mSize/2) << "; " << hist->mFoldoverHist(i) <<"\n";
        }
        out << "# Norm Foldover Histogram\n";
        out << "'Energy; Counts\n";
        for(int i=0;i<hist->mNormFoldoverHist.size();i++){
            out << hist->mEnergyScale(i+hist->mSize/2) << "; " << hist->mNormFoldoverHist(i) <<"\n";
        }
        file.close();

    }
}
