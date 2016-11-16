#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>

#include "mpahistfile.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_loadButton_clicked();

    void on_saveButton_clicked();

    void on_mpaLoadTool_clicked();

    void on_saveToolButton_clicked();

    void on_computeButton_clicked();

    void on_settingsToolButton_clicked();

private:
    Ui::MainWindow *ui;
    MpaHistFile *mMpaHist;
    QString mMpaFileName;
    QSettings *mExpSettings;

};

#endif // MAINWINDOW_H
