#include "plotwidget.h"
#include "ui_plotwidget.h"

#include "qcustomplot.h"

PlotWidget::PlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotWidget)
{
    ui->setupUi(this);
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
    ui->customPlot->addGraph();
    ui->customPlot->graph(index)->setData(x,y);
    if(index==0){
        ui->customPlot->xAxis->setLabel("Energy");
        ui->customPlot->yAxis->setLabel("Counts");
        ui->customPlot->xAxis->setRange(x.first(),x.last());
        ui->customPlot->yAxis->setRange(0,hist->mRawHist.maxCoeff());
    }
    ui->customPlot->replot();
}

void PlotWidget::addHist(MpaCdbHist *hist){
    //extract data
    QVector<double> x(hist->mSize), y(hist->mSize);
    for(int i=0; i<hist->mSize;i++){
        x[i] = hist->mEnergyScale(i);
        y[i] = hist->mProjectionHist(i);
    }
    int index = ui->customPlot->graphCount();
    ui->customPlot->addGraph();
    ui->customPlot->graph(index)->setData(x,y);
    if(index==0){
        ui->customPlot->xAxis->setLabel("Energy");
        ui->customPlot->yAxis->setLabel("Counts");
        ui->customPlot->xAxis->setRange(x.first(),x.last());
        ui->customPlot->yAxis->setRange(0,hist->mProjectionHist.maxCoeff());
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
    ui->customPlot->addGraph();
    ui->customPlot->graph(index)->setData(x,y);
    if(index==0){
        ui->customPlot->xAxis->setLabel("Energy");
        ui->customPlot->yAxis->setLabel("Counts");
        ui->customPlot->xAxis->setRange(x.first(),x.last());
        ui->customPlot->yAxis->setRange(0,hist->mRefHist.maxCoeff());
    }
    ui->customPlot->replot();
}


