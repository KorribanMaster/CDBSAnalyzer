#include "mainwindow2.h"
#include "ui_mainwindow2.h"

#include <QFileDialog>
#include <QSaveFile>


MainWindow2::MainWindow2(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  mCdbsTable = new HistTable(this);
  mRefTable = new HistTable(this);
  mMapTable = new HistTable(this);
  mManager = new HistManager(this);
  connect(mManager,SIGNAL(updatedCdbHistList(QList<HistInfo>)),mCdbsTable,SLOT(updateInfoList(QList<HistInfo>)));
  connect(mManager,SIGNAL(updated2dHistList(QList<HistInfo>)),mMapTable,SLOT(updateInfoList(QList<HistInfo>)));
  connect(mManager,SIGNAL(updatedRefHistList(QList<HistInfo>)),mRefTable,SLOT(updateInfoList(QList<HistInfo>)));
  connect(this,SIGNAL(loadHist(QString,QString)),mManager,SLOT(loadHist(QString,QString)));
  connect(this,SIGNAL(computeAll(double,double,double)),mManager,SLOT(projectAllCDBS(double,double,double)));
  connect(this,SIGNAL(referenceHist(QString,QStringList)),mManager,SLOT(referenceCDBS(QString,QStringList)));
  connect(this,SIGNAL(loadSettings(QString)),mManager,SLOT(loadSettings(QString)));

  emit loadSettings("../settings/test.ini");

  connect(ui->buttonImport,SIGNAL(clicked(bool)),this,SLOT(loadButtonClicked()));
  connect(ui->buttonCompute,SIGNAL(clicked(bool)),this,SLOT(computeButtonClicked()));
  connect(ui->buttonRef,SIGNAL(clicked(bool)),this,SLOT(referenceButtonClicked()));

  connect(ui->editImportName,SIGNAL(editingFinished()),this,SLOT(importNameEdited()));
  connect(ui->editFileName,SIGNAL(editingFinished()),this,SLOT(fileNameEdited()));

  connect(ui->toolFileName,SIGNAL(clicked(bool)),this,SLOT(loadToolClicked()));

  ui->tableCDBS->setModel(mCdbsTable);
  ui->tableCDBS2->setModel(mCdbsTable);
  ui->tableImportedMaps->setModel(mMapTable);
  ui->tableMaps->setModel(mMapTable);
  ui->tableRef->setModel(mRefTable);
  //ui->tableCDBS->show();
}

MainWindow2::~MainWindow2(){
    delete ui;
}

void MainWindow2::loadButtonClicked(){
    emit loadHist(ui->editFileName->text(),ui->editImportName->text());
}

void MainWindow2::loadToolClicked(){
    QString tmp = ui->editFileName->text();
    if (tmp.isEmpty()) tmp = "/home/ehecht/git/CDBSAnalyzer/data";
    QFileDialog *dialog = new QFileDialog(this);
    dialog->setDirectory(tmp);
    tmp = dialog->getOpenFileName();
    ui->editFileName->setText(tmp);
}

void MainWindow2::computeButtonClicked(){
    double roiWidth,roiLength,binWidth;
    roiLength = ui->editRoiLength->text().toDouble();
    roiWidth = ui->editRoiWidth->text().toDouble();
    binWidth = ui->editBinWidth->text().toDouble();
    emit computeAll(roiWidth,roiLength,binWidth);
}

void MainWindow2::referenceButtonClicked(){
    QString refName = ui->comboRef->currentText();
    QStringList list;
    //TODO: Fill list
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
