#ifndef PLOTWIDGET2_H
#define PLOTWIDGET2_H

#include <QWidget>

#include "qcustomplot.h"

#include "mpa1dhist.h"
#include "mparefhist.h"
#include "mpacdbhist.h"

namespace Ui {
class PlotWidget2;
}

class PlotWidget2 : public QWidget
{
    Q_OBJECT

public:
    explicit PlotWidget2(QWidget *parent = 0);
    ~PlotWidget2();
    void addHist(MpaCdbHist *hist);
    void addHist(MpaRefHist *hist);

public slots:
    void mouseWheel();
    void mousePress();
    void selectionChanged();
    void showErrors(bool state);
    void changeView(int state);
    void save();


private:
    Ui::PlotWidget2 *ui;
    int mActiveView;
    int mPlotCount;
    bool mShowError;
    int mPlotType; //0 = CDBS 1 = REF
    QVector<QVector<double>> mDataX;
    QVector<QVector<double>> mDataY;
    QVector<QVector<double>> mErrorsY;
    QVector<QVector<double>> mNormErrorsY;
    QVector<QVector<double>> mNormDataY;
    QVector<QVector<double>> mFoldoverDataX;
    QVector<QVector<double>> mFoldoverErrorsY;
    QVector<QVector<double>> mFoldoverDataY;
    QVector<QVector<double>> mNormFoldoverErrorsY;
    QVector<QVector<double>> mNormFoldoverDataY;
    QVector<QCPErrorBars*> mErrorBarPointer;
};

#endif // PLOTWIDGET_H
