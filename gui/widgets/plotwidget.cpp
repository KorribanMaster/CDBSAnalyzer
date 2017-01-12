#include "plotwidget.h"
#include "ui_plotwidget.h"

#include "qcustomplot.h"

PlotWidget::PlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotWidget)
{
    ui->setupUi(this);
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                    QCP::iSelectLegend | QCP::iSelectPlottables);
    ui->customPlot->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);

    ui->customPlot->legend->setFont(legendFont);
    ui->customPlot->legend->setSelectedFont(legendFont);
    ui->customPlot->legend->setSelectableParts(QCPLegend::spItems);
    // connect slot that ties some axis selections together (especially opposite axes):
    connect(ui->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    // make bottom and left axes transfer their ranges to top and right axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

}

PlotWidget::~PlotWidget()
{
    delete ui;
}

void PlotWidget::addHist(Mpa1dHist *hist){
    //extract data
    QVector<double> x(hist->mSize), y(hist->mSize);
    for(int i=0; i<hist->mSize;i++){
        x[i] = hist->mEnergyScale(i);
        y[i] = hist->mRawHist(i);
    }
    int index = ui->customPlot->graphCount();
    QPen pen;
    pen.setColor(QColor(qSin(index*0.3)*100+100, qSin(index*0.6+0.7)*100+100, qSin(index*0.4+0.6)*100+100));
    ui->customPlot->addGraph();
    ui->customPlot->graph(index)->setData(x,y);
    ui->customPlot->graph(index)->setPen(pen);
    ui->customPlot->graph(index)->setName(hist->mName);
    if(index==0){
        ui->customPlot->xAxis->setLabel("Energy");
        ui->customPlot->yAxis->setLabel("Counts");
        ui->customPlot->xAxis->setRange(x.first(),x.last());
        ui->customPlot->yAxis->setRange(0,hist->mRawHist.maxCoeff());
        ui->customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
        ui->customPlot->yAxis2->setScaleType(QCPAxis::stLogarithmic);

    }
    ui->customPlot->replot();
}

void PlotWidget::addHist(MpaCdbHist *hist){
    //extract data
    QVector<double> x(hist->mSize), y(hist->mSize);
    for(int i=0; i<hist->mSize;i++){
        x[i] = hist->mEnergyScale(i);
        y[i] = hist->mNormHist(i);
    }
    int index = ui->customPlot->graphCount();
    QPen pen;
    pen.setColor(QColor(qSin(index*0.3)*100+100, qSin(index*0.6+0.7)*100+100, qSin(index*0.4+0.6)*100+100));
    ui->customPlot->addGraph();
    ui->customPlot->graph(index)->setData(x,y);
    ui->customPlot->graph(index)->setPen(pen);
    ui->customPlot->graph(index)->setName(hist->mName);
    if(index==0){
        ui->customPlot->xAxis->setLabel("Energy");
        ui->customPlot->yAxis->setLabel("Counts");
        ui->customPlot->xAxis->setRange(x.first(),x.last());
        ui->customPlot->yAxis->setRange(0,hist->mNormHist.maxCoeff());
    }
    ui->customPlot->replot();
}

void PlotWidget::addHist(MpaRefHist *hist){
    //extract data
    QVector<double> x(hist->mSize), y(hist->mSize);
    for(int i=0; i<hist->mSize;i++){
        x[i] = hist->mEnergyScale(i);
        y[i] = hist->mRefHist(i);
    }
    int index = ui->customPlot->graphCount();
    QPen pen;
    pen.setColor(QColor(qSin(index*0.3)*100+100, qSin(index*0.6+0.7)*100+100, qSin(index*0.4+0.6)*100+100));
    ui->customPlot->addGraph();
    ui->customPlot->graph(index)->setData(x,y);
    ui->customPlot->graph(index)->setPen(pen);
    ui->customPlot->graph(index)->setName(hist->mName);
    if(index==0){
        ui->customPlot->xAxis->setLabel("Energy");
        ui->customPlot->yAxis->setLabel("Counts");
        ui->customPlot->xAxis->setRange(x.first(),x.last());
        ui->customPlot->yAxis->setRange(0,hist->mRefHist.maxCoeff());
    }
    ui->customPlot->replot();
}

void PlotWidget::selectionChanged()
{
  /*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.

   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.

   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */

  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }


}

void PlotWidget::mousePress()
{
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged

  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->xAxis->orientation());
  else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->yAxis->orientation());
  else
    ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void PlotWidget::mouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed

  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->xAxis->orientation());
  else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->yAxis->orientation());
  else
    ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}
