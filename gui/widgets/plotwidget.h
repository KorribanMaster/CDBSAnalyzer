#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>

#include "mpa1dhist.h"
#include "mparefhist.h"
#include "mpacdbhist.h"

namespace Ui {
class PlotWidget;
}

class PlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlotWidget(QWidget *parent = 0);
    ~PlotWidget();
    void addHist(Mpa1dHist *hist);
    void addHist(MpaCdbHist *hist);
    void addHist(MpaRefHist *hist);

    void mouseWheel();
    void mousePress();
    void selectionChanged();


private:
    Ui::PlotWidget *ui;
};

#endif // PLOTWIDGET_H
