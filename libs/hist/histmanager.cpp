#include "histmanager.h"

#include <QtConcurrent>
#include <QUuid>

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
//                QString cal = QStringLiteral("Calibration/Coinc%1").arg(num2dHist-1);
//                QList<QVariant> calList = mSettings->value(cal).toList();
//                float xcal = calList.at(0).toFloat();
//                float ycal = calList.at(1).toFloat();
//                hist->setCalibration(xcal,ycal);
                    QString key = QString("Pair%1").arg(num2dHist);
                    QString pair = mSettings->value(key).toString();
                    float mec2 = mSettings->value("mec2").toFloat();
                    QStringList pairs = pair.split("x");
                    mSettings->beginGroup(pairs.at(0));
                    float energyXCal =mSettings->value("EnergyChannel").toFloat();
                    float energyXOffset = mSettings->value("EnergyOffset").toFloat();
                    float cdbXOffset = mSettings->value("CDBOffset").toFloat();
                    float peakXChannel = (mec2-energyXOffset)/energyXCal-cdbXOffset;
                    mSettings->endGroup();
                    mSettings->beginGroup(pairs.at(1));
                    float energyYCal =mSettings->value("EnergyChannel").toFloat();
                    float energyYOffset = mSettings->value("EnergyOffset").toFloat();
                    float cdbYOffset = mSettings->value("CDBOffset").toFloat();
                    float peakYChannel = (mec2-energyYOffset)/energyYCal-cdbYOffset;
                    mSettings->endGroup();
                    hist->setCalibration(energyXCal*1e3,energyYCal*1e3);
                    hist->setCenter(peakXChannel,peakYChannel);


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
        HistInfo info(m1dHists[i],i);
        m1dHistInfos.append(info);
    }
    for(int i = m2dHists.size()-num2dHist;i<m2dHists.size();i++){
        HistInfo info(m2dHists[i],i);
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
       openDb();
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
    HistInfo info(projection,mCdbHists.size()-1);
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
    HistInfo info(projection,mCdbHists.size()-1);
    mCdbHistInfos.append(info);
    exportToDb(projection);
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
        HistInfo info(projection,mCdbHists.size()-1);
        mCdbHistInfos.append(info);
    }    
    emit updatedCdbHistList(mCdbHistInfos);
}

void HistManager::referenceCDBS(int referenceHistIndex, QVector<int> histIndexes){
    QVector<int> tmp = histIndexes;
    tmp.append(referenceHistIndex);
    if(!checkRefHistCompatability(tmp)){
        qDebug() << "Incompatible Histogramms";
        return;
    }
    for(int i=0; i< histIndexes.size();i++){
        MpaRefHist *hist = new MpaRefHist(getCdbHist(referenceHistIndex),getCdbHist(histIndexes[i]));
        mRefHists.append(hist);
        HistInfo info(hist,mRefHists.size()-1);
        mRefHistInfos.append(info);
    }
    emit updatedRefHistList(mRefHistInfos);
}

void HistManager::referenceCDBS(QString referenceHistName, QStringList histNames){
    QStringList tmp = histNames;
    tmp.append(referenceHistName);
    if(!checkRefHistCompatability(tmp)){
        qDebug() << "Incompatible Histogramms";
        return;
    }
    for(int i=0; i< histNames.size();i++){
        MpaRefHist *hist = new MpaRefHist(getCdbHist(histNames[i]),getCdbHist(referenceHistName));
        mRefHists.append(hist);
        HistInfo info(hist,mRefHists.size()-1);
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
        out << "#bin width = " << hist->mEnergyBinWidth << "\n#roi width = "<< hist->mRoiWidth << "\n#roi length = "<< hist->mRoiLength <<"\n#counts = "<< hist->mNorm;
        out << "#Energy; Projection;Projection Error; Norm; Norm Error; Foldover; Foldover Error; Norm Foldover; Norm Foldover Error\n";
        for(int i=0;i<hist->mSize/2;i++){
            out << hist->mEnergyScale(i) << "; " << hist->mProjectionHist(i) << "; " << hist->mProjectionHistError(i) << "; " << hist->mNormHist(i) << "; " << hist->mNormHistError(i) << "; ; ; ; \n";
        }
        for(int i=hist->mSize/2;i<hist->mSize;i++){
            out << hist->mEnergyScale(i) << "; " << hist->mProjectionHist(i) << "; " << hist->mProjectionHistError(i) << "; " << hist->mNormHist(i) << "; " << hist->mNormHistError(i) << "; " << hist->mFoldoverHist(i) << "; " << hist->mFoldoverHistError(i) << "; " << hist->mNormFoldoverHist(i) << "; " << hist->mNormFoldoverHistError(i)<< "\n";
        }
        file.close();
    }


}

void HistManager::saveRefs(QStringList histNames, QString saveFileName){
    QVector<MpaRefHist*> hists;
    bool ok = checkRefHistCompatability(histNames);
    if(!ok){
        qDebug() << "References incompatible: Canceling Saving";
        emit error("References incompatible: Canceling Saving");
    }
    foreach (QString histName, histNames) {
        MpaRefHist *hist = getRefHist(histName);
        hists.push_back(hist);
    }
    QFile file(saveFileName);
    if(!file.open(QIODevice::ReadWrite)){
        qDebug() << "Failed to open file";
    }
    QTextStream out(&file);
    out << "#Reference Hists\n";
    out << "#bin width = " << hists[0]->mEnergyBinWidth << "\n#roi width = "<< hists[0]->mRoiWidth << "\n#roi length = "<< hists[0]->mRoiLength <<"\n";
    out << "#foldover";
    out << "#energyscale; ";
    foreach (MpaRefHist *hist, hists) {
        out << hist->mName << "; " << hist->mName << "Error; ";
    }
    out <<"\n";
    for(int i=0; i<hists[0]->mSize/2;i++){
        out << hists[0]->mFoldoverEnergyScale(i) << "; ";
        foreach (MpaRefHist *hist, hists) {
            out << hist->mRefFoldoverHist(i) << "; " << hist->mRefFoldoverHistError(i) << "; ";
        }
        out <<"\n";
    }
    out << "#non foldover";
    out << "#energyscale; ";
    foreach (MpaRefHist *hist, hists) {
        out << hist->mName << "; " << hist->mName << "Error; ";
    }
    out <<"\n";
    for(int i=0; i<hists[0]->mSize;i++){
        out << hists[0]->mEnergyScale(i) << "; ";
        foreach (MpaRefHist *hist, hists) {
            out << hist->mRefHist(i) << "; " << hist->mRefHistError(i) << "; ";
        }
        out <<"\n";
    }
    file.close();

}

bool HistManager::checkCdbHistCompatability(QVector<int> histIndexes){
    QVector<MpaCdbHist*> hists;
    if(histIndexes.size()==0) return false;
    foreach (int histIndex, histIndexes) {
        MpaCdbHist *hist = getCdbHist(histIndex);
        hists.push_back(hist);
    }
    double roiWidth,roiLength,binWidth,size;
    roiWidth = hists[0]->mRoiWidth;
    roiLength = hists[0]->mRoiLength;
    binWidth = hists[0]->mEnergyBinWidth;
    size = hists[0]->mSize;
    bool result = true;
    foreach (MpaCdbHist *hist, hists) {
        if(roiWidth!=hist->mRoiWidth) result = false;
        if(roiLength!=hist->mRoiLength) result = false;
        if(binWidth!=hist->mEnergyBinWidth) result = false;
        if(size!=hist->mSize) result = false;
    }
    return result;

}

bool HistManager::checkCdbHistCompatability(QStringList histNames){
    QVector<MpaCdbHist*> hists;
    if(histNames.size()==0) return false;
    foreach (QString histName, histNames) {
        MpaCdbHist *hist = getCdbHist(histName);
        hists.push_back(hist);
    }
    double roiWidth,roiLength,binWidth,size;
    roiWidth = hists[0]->mRoiWidth;
    roiLength = hists[0]->mRoiLength;
    binWidth = hists[0]->mEnergyBinWidth;
    size = hists[0]->mSize;
    bool result = true;
    foreach (MpaCdbHist *hist, hists) {
        if(roiWidth!=hist->mRoiWidth) result = false;
        if(roiLength!=hist->mRoiLength) result = false;
        if(binWidth!=hist->mEnergyBinWidth) result = false;
        if(size!=hist->mSize) result = false;
    }
    return result;
}

bool HistManager::checkRefHistCompatability(QVector<int> histIndexes){
    QVector<MpaRefHist*> hists;
    if(histIndexes.size()==0) return false;
    foreach (int histIndex, histIndexes) {
        MpaRefHist *hist = getRefHist(histIndex);
        hists.push_back(hist);
    }
    double roiWidth,roiLength,binWidth,size;
    roiWidth = hists[0]->mRoiWidth;
    roiLength = hists[0]->mRoiLength;
    binWidth = hists[0]->mEnergyBinWidth;
    size = hists[0]->mSize;
    bool result = true;
    foreach (MpaRefHist *hist, hists) {
        if(roiWidth!=hist->mRoiWidth) result = false;
        if(roiLength!=hist->mRoiLength) result = false;
        if(binWidth!=hist->mEnergyBinWidth) result = false;
        if(size!=hist->mSize) result = false;
    }
    return result;

}
bool HistManager::checkRefHistCompatability(QStringList histNames){
    QVector<MpaRefHist*> hists;
    if(histNames.size()==0) return false;
    foreach (QString histName, histNames) {
        MpaRefHist *hist = getRefHist(histName);
        hists.push_back(hist);
    }
    double roiWidth,roiLength,binWidth,size;
    roiWidth = hists[0]->mRoiWidth;
    roiLength = hists[0]->mRoiLength;
    binWidth = hists[0]->mEnergyBinWidth;
    size = hists[0]->mSize;
    bool result = true;
    foreach (MpaRefHist *hist, hists) {
        if(roiWidth!=hist->mRoiWidth) result = false;
        if(roiLength!=hist->mRoiLength) result = false;
        if(binWidth!=hist->mEnergyBinWidth) result = false;
        if(size!=hist->mSize) result = false;
    }
    return result;
}

void HistManager::joinCDBS(QStringList histNames){
    if(!checkCdbHistCompatability(histNames)){
        qDebug() << "Can't join hists";
        return;
    }
    QVector<MpaCdbHist*> hists;
    foreach (QString histName, histNames) {
        MpaCdbHist *hist = getCdbHist(histName);
        hists.push_back(hist);
    }
    QString tmp = hists[0]->mName;
    MpaCdbHist *sum = new MpaCdbHist(tmp.replace("_CDAT","_SUM"));
    sum->setSize(hists[0]->mSize);
    sum->setRoiInformation(hists[0]->mRoiWidth,hists[0]->mRoiLength,hists[0]->mEnergyBinWidth);
    sum->mEnergyScale = hists[0]->mEnergyScale;
    sum->mEnergyScaleFoldover = hists[0]->mEnergyScaleFoldover;
    sum->mProjectionHist.setZero(sum->mSize);
    for(int i=0;i<hists.size();i++){
        sum->mProjectionHist += hists[i]->mProjectionHist;
    }
    sum->mProjectionHistError = sum->mProjectionHist.array().sqrt().matrix();
    sum->calculateFoldover();
    mCdbHists.append(sum);
    HistInfo info(sum,mCdbHists.size()-1);
    mCdbHistInfos.append(info);
    emit updatedCdbHistList(mCdbHistInfos);
}

bool HistManager::exportToDb(MpaCdbHist *hist){
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
    QSqlQuery query(mDb);
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


}

MpaCdbHist* HistManager::importFromDb(QString date, double roiWidth, double roiLength, double binWidth){

}

bool HistManager::openDb(QString path){
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    if(path.isEmpty()){
        path ="/home/eicke/git/CDBSAnalyzer/database/results.db";
    }
    db.setDatabaseName(path);
    if (!db.open())
    {
        qDebug() << "Could not open database file:";
        qDebug() << db.lastError();
        return -1;
    }
    qDebug() << "Opened Database";
    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS measurements ("
               "recordDate TIMESTAMP NOT NULL, "
               "name TEXT, "
               "size INT NOT NULL, "
               "roiWidth REAL NOT NULL, "
               "roiLength REAL NOT NULL, "
               "binWidth REAL NOT NULL, "
               "counts INT, "
               "baseUUID VARCHAR(38), "
               "energyScaleData VARBINARY, "
               "projectionData VARBINARY, "
               "projectionErrorData VARBINARY, "
               "normData VARBINARY, "
               "normErrorData VARBINARY, "
               "foldoverData VARBINARY, "
               "foldoverErrorData VARBINARY, "
               "normFoldoverData VARBINARY, "
               "normFoldoverErrorData VARBINARY,"
               "CONSTRAINT id PRIMARY KEY(recordDate, roiWidth, roiLength, binWidth));");
    qDebug() << query.lastError();
}
