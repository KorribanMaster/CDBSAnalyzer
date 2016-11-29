#include <QDebug>

#include <QtCore>
#include <QFile>
#include <QTextStream>

#include "mpa2dhist.h"
#include "mpa1dhist.h"



int main(){
    int size = 400;
    Mpa2dHist *hist = new Mpa2dHist("hist",size,size);
    Mpa1dHist *projection;
    hist->setCenter(size/2,size/2);
    hist->setCalibration(60,60);
    hist->setRoi(200,1000);
    hist->setEnergyBinWidth(100);
    for(int i=0;i<size;i++){
       for(int j=0;j<size;j++){
           hist->setBinContent(i,j,1);
       }
    }
    hist->findCenter();
    hist->updateMap();
    hist->updateRoi();
    projection = hist->projectCDBS();
    /*std::vector<CdbPixel*> map = hist->mEnergyMap;
    QFile mapFile("../data/map.txt");
    if(!mapFile.open(QIODevice::ReadWrite)){
        qDebug() << "Failed to open file";
    }
    QTextStream out(&mapFile);
    for (int i =0;i<map.size();i++){
        CdbPixel *px = map[i];
        for(int n=0;n<4;n++){
            out << px->mCorners[n](0) << "; " << px->mCorners[n](1) << "\n";
        }
        out.flush();
    }
    mapFile.close();
    std::vector<RoiPixel*> roi = hist->mRoiGrid;
    roi.push_back(hist->mRoiBorder);
    QFile roiFile("../data/roi.txt");
    if(!roiFile.open(QIODevice::ReadWrite)){
        qDebug() << "Failed to open file";
    }
    QTextStream out2(&roiFile);
    for (int i =0;i<roi.size();i++){
        RoiPixel *px = roi[i];
        for(int n=0;n<4;n++){
            out2 << px->mCorners[n](0) << "; " << px->mCorners[n](1) << "\n";
        }
    }
    roiFile.close();
    std::vector<CdbPixel*> filtered = hist->mEnergyMapFiltered;
    QFile filteredFile("../data/filtered.txt");
    if(!filteredFile.open(QIODevice::ReadWrite)){
        qDebug() << "Failed to open file";
    }
    QTextStream out3(&filteredFile);
    for (int i =0;i<filtered.size();i++){
        CdbPixel *px = filtered[i];
        for(int n=0;n<4;n++){
            out3 << px->mCorners[n](0) << "; " << px->mCorners[n](1) << "\n";
        }
    }
    filteredFile.close();
    */

}
