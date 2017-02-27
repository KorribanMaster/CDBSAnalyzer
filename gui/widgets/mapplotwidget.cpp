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

void MapPlotWidget::plotRaw(Mpa2dHist *hist){

    hist->updateEnergyScale();
    // configure axis rect:
    ui->customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    ui->customPlot->axisRect()->setupFullAxesBox(true);
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");
    // set up the QCPColorMap:
    QCPColorMap *colorMap = new QCPColorMap(ui->customPlot->xAxis, ui->customPlot->yAxis);
    colorMap->data()->setSize(hist->mXSize,hist->mYSize);
    colorMap->data()->setRange(QCPRange(hist->mXEnergyscale.minCoeff(),hist->mXEnergyscale.maxCoeff()),QCPRange(hist->mYEnergyscale.minCoeff(),hist->mYEnergyscale.maxCoeff()));
    //colorMap->data()->setRange(QCPRange(0,1024),QCPRange(0,1024));
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
    //colorScale->axis()->setLabel("Magnetic Field Strength");

    // set the color gradient of the color map to one of the presets:
    colorMap->setGradient(QCPColorGradient::gpPolar);
    // we could have also created a QCPColorGradient instance and added own colors to
    // the gradient, see the documentation of QCPColorGradient for what's possible.

    // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
    colorMap->rescaleDataRange();
    //QCPItemTracer *centerPoint = new QCPItemTracer(ui->customPlot);



    // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
    QCPMarginGroup *marginGroup = new QCPMarginGroup(ui->customPlot);
    ui->customPlot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    // rescale the key (x) and value (y) axes so the whole color map is visible:
    ui->customPlot->rescaleAxes();
    //Add Roi Border
    for(int i=0;i<4;i++){
        QCPItemLine *line = new QCPItemLine(ui->customPlot);
        int next = i+1;
        if (next == 4){
            next = 0;
        }
        line->start->setCoords(hist->mRoiBorder->mCorners[i](0),hist->mRoiBorder->mCorners[i](1));
        line->end->setCoords(hist->mRoiBorder->mCorners[next](0),hist->mRoiBorder->mCorners[next](1));
    }
    //Add Roi Segments
    for(int j=0;j<hist->mRoiGrid.size();j++){
        for(int i=0;i<4;i++){
            QCPItemLine *line = new QCPItemLine(ui->customPlot);
            int next = i+1;
            if (next == 4){
                next = 0;
            }
            line->start->setCoords(hist->mRoiGrid[j]->mCorners[i](0),hist->mRoiGrid[j]->mCorners[i](1));
            line->end->setCoords(hist->mRoiGrid[j]->mCorners[next](0),hist->mRoiGrid[j]->mCorners[next](1));
        }

    }
    //mark center
    QCPItemLine *crossX = new QCPItemLine(ui->customPlot);
    crossX->start->setCoords(511e3,510.8e3);
    crossX->end->setCoords(511e3,511.2e3);
    QCPItemLine *crossy = new QCPItemLine(ui->customPlot);
    crossy->start->setCoords(510.8e3,511e3);
    crossy->end->setCoords(511.2e3,511e3);
    ui->customPlot->xAxis->setRange(QCPRange(500e3,520e3));
    ui->customPlot->yAxis->setRange(QCPRange(500e3,520e3));



}

void MapPlotWidget::plotMap(Mpa2dHist *hist){
    if(!hist->mMapInitialised) return;
    //hist->updateEnergyScale();
    // configure axis rect:
    ui->customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    ui->customPlot->axisRect()->setupFullAxesBox(true);
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");
    // set up the QCPColorMap:
    QCPColorMap *colorMap = new QCPColorMap(ui->customPlot->xAxis, ui->customPlot->yAxis);
    colorMap->data()->setSize(hist->mCenteredHist.cols(),hist->mCenteredHist.rows());
    qDebug()<< hist->mEnergyMap[0]->mCenter(0)<<hist->mEnergyMap[hist->mCenteredHist.cols()-1]->mCenter(0) << hist->mEnergyMap[hist->mCenteredHist.cols()-1]->mCenter(1) << hist->mEnergyMap[0]->mCenter(1);
    colorMap->data()->setRange(QCPRange(hist->mEnergyMap[0]->mCenter(0),hist->mEnergyMap[hist->mCenteredHist.cols()-1]->mCenter(0)),QCPRange(hist->mEnergyMap[0]->mCenter(1),hist->mEnergyMap[std::pow(hist->mCenteredHist.cols()-1,2)]->mCenter(1)));
    //colorMap->data()->setRange(QCPRange(0,1024),QCPRange(0,1024));
    for(int i = 0;i<800*800;i++){
        int xIndex,yIndex;
        colorMap->data()->coordToCell(hist->mEnergyMap[i]->mCenter(0),hist->mEnergyMap[i]->mCenter(1),&xIndex,&yIndex);
        colorMap->data()->setCell(xIndex,yIndex,hist->mEnergyMap[i]->mCounts);
    }

    // add a color scale:
    QCPColorScale *colorScale = new QCPColorScale(ui->customPlot);
    ui->customPlot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect
    colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
    colorMap->setColorScale(colorScale); // associate the color map with the color scale
    //colorScale->axis()->setLabel("Magnetic Field Strength");

    // set the color gradient of the color map to one of the presets:
    colorMap->setGradient(QCPColorGradient::gpPolar);
    // we could have also created a QCPColorGradient instance and added own colors to
    // the gradient, see the documentation of QCPColorGradient for what's possible.

    // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
    colorMap->rescaleDataRange();
    //QCPItemTracer *centerPoint = new QCPItemTracer(ui->customPlot);



    // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
    QCPMarginGroup *marginGroup = new QCPMarginGroup(ui->customPlot);
    ui->customPlot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    // rescale the key (x) and value (y) axes so the whole color map is visible:
    ui->customPlot->rescaleAxes();
    //Add Roi Border
    for(int i=0;i<4;i++){
        QCPItemLine *line = new QCPItemLine(ui->customPlot);
        int next = i+1;
        if (next == 4){
            next = 0;
        }
        line->start->setCoords(hist->mRoiBorder->mCorners[i](0),hist->mRoiBorder->mCorners[i](1));
        line->end->setCoords(hist->mRoiBorder->mCorners[next](0),hist->mRoiBorder->mCorners[next](1));
    }
    //Add Roi Segments
    for(int j=0;j<hist->mRoiGrid.size();j++){
        for(int i=0;i<4;i++){
            QCPItemLine *line = new QCPItemLine(ui->customPlot);
            int next = i+1;
            if (next == 4){
                next = 0;
            }
            line->start->setCoords(hist->mRoiGrid[j]->mCorners[i](0),hist->mRoiGrid[j]->mCorners[i](1));
            line->end->setCoords(hist->mRoiGrid[j]->mCorners[next](0),hist->mRoiGrid[j]->mCorners[next](1));
        }

    }
    //mark center
    QCPItemLine *crossX = new QCPItemLine(ui->customPlot);
    crossX->start->setCoords(511e3,510.8e3);
    crossX->end->setCoords(511e3,511.2e3);
    QCPItemLine *crossy = new QCPItemLine(ui->customPlot);
    crossy->start->setCoords(510.8e3,511e3);
    crossy->end->setCoords(511.2e3,511e3);
    //ui->customPlot->xAxis->setRange(QCPRange(500e3,520e3));
    //ui->customPlot->yAxis->setRange(QCPRange(500e3,520e3));



}
void MapPlotWidget::addRoi(double roiWidth, double roiLength, double binWidth){

    QCPItemLine *line = new QCPItemLine(ui->customPlot);
    line->start->setCoords(510e3,510e3);
    line->end->setCoords(520e3,520e3);

}
