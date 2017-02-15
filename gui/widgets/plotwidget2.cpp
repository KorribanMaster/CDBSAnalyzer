#include "plotwidget2.h"
#include "ui_plotwidget2.h"

#include "qcustomplot.h"

PlotWidget2::PlotWidget2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotWidget2)
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
    connect(ui->typeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(changeView(int)));
    connect(ui->errorCheckBox,SIGNAL(toggled(bool)),this,SLOT(showErrors(bool)));
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(save()));
    mPlotCount = 0;
    mPlotType = -1;

}

PlotWidget2::~PlotWidget2()
{
    delete ui;
}

void PlotWidget2::addHist(MpaCdbHist *hist){
    //extract data
    if(mPlotCount>0 && mPlotType!=0){
        qDebug() << "You tried to mix different histogramm type in one Plot. Returning!";
        return;
    }
    mPlotType = 0;
    QVector<double> x(hist->mSize), y(hist->mSize), yError(hist->mSize), xFoldover(hist->mSize/2), yFoldover(hist->mSize/2), yFoldoverError(hist->mSize/2), yNorm(hist->mSize), yNormError(hist->mSize), yNormFoldover(hist->mSize/2), yNormFoldoverError(hist->mSize/2);
    for(int i=0; i<hist->mSize;i++){
        x[i] = hist->mEnergyScale(i);
        y[i] = hist->mProjectionHist(i);
        yError[i] = hist->mProjectionHistError(i);
        yNorm[i] = hist->mNormHist(i);
        yNormError[i] = hist->mNormHistError(i);
    }
    for (int i=0;i<hist->mSize/2;i++){
        xFoldover[i] = hist->mEnergyScaleFoldover(i);
        yFoldover[i] = hist->mFoldoverHist(i);
        yFoldoverError[i] = hist->mFoldoverHistError(i);
        yNormFoldover[i] = hist->mNormFoldoverHist(i);
        yNormFoldoverError[i] = hist->mNormFoldoverHistError(i);
    }
    mDataX.push_back(x);
    mDataY.push_back(y);
    mErrorsY.push_back(yError);
    mNormDataY.push_back(yNorm);
    mNormErrorsY.push_back(yNormError);
    mFoldoverDataX.push_back(xFoldover);
    mFoldoverDataY.push_back(yFoldover);
    mFoldoverErrorsY.push_back(yFoldoverError);
    mNormFoldoverDataY.push_back(yNormFoldover);
    mNormFoldoverErrorsY.push_back(yNormFoldoverError);

    int index = ui->customPlot->graphCount();
    QPen pen;
    pen.setColor(QColor(qSin(index*0.3)*100+100, qSin(index*0.6+0.7)*100+100, qSin(index*0.4+0.6)*100+100));
    ui->customPlot->addGraph();
    mPlotCount++;
    ui->customPlot->graph(index)->setData(x,yNorm);
    ui->customPlot->graph(index)->setPen(pen);
    ui->customPlot->graph(index)->setName(hist->mName);
    ui->customPlot->graph(index)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 4));
    QCPErrorBars *errorBars = new QCPErrorBars(ui->customPlot->xAxis, ui->customPlot->yAxis);
    errorBars->removeFromLegend();
    errorBars->setAntialiased(false);
    errorBars->setDataPlottable(ui->customPlot->graph(index));
    errorBars->setData(yNormError);
    mErrorBarPointer.push_back(errorBars);
    if(index==0){
        ui->customPlot->xAxis->setLabel("Energy");
        ui->customPlot->yAxis->setLabel("Counts");
        ui->customPlot->xAxis->setRange(x.first(),x.last());
        ui->customPlot->yAxis->setRange(0,hist->mNormHist.maxCoeff());

    }
    qDebug() << ui->customPlot->plottableCount();
    ui->customPlot->replot();
}

void PlotWidget2::addHist(MpaRefHist *hist){
    if(mPlotCount>0 && mPlotType!=1){
        qDebug() << "You tried to mix different histogramm type in one Plot. Returning!";
        return;
    }
    mPlotType = 1;
    //extract data
    QVector<double> x(hist->mSize), y(hist->mSize), yError(hist->mSize), xFoldover(hist->mSize/2), yFoldover(hist->mSize/2), yFoldoverError(hist->mSize/2), yNorm(hist->mSize), yNormError(hist->mSize), yNormFoldover(hist->mSize/2), yNormFoldoverError(hist->mSize/2);
    for(int i=0; i<hist->mSize;i++){
        x[i] = hist->mEnergyScale(i);
        yNorm[i] = hist->mRefHist(i);
        yNormError[i] = hist->mRefHistError(i);
    }
    for (int i=0;i<hist->mSize/2;i++){
        xFoldover[i] = hist->mFoldoverEnergyScale(i);
        yNormFoldover[i] = hist->mRefFoldoverHist(i);
        yNormFoldoverError[i] = hist->mRefFoldoverHistError(i);
    }
    mDataX.push_back(x);
    mNormDataY.push_back(yNorm);
    mNormErrorsY.push_back(yNormError);
    mFoldoverDataX.push_back(xFoldover);
    mNormFoldoverDataY.push_back(yNormFoldover);
    mNormFoldoverErrorsY.push_back(yNormFoldoverError);
    int index = ui->customPlot->graphCount();
    QPen pen;
    pen.setColor(QColor(qSin(index*0.3)*100+100, qSin(index*0.6+0.7)*100+100, qSin(index*0.4+0.6)*100+100));
    ui->customPlot->addGraph();
    ui->customPlot->graph(index)->setData(x,yNorm);
    ui->customPlot->graph(index)->setPen(pen);
    ui->customPlot->graph(index)->setName(hist->mName);
    QCPErrorBars *errorBars = new QCPErrorBars(ui->customPlot->xAxis, ui->customPlot->yAxis);
    errorBars->removeFromLegend();
    errorBars->setAntialiased(false);
    errorBars->setDataPlottable(ui->customPlot->graph(index));
    errorBars->setData(yNormError);
    mErrorBarPointer.push_back(errorBars);
    if(index==0){
        ui->customPlot->xAxis->setLabel("Energy");
        ui->customPlot->yAxis->setLabel("Counts");
        ui->customPlot->xAxis->setRange(x.first(),x.last());
        ui->customPlot->yAxis->setRange(0,hist->mRefHist.maxCoeff());
        ui->typeComboBox->removeItem(0);
        ui->typeComboBox->removeItem(1);

    }
    ui->customPlot->replot();
}

void PlotWidget2::selectionChanged()
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

void PlotWidget2::mousePress()
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

void PlotWidget2::mouseWheel()
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

void PlotWidget2::showErrors(bool state){
    for(int i=1;i<ui->customPlot->plottableCount();i+=2)
    {
        QCPAbstractPlottable *plt = ui->customPlot->plottable(i);
        plt->setVisible(state);
    }
    ui->customPlot->replot();

}

void PlotWidget2::changeView(int state){

    if (mPlotType ==1){
        switch(state){
        case 0: //norm

            for(int i=0;i<ui->customPlot->graphCount();i++)
            {
                QCPGraph *plt = ui->customPlot->graph(i);
                plt->setData(mDataX[i],mNormDataY[i]);
                mErrorBarPointer[i]->setData(mNormErrorsY[i]);
                ui->customPlot->xAxis->setRange(mDataX[i].takeFirst(),mDataX[i].takeLast());
                double max = *std::max_element(mNormDataY[i].begin(), mNormDataY[i].end());
                ui->customPlot->yAxis->setRange(0,max);

            }
            break;

        case 1: //norm foldover

            for(int i=0;i<ui->customPlot->graphCount();i++)
            {
                QCPGraph *plt = ui->customPlot->graph(i);
                plt->setData(mFoldoverDataX[i],mNormFoldoverDataY[i]);
                mErrorBarPointer[i]->setData(mNormFoldoverErrorsY[i]);
                ui->customPlot->xAxis->setRange(mFoldoverDataX[i].takeFirst(),mFoldoverDataX[i].takeLast());
                double max = *std::max_element(mNormFoldoverDataY[i].begin(), mNormFoldoverDataY[i].end());
                ui->customPlot->yAxis->setRange(0,max);


            }
            break;

    }
    }
    else if(mPlotType==0){
        switch (state) {
        case 0: //projection
            for(int i=0;i<ui->customPlot->graphCount();i++)
            {
                QCPGraph *plt = ui->customPlot->graph(i);
                plt->setData(mDataX[i],mDataY[i]);
                mErrorBarPointer[i]->setData(mErrorsY[i]);
                ui->customPlot->xAxis->setRange(mDataX[i].takeFirst(),mDataX[i].takeLast());
                double max = *std::max_element(mDataY[i].begin(), mDataY[i].end());
                ui->customPlot->yAxis->setRange(0,max);

            }
            break;
        case 1: //norm

            for(int i=0;i<ui->customPlot->graphCount();i++)
            {
                QCPGraph *plt = ui->customPlot->graph(i);
                plt->setData(mDataX[i],mNormDataY[i]);
                mErrorBarPointer[i]->setData(mNormErrorsY[i]);
                ui->customPlot->xAxis->setRange(mDataX[i].takeFirst(),mDataX[i].takeLast());
                double max = *std::max_element(mNormDataY[i].begin(), mNormDataY[i].end());
                ui->customPlot->yAxis->setRange(0,max);

            }
            break;
        case 2: //foldover

            for(int i=0;i<ui->customPlot->graphCount();i++)
            {
                QCPGraph *plt = ui->customPlot->graph(i);
                plt->setData(mFoldoverDataX[i],mFoldoverDataY[i]);
                mErrorBarPointer[i]->setData(mFoldoverErrorsY[i]);
                ui->customPlot->xAxis->setRange(mFoldoverDataX[i].takeFirst(),mFoldoverDataX[i].takeLast());
                double max = *std::max_element(mFoldoverDataY[i].begin(), mFoldoverDataY[i].end());
                ui->customPlot->yAxis->setRange(0,max);


            }
            break;
        case 3: //norm foldover

            for(int i=0;i<ui->customPlot->graphCount();i++)
            {
                QCPGraph *plt = ui->customPlot->graph(i);
                plt->setData(mFoldoverDataX[i],mNormFoldoverDataY[i]);
                mErrorBarPointer[i]->setData(mNormFoldoverErrorsY[i]);
                ui->customPlot->xAxis->setRange(mFoldoverDataX[i].takeFirst(),mFoldoverDataX[i].takeLast());
                double max = *std::max_element(mNormFoldoverDataY[i].begin(), mNormFoldoverDataY[i].end());
                ui->customPlot->yAxis->setRange(0,max);


            }
            break;

        default:
            break;
        }
    }
    ui->customPlot->replot();

}

void PlotWidget2::save(){
    QString str = QFileDialog::getSaveFileName(this,"Save Graph","/home/ehecht/git/CDBSAnalyzer/data");
    ui->customPlot->savePdf(str);
}
