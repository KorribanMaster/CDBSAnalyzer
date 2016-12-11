#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include "mpa1dhist.h"
#include "mpa2dhist.h"

#include <QMainWindow>

namespace Ui {
class PlotWindow;
}

class PlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlotWindow(QWidget *parent = 0);
    ~PlotWindow();
    void showHist(Mpa1dHist *hist);
    void showHist(Mpa2dHist *hist);

private:
    Ui::PlotWindow *ui;
};

#endif // PLOTWINDOW_H
