#include "mainwindow2.h"
#include "ui_mainwindow2.h"

#include <QFileDialog>
#include <QSaveFile>


MainWindow2::MainWindow2(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  mCDBSCount =0;
  ui->setupUi(this);
  mCdbsTable = new HistTable(this);
  mRefTable = new HistTable(this);
  mMapTable = new HistTable(this);
  mManager = new HistManager(this);
  mPlot = new PlotWidget2();
  mMapPlot = new MapPlotWidget();
  mPlot2 = new PlotWidget2();
  //mPlot->hide();
  //mManager->moveToThread(&mThread);
  //mThread.start();
  connect(mManager,SIGNAL(updatedCdbHistList(QList<HistInfo>)),mCdbsTable,SLOT(updateInfoList(QList<HistInfo>)));
  connect(mManager,SIGNAL(updatedCdbHistList(QList<HistInfo>)),this,SLOT(addRef(QList<HistInfo>)));
  connect(mManager,SIGNAL(updated2dHistList(QList<HistInfo>)),mMapTable,SLOT(updateInfoList(QList<HistInfo>)));
  connect(mManager,SIGNAL(updatedRefHistList(QList<HistInfo>)),mRefTable,SLOT(updateInfoList(QList<HistInfo>)));
  connect(this,SIGNAL(loadHist(QString,QString)),mManager,SLOT(loadHist(QString,QString)));
  connect(this,SIGNAL(computeAll(double,double,double,int)),mManager,SLOT(projectAllCDBS(double,double,double,int)));
  connect(this,SIGNAL(referenceHist(QString,QStringList)),mManager,SLOT(referenceCDBS(QString,QStringList)));
  connect(this,SIGNAL(loadSettings(QString)),mManager,SLOT(loadSettings(QString)));
  connect(this,SIGNAL(saveHists(QString)),mManager,SLOT(saveHists(QString)));
  connect(this,SIGNAL(compute(QString,double,double,double,int)),mManager,SLOT(projectCDBS(QString,double,double,double,int)));
  connect(this,SIGNAL(saveRefs(QStringList,QString)),mManager, SLOT(saveRefs(QStringList,QString)));
  connect(this,SIGNAL(join(QStringList)),mManager,SLOT(joinCDBS(QStringList)));


  emit loadSettings("../settings/CDBSUpgrade.ini");

  connect(ui->buttonImport,SIGNAL(clicked(bool)),this,SLOT(loadButtonClicked()));
  connect(ui->buttonCompute,SIGNAL(clicked(bool)),this,SLOT(computeButtonClicked()));
  connect(ui->buttonRef,SIGNAL(clicked(bool)),this,SLOT(referenceButtonClicked()));
  connect(ui->actionSave_Session,SIGNAL(triggered(bool)),this,SLOT(saveSessionClicked()));
  connect(ui->buttonPlotCdbs,SIGNAL(clicked(bool)),this,SLOT(plotCdbsButtonClicked()));
  connect(ui->buttonPlotRef,SIGNAL(clicked(bool)),this,SLOT(plotRefButtonClicked()));
  connect(ui->buttonPreview,SIGNAL(clicked(bool)),this,SLOT(previewButtonClicked()));
  connect(ui->checkVariableRoi,SIGNAL(clicked(bool)),this, SLOT(checkVariableRoiClicked()));
  connect(ui->buttonSave,SIGNAL(clicked(bool)),this,SLOT(saveRefButtonClicked()));
  connect(ui->buttonJoin,SIGNAL(clicked(bool)),this,SLOT(joinButtonClicked()));

  connect(ui->editImportName,SIGNAL(editingFinished()),this,SLOT(importNameEdited()));
  connect(ui->editFileName,SIGNAL(editingFinished()),this,SLOT(fileNameEdited()));

  connect(ui->toolFileName,SIGNAL(clicked(bool)),this,SLOT(loadToolClicked()));

  ui->tableCDBS->setModel(mCdbsTable);

  ui->tableCDBS2->setModel(mCdbsTable);
  ui->tableImportedMaps->setModel(mMapTable);
  ui->tableMaps->setModel(mMapTable);
  ui->tableRef->setModel(mRefTable);
  ui->tableImportedMaps->setColumnHidden(1,true);
  ui->tableImportedMaps->setColumnHidden(2,true);
  ui->tableImportedMaps->setColumnHidden(3,true);
  ui->tableImportedMaps->setColumnHidden(6,true);
  ui->tableMaps->setColumnHidden(1,true);
  ui->tableMaps->setColumnHidden(2,true);
  ui->tableMaps->setColumnHidden(3,true);
  ui->tableMaps->setColumnHidden(6,true);
  ui->tableCDBS->setColumnHidden(6,true);
  ui->tableCDBS2->setColumnHidden(6,true);
  //ui->tableCDBS->show();
}

MainWindow2::~MainWindow2(){
    delete ui;
    mThread.quit();
    mThread.wait();
}

void MainWindow2::saveSessionClicked(){
    QString saveFolderName = QFileDialog::getExistingDirectory(this,"Save Folder","..data");
    emit saveHists(saveFolderName);
}

void MainWindow2::loadButtonClicked(){
    emit loadHist(ui->editFileName->text(),ui->editImportName->text());
}

void MainWindow2::loadToolClicked(){
    QString tmp = ui->editFileName->text();
    if (tmp.isEmpty()) tmp = "/home/ehecht/git/CDBSAnalyzer/data";

    QString tmp2 = QFileDialog::getOpenFileName(this,"Load Data",tmp);
    ui->editFileName->setText(tmp2);
}

void MainWindow2::computeButtonClicked(){
    double roiWidth,roiLength,binWidth;
    roiLength = ui->editRoiLength->text().toDouble();
    roiWidth = ui->editRoiWidth->text().toDouble();
    binWidth = ui->editBinWidth->text().toDouble();
    if(ui->checkVariableRoi->checkState() == Qt::Checked){
        binWidth = -1;
        roiLength = -1;
    }
    int depth = ui->comboSplitDepth->currentText().toInt();
    QStringList selected = mMapTable->getChecked();
    foreach (QString name,selected) {
             emit compute(name,roiWidth,roiLength,binWidth,depth);
    }
    //emit computeAll(roiWidth,roiLength,binWidth,depth);
}

void MainWindow2::referenceButtonClicked(){
    QString refName = ui->comboRef->currentText();
    QStringList list = mCdbsTable->getChecked();
    emit referenceHist(refName,list);
}

void MainWindow2::importNameEdited(){
    QString tmp = ui->editImportName->text();
    if (tmp.isEmpty()){
        ui->buttonImport->setEnabled(false);
    }
    else if (!ui->editFileName->text().isEmpty() && !tmp.isEmpty()) {
        ui->buttonImport->setEnabled(true);
    }
}

void MainWindow2::fileNameEdited(){
    QString tmp = ui->editFileName->text();
    if (tmp.isEmpty()){
        ui->buttonImport->setEnabled(false);
    }
    else if (!ui->editImportName->text().isEmpty() && !tmp.isEmpty()) {
        ui->buttonImport->setEnabled(true);
    }
}

void MainWindow2::addRef(QList<HistInfo> list){
    ui->buttonRef->setEnabled(true);
    int tmp = mCDBSCount;
    mCDBSCount = list.size();
    for (tmp;tmp<mCDBSCount;tmp++){
        ui->comboRef->addItem(list.at(tmp).name());
    }
}

void MainWindow2::plotCdbsButtonClicked(){
    mPlot2->deleteLater();
    mPlot2 = new PlotWidget2();
    QStringList list = mCdbsTable->getChecked();
    foreach (QString name, list) {
        MpaCdbHist *hist = mManager->getCdbHist(name);
        mPlot2->addHist(hist);
    }
    mPlot2->show();
}

void MainWindow2::plotRefButtonClicked(){
    mPlot->deleteLater();
    mPlot = new PlotWidget2();
    QStringList list = mRefTable->getChecked();
    foreach (QString name, list) {
        MpaRefHist *hist = mManager->getRefHist(name);
        mPlot->addHist(hist);
    }
    mPlot->show();
}

void MainWindow2::previewButtonClicked(){
    mMapPlot->deleteLater();
    mMapPlot = new MapPlotWidget();
    QStringList list = mMapTable->getChecked();
    if (list.isEmpty()){
        return;
    }
    mMapPlot->show();
    Mpa2dHist *hist = mManager->get2dHist(list[0]);
    if(ui->checkVariableRoi->checkState() == Qt::Unchecked){
        hist->setEnergyBinWidth(ui->editBinWidth->text().toDouble());
        hist->setRoi(ui->editRoiWidth->text().toDouble(),ui->editRoiLength->text().toDouble());
        hist->updateRoi();
    }
    else{
        hist->setEnergyBinWidth(-1);
        hist->setRoi(ui->editRoiWidth->text().toDouble(),ui->editRoiLength->text().toDouble());
        hist->updateRoi();
    }
    mMapPlot->addHist(hist);

    return;
}

void MainWindow2::saveRefButtonClicked(){
    QStringList list = mRefTable->getChecked();
    if (list.isEmpty()){
        qDebug() << "Empty list";
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this,"Export References","../data");
    if(!fileName.endsWith(".txt")){
        fileName.append(".txt");
    }
    emit saveRefs(list,fileName);

}

void MainWindow2::checkVariableRoiClicked(){
    if(ui->checkVariableRoi->checkState()==Qt::Checked){
        ui->editBinWidth->setDisabled(true);
        ui->editRoiLength->setDisabled(true);
    }
    else if(ui->checkVariableRoi->checkState() == Qt::Unchecked){
        ui->editBinWidth->setDisabled(false);
        ui->editRoiLength->setDisabled(false);
    }
}

void MainWindow2::joinButtonClicked(){
    QStringList list = mCdbsTable->getChecked();
    emit join(list);
}
