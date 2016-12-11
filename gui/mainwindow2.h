#ifndef MAINWINDOW2_H
#define MAINWINDOW2_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QThread>

#include "histmanager.h"
#include "histtable.h"
#include "plotwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow2 : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow2(QWidget *parent = 0);
    ~MainWindow2();
private:
    Ui::MainWindow *ui;
    HistManager *mManager;
    HistTable *mCdbsTable;
    HistTable *mRefTable;
    HistTable *mMapTable;
    HistTable *mImportTable;
    PlotWidget *mPlot;

signals:
    void loadHist(QString fileName,QString name);
    void loadSettings(QString fileName);
    void referenceHist(QString refName,QStringList histNames);
    void computeAll(double roiWidth,double roiLength,double binWidth);

public slots:
    void loadButtonClicked();
    void loadToolClicked();
    void computeButtonClicked();
    void referenceButtonClicked();
    void importNameEdited();
    void fileNameEdited();
};

#endif // MAINWINDOW2_H
