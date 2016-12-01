#include "plotwindow.h"
#include "ui_plotwindow.h"

PlotWindow::PlotWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlotWindow)
{
    ui->setupUi(this);
    // generate some data:
    const double PI = 3.141592653589793;
    QVector<double> x(101), y(101), p(3); // initialize with entries 0..100
    p[0] = 1/(std::sqrt(0.2*2*PI));
    p[1] = 0;
    p[2] = std::sqrt(0.2);
    for (int i=0; i<101; ++i)
    {
      x[i] = (i/50.0 - 1)*5; // x goes from -5 to 5
      y[i] = (p[0]*std::exp(-(std::pow((x[i]-p[1]),2)/(2*pow(p[2],2))))); // let's plot a quadratic function
    }
    ui->customPlot->clearItems();
    ui->customPlot->clearPlottables();
    // create graph and assign data to it:
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setData(x, y);
    // give the axes some labels:
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    ui->customPlot->xAxis->setRange(-5, 5);
    ui->customPlot->yAxis->setRange(0, 1);
    ui->customPlot->replot();
}

PlotWindow::~PlotWindow()
{
    delete ui;
}
