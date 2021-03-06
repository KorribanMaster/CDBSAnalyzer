#include "mpa2dhist.h"

#include <unsupported/Eigen/NonLinearOptimization>
#include <unsupported/Eigen/NumericalDiff>
#include <unsupported/Eigen/SpecialFunctions>

#include <dlib/optimization.h>
#include <dlib/optimization/find_optimal_parameters.h>

#include "D2Model.h"

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

struct lmdif_gauss4dex_functor : Functor<double>
{
    Eigen::VectorXd mZ;
    lmdif_gauss4dex_functor(const Eigen::VectorXd &z,int zsize) : Functor<double>(9,zsize),mZ(z) {}
    int operator()(const Eigen::VectorXd &xtmp, Eigen::VectorXd &fvec) const
    {
        assert(xtmp.size()==9);
        Eigen::VectorXd x(10) ;
        x << xtmp.head(5), -0.813808177223752 , xtmp.tail(4);
        int size=std::sqrt(mZ.size());
        // parameters are: [Amplitude, x0, sigmaxrot, y0, sigmayrot, %angel(in rad),Amplitudex,sigmax,Amplitudey sigmay]
        Eigen::VectorXd xrow,ycol;
        xrow.setLinSpaced(size,0,size-1);
        ycol.setLinSpaced(size,0,size-1);
        Eigen::ArrayXXd xmesh(size,size),ymesh(size,size);
        for(int i=0;i<size;i++){
            xmesh.row(i) = xrow;
            ymesh.col(i) = ycol;
        }
        for(int i=0;i<size;i++){
            xrow(i) = xrow(i)*std::cos(x(5))-xrow(i)*std::sin(x(5));
            ycol(i) = ycol(i)*std::sin(x(5))+ycol(i)*std::cos(x(5));
        }

        Eigen::ArrayXXd xmeshrot(size,size),ymeshrot(size,size);
        for(int i=0;i<size;i++){
            xmeshrot.row(i) = xrow;
            ymeshrot.col(i) = ycol;
        }
        double x0rot = x(1)*std::cos(x(5)) - x(3)*std::sin(x(5));
        double y0rot = x(1)*std::sin(x(5)) + x(3)*std::cos(x(5));
        Eigen::Map<Eigen::ArrayXXd> Z0((double*)mZ.data(),size,size);
        Eigen::ArrayXXd rotgauss = x(0)*(-((xmeshrot-x0rot).square()/(2*std::pow(x(2),2))+(ymeshrot-y0rot).square()/(2*std::pow(x(4),2)))).exp();
        //Eigen::ArrayXXd crossgauss = x(6)*Eigen::exp(-(xmesh-x(1)).square()/(2*std::pow(x(7),2)))+x(8)*Eigen::exp(-(ymesh-x(3)).square()/(2*std::pow(x[9],2)));
        Eigen::ArrayXXd crossgausserfc = (x(6)*0.1*Eigen::erfc((ymesh-x(3))/x(4))+0.9*x(6))*Eigen::exp(-(xmesh-x(1)).square()/(2*std::pow(x(7),2)))+(x(8)*0.1*Eigen::erfc((ymesh-x(1))/x(2))+0.9*x(8))*Eigen::exp(-(ymesh-x(3)).square()/(2*std::pow(x[9],2)));
        Eigen::ArrayXXd err = Z0-(rotgauss+crossgausserfc);
        Eigen::Map<Eigen::VectorXd> f(err.data(),mZ.size());
        fvec = f;

        return 0;
    }
};

struct lmdif_gaussrotonly_functor : Functor<double>
{
    Eigen::VectorXd mZ;
    Eigen::VectorXd mCenter;
    lmdif_gaussrotonly_functor(const Eigen::VectorXd &z,int zsize,const Eigen::VectorXd center) : Functor<double>(8,zsize),mZ(z),mCenter(center) {}
    int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
    {
        assert(x.size()==8);
        int size=std::sqrt(mZ.size());
        // parameters are: [Amplitude, sigmaxrot, sigmayrot, angel(in rad),Amplitudex,sigmax,Amplitudey sigmay]
        Eigen::VectorXd xrow,ycol;
        xrow.setLinSpaced(size,1,size);
        ycol.setLinSpaced(size,size,1);
        Eigen::ArrayXXd xmesh(size,size),ymesh(size,size);
        for(int i=0;i<size;i++){
            xmesh.row(i) = xrow;
            ymesh.col(i) = ycol;
        }
        for(int i=0;i<size;i++){
            xrow(i) = xrow(i)*std::cos(x(3))-xrow(i)*std::sin(x(3));
            ycol(i) = ycol(i)*std::sin(x(3))+ycol(i)*std::cos(x(3));
        }

        Eigen::ArrayXXd xmeshrot(size,size),ymeshrot(size,size);
        for(int i=0;i<size;i++){
            xmeshrot.row(i) = xrow;
            ymeshrot.col(i) = ycol;
        }
        double x0rot = mCenter(0)*std::cos(x(3)) - mCenter(1)*std::sin(x(3));
        double y0rot = mCenter(0)*std::sin(x(3)) + mCenter(1)*std::cos(x(3));
        Eigen::Map<Eigen::ArrayXXd> Z0((double*)mZ.data(),size,size);
        Eigen::ArrayXXd rotgauss = x(0)*(-((xmeshrot-x0rot).square()/(2*std::pow(x(1),2))+(ymeshrot-y0rot).square()/(2*std::pow(x(2),2)))).exp();
        Eigen::ArrayXXd crossgauss = x(4)*Eigen::exp(-(xmesh-mCenter(0)).square()/(2*std::pow(x(5),2)))+x(6)*Eigen::exp(-(ymesh-mCenter(1)).square()/(2*std::pow(x(7),2)));
        Eigen::ArrayXXd err = Z0-(rotgauss+crossgauss);
        Eigen::Map<Eigen::VectorXd> f(err.data(),mZ.size());
        fvec = f;

        return 0;
    }
};
typedef dlib::matrix<double,0,1> column_vector;

class DataModel
{
public:
    DataModel(Eigen::ArrayXXd data){
        mData = data;
    }
    double operator()(const column_vector& x) const{
        int size=mData.cols();
        // parameters are: [Amplitude, x0, sigmaxrot, y0, sigmayrot, %angel(in rad),Amplitudex,sigmax,Amplitudey sigmay]
        double xraw[10],F[40000];
        for (int i=0;i<10;i++){
            xraw[i] = x(i);
        }
        D2Model(xraw,F);
        Eigen::Map<Eigen::ArrayXXd> model(F,200,200);
        double err = (mData-model).matrix().squaredNorm()/40000;
        return err;

    }
private:
    Eigen::ArrayXXd mData;

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
//    qDebug() << "Corners map";
//    qDebug() << (511e3-400*mCal(0)) << (511e3+400*mCal(1));
//    qDebug() << 511e3+(mXSize-mCenter(0))*mCal(0);
//    qDebug() << 511e3+(mXSize-mCenter(1))*mCal(1);
//    qDebug() << 511e3-(mCenter(0))*mCal(0);
//    qDebug() << 511e3-(mCenter(1))*mCal(1);
//    qDebug() << 511e3+(mXSize-mCenter(0))*mCal(0);
//    qDebug() << 511e3-(mCenter(1))*mCal(1);
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
    qDebug() << "Center"<< mCenter(0) << "," << mCenter(1);
    int topX,topY;
    topY = std::round(mCenter(1))-400;
    topX = std::round(mCenter(0))-400;
    mCenteredHist = mRawHist.block(topY,topX,800,800); //Commented for testing reasons

    //Test: Normalise full hist
    //mCenteredHist = (mCenteredHist.array()/mRawHist.sum()).matrix();
    //mCenteredHist = mRawHist;
}

void Mpa2dHist::findCenterEx(){
    Eigen::Vector2d backup = mCenter;
    Eigen::VectorXd x(6);

    Eigen::MatrixXd cut = mRawHist.block<200,200>(400,400);
    // parameters are: [Amplitude, x0, sigmax, y0, sigmay, angel(in rad)]
    x << cut.maxCoeff(),cut.cols()/2,20,cut.rows()/2,10,3.14/5;
    Eigen::Map<Eigen::VectorXd> y(cut.data(), cut.size());
    lmdif_gauss2dex_functor functor(y,y.size());
    Eigen::NumericalDiff<lmdif_gauss2dex_functor> numDiff(functor);
    Eigen::LevenbergMarquardt<Eigen::NumericalDiff<lmdif_gauss2dex_functor>,double> lm(numDiff);
    lm.parameters.maxfev = 6000;
    lm.parameters.xtol = 1.0e-6;
    int ret = lm.minimize(x);
    qDebug() << lm.nfev;
    mCenter(0) = x(1)+400;
    mCenter(1) = x(3)+400;
    qDebug() << "Center"<< mCenter(0) << "," << mCenter(1);
    mCenteredHist = mRawHist.block<800,800>(std::round(mCenter(1))-400,std::round(mCenter(0))-400);
    //mCal(1) = -1*mCal(0)*std::tan(x(5));
    qDebug() <<"Corrected Calibration" << mCal(0) << "," <<mCal(1);
}

void Mpa2dHist::findCenterTrustRegion(){

    column_vector x(10);
    column_vector ub(10);
    column_vector lb(10);
    Eigen::ArrayXXd cut = mRawHist.block<200,200>(400,400).array();
    x = cut.maxCoeff(),	100,	10,	100,	20, -3.14/4 ,	22.9363986856602,	23.6247277546898,	25.2606329760491,	20.3816849203029;
    ub = cut.maxCoeff(), 200 , 1000, 200, 1000,3.14/3,200,200,200,200;
    lb = 0,0,0,0,0,-3.14/3,0,0,0,0;
    //dlib::find_min_bobyqa(DataModel(cut),x,21,lb,ub,.1,1e-4,1000);
    double err = dlib::find_optimal_parameters(1,1e-7,2000,x,lb,ub,DataModel(cut));
    //dlib::find_min_box_constrained(dlib::bfgs_search_strategy(),dlib::objective_delta_stop_strategy(1e-6),DataModel,dlib::derivative(DataModel),x,lb,ub);

    for (int i=0;i<10;i++){
        qDebug() << x(i);
    }
    qDebug()<< err;

    mCenter(0) = x(1)+400;
    mCenter(1) = x(3)+400;
    qDebug() << "Center"<< mCenter(0) << "," << mCenter(1);
    mCenteredHist = mRawHist.block<800,800>(std::round(mCenter(1))-400,std::round(mCenter(0))-400);
    //mCal(1) = std::abs(mCal(0)/std::tan(x(5)));
    //qDebug() <<"Corrected Calibration" << mCal(0) << "," <<mCal(1);
}

void Mpa2dHist::findCenter4d(){
    Eigen::Vector2d backup = mCenter;
    Eigen::VectorXd x(9);

    Eigen::MatrixXd cut = mRawHist.block<200,200>(400,400);
    // parameters are: [Amplitude rot, x0, sigmaxrot, y0, sigmayrot, angel(in rad),Amplitudex,sigmax,Amplitudey sigmay]
    x << 2288.63295352580,	115.552765609990,	10.5854086859074,	109.412457372976,	22.9230261806958,	22.9363986856602,	23.6247277546898,	25.2606329760491,	20.3816849203029;
    Eigen::Map<Eigen::VectorXd> y(cut.data(), cut.size());
    lmdif_gauss4dex_functor functor(y,y.size());
    Eigen::NumericalDiff<lmdif_gauss4dex_functor> numDiff(functor);
    Eigen::LevenbergMarquardt<Eigen::NumericalDiff<lmdif_gauss4dex_functor>,double> lm(numDiff);
    lm.parameters.factor = 50;
    lm.parameters.maxfev = 600;
    lm.parameters.xtol = 1.0e-4;
    int ret = lm.minimize(x);
    qDebug() << lm.nfev;
    if (ret==1){
        mCenter(0) = x(1)+400;
        mCenter(1) = x(3)+400;
    }
    //mCenter(0) = x(1)+400;
    //mCenter(1) = x(3)+400;
    qDebug() << "Center"<< mCenter(0) << "," << mCenter(1);
    int topX,topY;
    topY = std::round(mCenter(1))-400;
    topX = std::round(mCenter(0))-400;
    mCenteredHist = mRawHist.block(topY,topX,800,800);
    qDebug() <<"Corrected Calibration" << mCal(0) << "," <<mCal(1);
    //mCal(1) = std::abs(mCal(0)/std::tan(x(5)));
    //qDebug() <<"Corrected Calibration" << mCal(0) << "," <<mCal(1);
}

void Mpa2dHist::findRot(){
    Eigen::Vector2d backup = mCenter;
    Eigen::VectorXd x(8);

    Eigen::MatrixXd cut = mRawHist.block<200,200>(400,400);
    // parameters are: [Amplitude, x0, sigmax, y0, sigmay, angel(in rad)]
    x << cut.maxCoeff()*0.8,40,10,3.14/5,cut.maxCoeff()*0.1,10,cut.maxCoeff()*0.1,10;
    Eigen::Map<Eigen::VectorXd> y(cut.data(), cut.size());
    lmdif_gaussrotonly_functor functor(y,y.size(),mCenter-400);
    Eigen::NumericalDiff<lmdif_gaussrotonly_functor> numDiff(functor);
    Eigen::LevenbergMarquardt<Eigen::NumericalDiff<lmdif_gaussrotonly_functor>,double> lm(numDiff);
    lm.parameters.maxfev = 6000;
    lm.parameters.xtol = 1.0e-6;
    int ret = lm.minimize(x);
    mCal(1) = std::abs(mCal(0)*std::tan(x(3)));
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

    double correctionX = mCenter(0)-std::round(mCenter(0));
    double correctionY = mCenter(1)-std::round(mCenter(1));
    leftTop(0) = 511e3-(mCenteredHist.rows()/2-correctionX)*mCal(0);//correction missing for using int percision here
    leftTop(1) = 511e3+(mCenteredHist.cols()/2-correctionY)*mCal(1);

//    leftTop(0) = 511e3-(mCenteredHist.rows()/2)*mCal(0);//correction missing for using int percision here
//    leftTop(1) = 511e3+(mCenteredHist.cols()/2)*mCal(1);
    for(int y=0;y<mCenteredHist.cols();y++){
       for(int x=0;x<mCenteredHist.rows();x++){
           Eigen::Array2d center(x,y);
           Eigen::Array2d tmp;
           Eigen::Array2d odd;
           std::vector<Eigen::Array2d> corners;
           center(0) = leftTop(0)+x*mCal(0);
           center(1) = leftTop(1)-y*mCal(1);
           odd << -0.5,0.5;
           tmp = center-0.5*mCal;
           corners.push_back(tmp);
           tmp = center+odd*mCal;
           corners.push_back(tmp);
           tmp = center+0.5*mCal;
           corners.push_back(tmp);
           tmp = center-odd*mCal;
           corners.push_back(tmp);
           CdbPixel *px = new CdbPixel(corners,mCenteredHist(y,mCenteredHist.rows()-x-1));
           mEnergyMap.push_back(px);
           if (x==0 && y==0){
               qDebug()<< "upper left:";
               qDebug() << px->mCorners[1](0) <<" , " << px->mCorners[1](1);;
           }
           if (x==799 && y==0){
               qDebug()<< "upper right:";
               qDebug() << px->mCorners[2](0) <<" , " << px->mCorners[2](1);;
           }
           if (x==400 && y==400){
                          qDebug()<< "Center:";
                          qDebug() << px->mCenter(0) <<" , " << px->mCenter(1);;
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
        //findCenter2d();
        findCenterTrustRegion();
        //findCenter4d();
        //findRot();
        updateMap();
        mMapInitialised = true;
        /*
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
        translateMap(mOffset*(-1),200);
        //updateMap();
        */


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
