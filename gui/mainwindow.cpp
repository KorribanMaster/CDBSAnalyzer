#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "plotwindow.h"
#include "mparefhist.h"
#include <QFileDialog>
#include <QSaveFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete mMpaHist;
    delete ui;
}

void MainWindow::on_loadButton_clicked()
{
    mMpaFileName = ui->mpaFileEdit->text();
    QString settingsFileName  = ui->settingsEdit->text();
    mMpaHist = new MpaHistFile(mMpaFileName,settingsFileName,"default");
    mMpaHist->loadFile();


}

void MainWindow::on_saveButton_clicked()
{
    QFile mapFile(ui->outputFileEdit->text());
    if(!mapFile.open(QIODevice::ReadWrite)){
        qDebug() << "Failed to open file";
    }
    QTextStream out(&mapFile);
    for(int i=0;i<mMpaHist->mCdbHists.size();i++){
        MpaCdbHist *hist = mMpaHist->mCdbHists.at(i);
        out << hist->mName << "\n";
        for(int n=0;n<hist->mProjectionHist.size();n++){
            out << hist->mEnergyScale(n) <<"; " << hist->mProjectionHist(n) << "\n";
        }

    }
    mapFile.close();
}


void MainWindow::on_mpaLoadTool_clicked()
{
    QFileDialog *dial = new QFileDialog(this);
    dial->setDirectory("../data");
    QString name = dial->getOpenFileName();
    ui->mpaFileEdit->setText(name);

}

void MainWindow::on_saveToolButton_clicked()
{
    QString name = QFileDialog::getSaveFileName(this);
    ui->outputFileEdit->setText(name);

}

void MainWindow::on_computeButton_clicked()
{
    double roiWidth = ui->roiWidthEdit->text().toDouble();
    double roiLength = ui->roiLengthEdit->text().toDouble();
    double binWidth = ui->binWidthEdit->text().toDouble();
    mMpaHist->computeCdbHists(roiWidth,roiLength,binWidth);
    //plot CDBs
    for(int i=0; i<mMpaHist->mCdbHists.size();i++){
        PlotWindow w(this);
        //w.showHist(mMpaHist->mCdbHists.at(i));
        //w.show();
    }


}

void MainWindow::on_settingsToolButton_clicked()
{
    QFileDialog *dial = new QFileDialog(this);
    dial->setDirectory("../settings");
    QString name = dial->getOpenFileName(this);
    ui->settingsEdit->setText(name);


}
