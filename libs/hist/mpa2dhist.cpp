#include "mpa2dhist.h"

#include <unsupported/Eigen/NonLinearOptimization>
#include <unsupported/Eigen/NumericalDiff>

#include <QDebug>
#include <QtConcurrent>

template<typename _Scalar, int NX=Eigen::Dynamic, int NY=Eigen::Dynamic>
struct Functor
{
  typedef _Scalar Scalar;
  enum {
    InputsAtCompileTime = NX,
    ValuesAtCompileTime = NY
  };
  typedef Eigen::Matrix<Scalar,InputsAtCompileTime,1> InputType;
  typedef Eigen::Matrix<Scalar,ValuesAtCompileTime,1> ValueType;
  typedef Eigen::Matrix<Scalar,ValuesAtCompileTime,InputsAtCompileTime> JacobianType;

  const int m_inputs, m_values;

  Functor() : m_inputs(InputsAtCompileTime), m_values(ValuesAtCompileTime) {}
  Functor(int inputs, int values) : m_inputs(inputs), m_values(values) {}

  int inputs() const { return m_inputs; }
  int values() const { return m_values; }

  // you should define that in the subclass :
//  void operator() (const InputType& x, ValueType* v, JacobianType* _j=0) const;
};

struct lmdif_gauss_functor : Functor<double>
{
    const int mYSize;
    Eigen::VectorXd mY;
    lmdif_gauss_functor(const Eigen::VectorXd &y,int ysize) : Functor<double>(3,ysize), mYSize(ysize),mY(y) {}
    int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
    {
        assert(x.size()==3);
        assert(fvec.size()==mYSize);
        for(int i=0;i<mYSize;i++){
            //implement error of gaussian
            fvec(i) = mY(i) - (x(0)*std::exp(-(std::pow((i-x(1)),2)/(2*std::pow(x(2),2)))));
        }
        return 0;
    }
};

Mpa2dHist::Mpa2dHist(QString name)
{
    setSize(0,0);
    mName = name;
    mMaxDepth = 8;
    mEnergyCenter = Eigen::Array2d(511,511);
    mCenteredHistSize = 800;
    mCdbCounter = 0;

}

Mpa2dHist::Mpa2dHist(QString name, int xbins, int ybins){
    setSize(xbins,ybins);
    mName = name;
    mMaxDepth =8;
    mEnergyCenter = Eigen::Array2d(511,511);
    mCenteredHistSize = 800;
    mCdbCounter = 0;


}

Mpa2dHist::~Mpa2dHist(){
    mRoiGrid.clear();
    mEnergyMap.clear();
    mEnergyMapFiltered.clear();
    delete mRoiBorder;
}

void Mpa2dHist::setSize(int xbins, int ybins){
    mXSize = xbins;
    mYSize = ybins;
    mRawHist.resize(xbins,ybins);
    mXEnergyscale.resize(xbins);
    mYEnergyscale.resize(ybins);

}



void Mpa2dHist::setBinContent(int xbin, int ybin,int counts){
    mRawHist(xbin,ybin) = counts;
}

void Mpa2dHist::setRoi(double roiWidth, double roiLength){
    mRoiWidth = roiWidth;
    mRoiLength = roiLength;

}

void Mpa2dHist::setEnergyBinWidth(double binWidth){
    mEnergyBinWidth = binWidth;
}

void Mpa2dHist::setCalibration(float xcal,float ycal){
    mCal(0)=xcal;
    mCal(1)= ycal;
}

void Mpa2dHist::setCenter(float xcenter, float ycenter){
    //This is a temporary solution with a fixed centerpoint
    mCenter(0)=xcenter;
    mCenter(1)= ycenter;
}

void Mpa2dHist::findCenter(){

    double ycenter=0;
    double xcenter=0;
    //calculate cuts in y direction
    for(int i=0;i<4;i++){
        Eigen::MatrixXd tmp = mRawHist.block<1024,100>(0,i*100);
        Eigen::VectorXd cut = tmp.rowwise().sum();
        Eigen::VectorXd x(3);
        x << cut.maxCoeff(), cut.size()/2 , 50;
        lmdif_gauss_functor functor(cut,cut.size());
        Eigen::NumericalDiff<lmdif_gauss_functor> numDiff(functor);
        Eigen::LevenbergMarquardt<Eigen::NumericalDiff<lmdif_gauss_functor>,double> lm(numDiff);
        lm.parameters.maxfev = 2000;
        lm.parameters.xtol = 1.0e-10;
        int ret = lm.minimize(x);
        // check return values
        if (ret !=1){
            qDebug() << "Fitting unsuccesfull";
        }
        ycenter += x(1);
    }
    //calculate cuts in x direction
    for(int i=0;i<4;i++){
        Eigen::MatrixXd tmp = mRawHist.block<100,1024>((i+1)*100,0);
        Eigen::VectorXd cut = tmp.colwise().sum().transpose();
        Eigen::VectorXd x(3);
        x << cut.maxCoeff(), cut.size()/2 , 50;
        lmdif_gauss_functor functor(cut,cut.size());
        Eigen::NumericalDiff<lmdif_gauss_functor> numDiff(functor);
        Eigen::LevenbergMarquardt<Eigen::NumericalDiff<lmdif_gauss_functor>,double> lm(numDiff);
        lm.parameters.maxfev = 2000;
        lm.parameters.xtol = 1.0e-10;
        int ret = lm.minimize(x);
        // check return values
        if (ret !=1){
            qDebug() << "Fitting unsuccesfull";
        }
        xcenter += x(1);
    }
    mCenter(0) = xcenter/4;
    mCenter(1) = ycenter/4;
    mCenteredHist = mRawHist.block<800,800>(std::round(mCenter(1))-400,std::round(mCenter(0))-400); //Commented for testing reasons
    //mCenteredHist = mRawHist;
}

void Mpa2dHist::centerHist(){
    mCenteredHist = mRawHist.block<800,800>(std::round(mCenter(1))-400,std::round(mCenter(0))-400);
}

void Mpa2dHist::updateRoi(){
    mRoiGrid.clear();
    //Roi Border points. See drawing in lab report.
    Eigen::Vector2d p1;
    Eigen::Vector2d p2;
    Eigen::Vector2d c1;
    Eigen::Vector2d c2;
    std::vector<Eigen::Vector2d> corners(4);
    Eigen::Vector2d v1(1/std::sqrt(2),-1/std::sqrt(2));
    Eigen::Vector2d v2(1/std::sqrt(2),1/std::sqrt(2));
    Eigen::Vector2d v3(1/std::sqrt(2),-1/std::sqrt(2));
    v1 = v1*mRoiLength;
    v2 = v2*mRoiWidth;
    v3 = v3*mEnergyBinWidth;
    p1 = mEnergyCenter.matrix()*1e3-v1;
    p2 = mEnergyCenter.matrix()*1e3+v1;
    corners[0] = p1-v2;
    corners[1] = p1+v2;
    corners[2] = p2+v2;
    corners[3] = p2-v2;
    mRoiBorder = new RoiPixel(corners,mDepth);

    //RoiPixels. Assume the binWidth*n=roiLength
    //should be generalized
    c1 = corners[0];
    c2 = corners[1];
    for(int i=0;i<mRoiLength/mEnergyBinWidth*2;i++){
       corners[0] = c1+i*v3;
       corners[1] = c2+i*v3;
       corners[2] = c2+(i+1)*v3;
       corners[3] = c1+(i+1)*v3;
       RoiPixel *px = new RoiPixel(corners,mDepth);
       mRoiGrid.push_back(px);
    }

}

void Mpa2dHist::updateMap(){
    mEnergyMap.clear();
    Eigen::Array2d leftTop;
    leftTop(0) = mEnergyCenter(0)*1e3-(mCenteredHist.rows()/2)*mCal(0);
    leftTop(1) = mEnergyCenter(1)*1e3+(mCenteredHist.cols()/2)*mCal(1);
    for(int y=0;y<mCenteredHist.cols();y++){
       for(int x=0;x<mCenteredHist.rows();x++){
           Eigen::Array2d center(x,y);
           Eigen::Array2d tmp;
           Eigen::Array2d odd;
           std::vector<Eigen::Array2d> corners;
           center(0) = leftTop(0)+x*mCal(0);
           center(1) = leftTop(1)-y*mCal(0);
           odd << -0.5,0.5;
           tmp = center-0.5*mCal;
           corners.push_back(tmp);
           tmp = center+odd*mCal;
           corners.push_back(tmp);
           tmp = center+0.5*mCal;
           corners.push_back(tmp);
           tmp = center-odd*mCal;
           corners.push_back(tmp);
           CdbPixel *px = new CdbPixel(corners,mCenteredHist(y,x));
           mEnergyMap.push_back(px);
       }
    }

}

MpaCdbHist* Mpa2dHist::projectCDBS(){
    //findCenter();
    centerHist();
    updateMap();
    updateRoi();
    QString histName = mName + QString("_CDBS%1").arg(mCdbCounter);
    mCdbCounter++;
    MpaCdbHist *projection = new MpaCdbHist(histName);
    projection->setSize(mRoiGrid.size());
    std::vector<CdbPixel*> remaining;
    //remaining.reserve(mEnergyMap.size());
    for(int i= 0;i<mEnergyMap.size();i++){
        CdbPixel *px = mEnergyMap[i];
        int nInside = px->inside(mRoiBorder);
        nInside += mRoiBorder->inside(px);
        if (nInside > 0){
            remaining.push_back(mEnergyMap[i]);
        }
    }
    mEnergyMapFiltered = remaining;
    std::vector<QFuture<void>> futures(mRoiGrid.size());
    for (int i=0;i<mRoiGrid.size();i++){
        QFuture<void> future = QtConcurrent::run(mRoiGrid[i],&RoiPixel::getContent,remaining,0);
        futures[i] = future;

    }
    for (int i=0;i<mRoiGrid.size();i++){
        futures[i].waitForFinished(); //wait till computation has finished;
        projection->setBinContent(i,mRoiGrid[i]->content());
    }
    projection->setCalibration(mEnergyBinWidth,511e3,mRoiGrid.size()/2);
    projection->setRoiInformation(mRoiLength,mRoiWidth,mEnergyBinWidth);
    projection->calculateFoldover();
    return projection;
}



MpaCdbHist* Mpa2dHist::projectCDBS(double roiWidth, double roiLength, double binWidth, int depth){
    setDepth(depth);
    setRoi(roiWidth,roiLength);
    setEnergyBinWidth(binWidth);
    MpaCdbHist *projection = projectCDBS();
    return projection;

}

void Mpa2dHist::setDepth(int depth){
    mDepth = depth;
}
