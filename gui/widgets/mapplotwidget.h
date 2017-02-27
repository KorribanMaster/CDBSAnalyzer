#ifndef MAPPLOTWIDGET_H
#define MAPPLOTWIDGET_H

#include <QWidget>

#include <mpa2dhist.h>

namespace Ui {
class MapPlotWidget;
}

class MapPlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MapPlotWidget(QWidget *parent = 0);
    ~MapPlotWidget();
    void plotRaw(Mpa2dHist *hist);
    void plotMap(Mpa2dHist *hist);
    void addRoi(double roiWidth, double roiLength, double binWidth);

private:
    Ui::MapPlotWidget *ui;
};

#endif // MAPPLOTWIDGET_H
