#include "mainwindow.h"
#include "ui_mainwindow.h"
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
    delete ui;
}

void MainWindow::on_loadButton_clicked()
{
    mMpaFileName = ui->mpaFileEdit->text();
    QString settingsFileName  = ui->settingsEdit->text();
    mMpaHist = new MpaHistFile(mMpaFileName,settingsFileName,"default");


}

void MainWindow::on_saveButton_clicked()
{

}

void MainWindow::on_mpaLoadTool_clicked()
{
    QString name = QFileDialog::getOpenFileName(this);
    ui->mpaFileEdit->setText(name);

}

void MainWindow::on_saveToolButton_clicked()
{
    QString name = QFileDialog::getSaveFileName(this);

}

void MainWindow::on_computeButton_clicked()
{

}

void MainWindow::on_settingsToolButton_clicked()
{

}
