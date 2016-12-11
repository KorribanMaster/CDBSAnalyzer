#include "mapplotwidget.h"
#include "ui_mapplotwidget.h"

MapPlotWidget::MapPlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapPlotWidget)
{
    ui->setupUi(this);
}

MapPlotWidget::~MapPlotWidget()
{
    delete ui;
}

void MapPlotWidget::addHist(Mpa2dHist *hist){
    // configure axis rect:
    ui->customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    ui->customPlot->axisRect()->setupFullAxesBox(true);
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");
    // set up the QCPColorMap:
    QCPColorMap *colorMap = new QCPColorMap(ui->customPlot->xAxis, ui->customPlot->yAxis);
    colorMap->data()->setSize(hist->mXSize,hist->mYSize);
    //colorMap->data()->setRange(QCPRange(hist->mXEnergyscale(0),hist->mXEnergyscale(hist->mXSize-1)),QCPRange(hist->mYEnergyscale(0),hist->mYEnergyscale(hist->mYSize-1)));
    colorMap->data()->setRange(QCPRange(0,hist->mXSize),QCPRange(0,hist->mYSize));
    for (int xIndex=0; xIndex<hist->mXSize; ++xIndex)
    {
      for (int yIndex=0; yIndex<hist->mYSize; ++yIndex){
          colorMap->data()->setCell(xIndex,yIndex,hist->mRawHist(yIndex,xIndex));
      }
    }
    // add a color scale:
    QCPColorScale *colorScale = new QCPColorScale(ui->customPlot);
    ui->customPlot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect
    colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
    colorMap->setColorScale(colorScale); // associate the color map with the color scale
    colorScale->axis()->setLabel("Magnetic Field Strength");

    // set the color gradient of the color map to one of the presets:
    colorMap->setGradient(QCPColorGradient::gpPolar);
    // we could have also created a QCPColorGradient instance and added own colors to
    // the gradient, see the documentation of QCPColorGradient for what's possible.

    // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
    colorMap->rescaleDataRange();

    // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
    QCPMarginGroup *marginGroup = new QCPMarginGroup(ui->customPlot);
    ui->customPlot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    // rescale the key (x) and value (y) axes so the whole color map is visible:
    ui->customPlot->rescaleAxes();
}
