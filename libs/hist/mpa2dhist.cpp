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

struct lmdif_gauss2d_functor : Functor<double>
{
    const int mYSize;
    Eigen::VectorXd mY;
    lmdif_gauss2d_functor(const Eigen::VectorXd &y,int ysize) : Functor<double>(6,ysize), mYSize(ysize),mY(y) {}
    int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
    {
        assert(x.size()==6);
        assert(fvec.size()==mYSize);
        for(int i=0;i<std::sqrt(mYSize);i++){
            for(int j=0;j<std::sqrt(mYSize);j++){
                //implement error of gaussian
                int sum = i*std::sqrt(mYSize)+j;
                fvec(sum) = mY(sum) - ((x(0)*std::exp(-(std::pow((i-x(1)),2)/(2*std::pow(x(2),2)))))+(x(3)*std::exp(-(std::pow((j-x(4)),2)/(2*std::pow(x(5),2))))));
            }

        }
        return 0;
    }
};

struct lmdif_gauss2dex_functor : Functor<double>
{
    Eigen::VectorXd mZ;
    lmdif_gauss2dex_functor(const Eigen::VectorXd &z,int zsize) : Functor<double>(6,zsize),mZ(z) {}
    int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
    {
        assert(x.size()==6);
        int size=std::sqrt(mZ.size());
        // parameters are: [Amplitude, x0, sigmax, y0, sigmay, angel(in rad)]
        Eigen::VectorXd xrow,ycol;
        xrow.setLinSpaced(size,1,size);
        ycol.setLinSpaced(size,size,1);
        for(int i=0;i<size;i++){
            xrow(i) = xrow(i)*std::cos(x(5))-xrow(i)*std::sin(x(5));
            ycol(i) = ycol(i)*std::sin(x(5))+ycol(i)*std::cos(x(5));
        }

        Eigen::ArrayXXd xmesh(size,size),ymesh(size,size);
        for(int i=0;i<size;i++){
            xmesh.row(i) = xrow;
            ymesh.col(i) = ycol;
        }
        double x0rot = x(1)*std::cos(x(5)) - x(3)*std::sin(x(5));
        double y0rot = x(1)*std::sin(x(5)) + x(3)*std::cos(x(5));
        Eigen::Map<Eigen::MatrixXd> Z0((double*)mZ.data(),size,size);
        Eigen::ArrayXXd err = Z0.array()-x(0)*(-((xmesh-x0rot).square()/(2*std::pow(x(2),2))+(ymesh-y0rot).square()/(2*std::pow(x(4),2)))).exp();
        Eigen::Map<Eigen::VectorXd> f(err.data(),mZ.size());
        fvec = f;

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
    mMapInitialised = false;

}

Mpa2dHist::Mpa2dHist(QString name, int xbins, int ybins){
    setSize(xbins,ybins);
    mName = name;
    mMaxDepth =8;
    mEnergyCenter = Eigen::Array2d(511,511);
    mCenteredHistSize = 800;
    mCdbCounter = 0;
    mMapInitialised = false;


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

void Mpa2dHist::updateEnergyScale(){
    mXEnergyscale.setLinSpaced(mXSize,511e3-(mCenter(0))*mCal(0),511e3+(mXSize-mCenter(0))*mCal(0));
    mYEnergyscale.setLinSpaced(mYSize,511e3-(mCenter(1))*mCal(1),511e3+(mYSize-mCenter(1))*mCal(1));
    qDebug() << "Corners map";
    qDebug() << (511e3-400*mCal(0)) << (511e3+400*mCal(1));
    qDebug() << 511e3+(mXSize-mCenter(0))*mCal(0);
    qDebug() << 511e3+(mXSize-mCenter(1))*mCal(1);
    qDebug() << 511e3-(mCenter(0))*mCal(0);
    qDebug() << 511e3-(mCenter(1))*mCal(1);
    qDebug() << 511e3+(mXSize-mCenter(0))*mCal(0);
    qDebug() << 511e3-(mCenter(1))*mCal(1);
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
        qDebug() << lm.iter;
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

void Mpa2dHist::findCenter2d(){
    Eigen::Vector2d backup = mCenter;
    Eigen::VectorXd x(6);

    Eigen::MatrixXd cut = mRawHist.block<200,200>(400,400);
    x << cut.maxCoeff(),cut.cols()/2,10,cut.maxCoeff(),cut.rows()/2,10;
    Eigen::Map<Eigen::VectorXd> y(cut.data(), cut.size());
    lmdif_gauss2d_functor functor(y,y.size());
    Eigen::NumericalDiff<lmdif_gauss2d_functor> numDiff(functor);
    Eigen::LevenbergMarquardt<Eigen::NumericalDiff<lmdif_gauss2d_functor>,double> lm(numDiff);
    lm.parameters.maxfev = 6000;
    lm.parameters.xtol = 1.0e-4;
    int ret = lm.minimize(x);
    mCenter(0) = x(1)+400;
    mCenter(1) = x(4)+400;
//    if (ret !=1){
//        qDebug() << "Fitting unsuccesfull";
//        qDebug() << "Increasing cut size";
//        Eigen::MatrixXd cut2 = mRawHist.block<800,800>(std::round(backup(1))-400,std::round(backup(0))-400);
//        x << 1,cut2.cols()/2,10,1,cut2.rows()/2,10;
//        Eigen::Map<Eigen::VectorXd> y2(cut2.data(), cut2.size());
//        lmdif_gauss2d_functor functor2(y2,y2.size());
//        Eigen::NumericalDiff<lmdif_gauss2d_functor> numDiff2(functor2);
//        Eigen::LevenbergMarquardt<Eigen::NumericalDiff<lmdif_gauss2d_functor>,double> lm2(numDiff2);
//        lm2.parameters.maxfev = 10000;
//        lm2.parameters.xtol = 1.0e-3;
//        int ret2 = lm2.minimize(x);
//        mCenter(0) = x(1)+std::round(backup(1))-400;
//        mCenter(1) = x(4)+std::round(backup(0))-400;
//        if (ret2 !=1){
//            qDebug() << "Fitting unsuccesfull";
//            qDebug() << "Using initial guess instead";
//            mCenter=backup;
//        }

//  }

    mCenter(0) = x(1)+400;
    mCenter(1) = x(4)+400;
    qDebug() << mCenter(0) << "," << mCenter(1);
    mCenteredHist = mRawHist.block<800,800>(std::round(mCenter(1))-400,std::round(mCenter(0))-400); //Commented for testing reasons

    //Test: Normalise full hist
    //mCenteredHist = (mCenteredHist.array()/mRawHist.sum()).matrix();
    //mCenteredHist = mRawHist;
}

void Mpa2dHist::findCenterEx(){
    Eigen::Vector2d backup = mCenter;
    Eigen::VectorXd x(6);

    Eigen::MatrixXd cut = mRawHist.block<400,400>(300,300);
    // parameters are: [Amplitude, x0, sigmax, y0, sigmay, angel(in rad)]
    x << cut.maxCoeff(),cut.cols()/2,10,cut.rows()/2,10,3.14/4;
    Eigen::Map<Eigen::VectorXd> y(cut.data(), cut.size());
    lmdif_gauss2dex_functor functor(y,y.size());
    Eigen::NumericalDiff<lmdif_gauss2dex_functor> numDiff(functor);
    Eigen::LevenbergMarquardt<Eigen::NumericalDiff<lmdif_gauss2dex_functor>,double> lm(numDiff);
    lm.parameters.maxfev = 6000;
    lm.parameters.xtol = 1.0e-4;
    int ret = lm.minimize(x);
    mCenter(0) = x(1)+300;
    mCenter(1) = x(3)+300;
    qDebug() << "Center"<< mCenter(0) << "," << mCenter(1);
    mCenteredHist = mRawHist.block<800,800>(std::round(mCenter(1))-400,std::round(mCenter(0))-400);
    mCal(1) = -1*mCal(0)*std::tan(x(5));
    qDebug() <<"Corrected Calibration" << mCal(0) << "," <<mCal(1);
}

void Mpa2dHist::centerHist(){
    mCenteredHist = mRawHist.block<800,800>(std::round(mCenter(1))-400,std::round(mCenter(0))-400);
}

void Mpa2dHist::updateRoi(){
    if(mEnergyBinWidth==-1){
        variableRoi();
        return;
    }
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
    return;

}

void Mpa2dHist::variableRoi(){
    mRoiGrid.clear();
    //Roi Border points. See drawing in lab report.
    Eigen::Vector2d p1;
    Eigen::Vector2d p2;
    Eigen::Vector2d c1, c2,c3,c4;
    std::vector<Eigen::Vector2d> corners(4);
    Eigen::Vector2d v1(1/std::sqrt(2),-1/std::sqrt(2));
    Eigen::Vector2d v2(1/std::sqrt(2),1/std::sqrt(2));
    Eigen::Vector2d v3(1/std::sqrt(2),-1/std::sqrt(2));
    QFile file("../settings/variableRoiTable.txt");

    if(!file.open(QIODevice::ReadWrite)){
        qDebug() << "Failed to open file";
    }
    QTextStream in(&file);
    QVector<double> roiSegments;
    while(!in.atEnd()){
        QString line = in.readLine();
        QStringList entries = line.split(' ');
        roiSegments.push_back(entries[2].toDouble());
    }
    double roiLength = 0;
    foreach (double seg, roiSegments) {
        roiLength += seg;
    }
    mRoiLength = roiLength*1e3;
    v1 = v1*mRoiLength;
    v2 = v2*mRoiWidth;
    p1 = mEnergyCenter.matrix()*1e3-v1;
    p2 = mEnergyCenter.matrix()*1e3+v1;
    corners[0] = p1-v2;
    corners[1] = p1+v2;
    corners[2] = p2+v2;
    corners[3] = p2-v2;
    mRoiBorder = new RoiPixel(corners,mDepth);
    double currentOffset = 0;
    c1 = corners[0];
    c2 = corners[1];

    for(int i=roiSegments.size()-1;i>=0;i--){
       corners[0] = c1+currentOffset*v3;
       corners[1] = c2+currentOffset*v3;
       corners[2] = c2+currentOffset*v3+roiSegments[i]*v3*1e3;
       corners[3] = c1+currentOffset*v3+roiSegments[i]*v3*1e3;
       RoiPixel *px = new RoiPixel(corners,mDepth);
       mRoiGrid.push_back(px);
       currentOffset +=roiSegments[i]*1e3;
    }
    c2 = corners[2];
    c1 = corners[3];
    currentOffset=0;
    for(int i=0;i<roiSegments.size();i++){
        corners[0] = c1+currentOffset*v3;
        corners[1] = c2+currentOffset*v3;
        corners[2] = c2+currentOffset*v3+roiSegments[i]*v3*1e3;
        corners[3] = c1+currentOffset*v3+roiSegments[i]*v3*1e3;
        RoiPixel *px = new RoiPixel(corners,mDepth);
        mRoiGrid.push_back(px);
        currentOffset +=roiSegments[i]*1e3;
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
           if (x==0 && y==0){
               qDebug()<< "upper left:";
               qDebug() << px->mCorners[1](0) <<" , " << px->mCorners[1](1);;
           }
           if (x==799 && y==0){
               qDebug()<< "upper right:";
               qDebug() << px->mCorners[2](0) <<" , " << px->mCorners[2](1);;
           }
           if (x==0 && y==799){
               qDebug() << "lower left:";
               qDebug() << px->mCorners[0](0) <<" , " << px->mCorners[0](1);
           }
           if (x==799 && y==799){
               qDebug()<< "lower right:";
               qDebug() << px->mCorners[3](0)<<" , " << px->mCorners[3](1);;
           }
       }
    }

}

void Mpa2dHist::translateMap(double offset, double binWidth){
    Eigen::Vector2d shift(1/std::sqrt(2),-1/std::sqrt(2));
    shift = shift*offset*binWidth;
    mCenter=mCenter-(shift.array()/mCal);
    updateEnergyScale();
    for (int n=0;n<mEnergyMap.size();n++) {
        CdbPixel *px = mEnergyMap[n];
        px->mCenter = px->mCenter+shift;
        //if (px->mCounts != 0) qDebug() << px->mCounts;
        for(int i=0;i<4;i++){
            px->mCorners[i] = px->mCorners[i]+shift;
        }
    }
}

MpaCdbHist* Mpa2dHist::projectCDBS(){

    //centerHist();

    if(!mMapInitialised){
        findCenterEx();
        updateMap();
        mMapInitialised = true;
        int tmpDepth = mDepth;
        double tmpRoiWidth = mRoiWidth;
        double tmpRoiLength = mRoiLength;
        double tmpEnergyBinWidth = mEnergyBinWidth;
        mOffset = 0;
        mCdbCounter--;
        MpaCdbHist *test = projectCDBS(800,4000,200,4);
        mOffset = test->autoCalibration(200);
        setDepth(tmpDepth);
        setRoi(tmpRoiWidth,tmpRoiLength);
        setEnergyBinWidth(tmpEnergyBinWidth);
        //translateMap(mOffset*(-1),200);
        //updateMap();

    }

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
        projection->setBinContent(i,mRoiGrid[i]->content(),mRoiGrid[i]->mEdges[1].norm(),mRoiGrid[i]->incomplete());
    }

    //projection->autoCalibration(mEnergyBinWidth);
    projection->setRoiInformation(mRoiLength,mRoiWidth,mEnergyBinWidth);
    //calculate energy scale
    Eigen::VectorXd energyScale(mRoiGrid.size());
    Eigen::VectorXd foldoverEnergyScale(mRoiGrid.size()/2);
    for(int i=0;i<mRoiGrid.size();i++){
        double sgn;
        if(i<mRoiGrid.size()/2) {
            sgn= -1;
        }
        else {
            sgn=1;
            foldoverEnergyScale(i-mRoiGrid.size()/2) = (mEnergyCenter.matrix()*1e3-mRoiGrid[i]->mCenter).norm()*sgn+511e3;
        }
        energyScale(i) = (mEnergyCenter.matrix()*1e3-mRoiGrid[i]->mCenter).norm()*sgn+511e3;

    }
    //projection->setCalibration(mEnergyBinWidth,511e3,mRoiGrid.size()/2);
    projection->mEnergyScale = energyScale;
    projection->mEnergyScaleFoldover = foldoverEnergyScale;
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
