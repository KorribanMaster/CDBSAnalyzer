#include "mpacdbhist.h"

#include <Eigen/Core>
#include <unsupported/Eigen/NonLinearOptimization>
#include <unsupported/Eigen/NumericalDiff>


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

MpaCdbHist::MpaCdbHist(QString name ,QObject *parent): QObject(parent)
{
    setSize(0);
    mName = name;
}

MpaCdbHist::MpaCdbHist(QString name,int size,QObject *parent): QObject(parent)
{
    mName = name;
    setSize(size);
}

MpaCdbHist::~MpaCdbHist(){

}

void MpaCdbHist::setSize(int size){
    mSize = size;
    mProjectionHist.resize(mSize);
    mProjectionHistError.resize(mSize);
    mNormHist.resize(mSize);
    mNormHistError.resize(mSize);
    mFoldoverHist.resize(mSize/2);
    mFoldoverHistError.resize(mSize/2);
    mNormFoldoverHist.resize(mSize/2);
    mNormFoldoverHistError.resize(mSize/2);
    mEnergyScale.resize(mSize);
    mEnergyScaleFoldover.resize(mSize/2);
    mIncomplete.resize(mSize);
    mSingleBinWidth.resize(mSize);
}

void MpaCdbHist::setBinContent(int nBin, double counts, double binWidth, double incomplete){
    mProjectionHist(nBin) = counts;
    mProjectionHistError(nBin) = std::sqrt(counts+std::pow(incomplete,2));
    mIncomplete(nBin) = incomplete;
    mSingleBinWidth(nBin) = binWidth;
}

void MpaCdbHist::setCalibration(float channelToEnergy, float peakEnergy, float peakPosition){
    mEnergyScale.setLinSpaced(mSize,peakEnergy-peakPosition*channelToEnergy,peakEnergy+(mSize-peakPosition)*channelToEnergy);
    mEnergyScaleFoldover.setLinSpaced(mSize/2,peakEnergy,peakEnergy+(mSize-peakPosition)*channelToEnergy); //must be adjusted if peak is not at the center position!!!
}

double MpaCdbHist::autoCalibration(double binWidth){
    Eigen::VectorXd x(3);
    x << mProjectionHist.maxCoeff(), mProjectionHist.size()/2 , 50;
    lmdif_gauss_functor functor(mProjectionHist, mProjectionHist.size());
    Eigen::NumericalDiff<lmdif_gauss_functor> numDiff(functor);
    Eigen::LevenbergMarquardt<Eigen::NumericalDiff<lmdif_gauss_functor>,double> lm(numDiff);
    lm.parameters.maxfev = 2000;
    lm.parameters.xtol = 1.0e-10;
    int ret = lm.minimize(x);
    // check return values
    if (ret !=1){
        qDebug() << "auto calibration: Fitting unsuccesfull";
    }
    qDebug() << mName << x(1) << mProjectionHist.size();
    mEnergyScale.setLinSpaced(mSize,511e3-x(1)*binWidth,511e3+(mSize-x(1))*binWidth);
    return x(1)-mProjectionHist.size()/2;

}

void MpaCdbHist::setRoiInformation(double roiWidth, double roiLength, double binWidth){
    mRoiWidth = roiWidth;
    mRoiLength = roiLength;
    mEnergyBinWidth = binWidth;
}

void MpaCdbHist::calculateNorm(){
    mNorm = 0;
    //mSingleBinWidth = (mSingleBinWidth.array()/mSingleBinWidth.sum()).matrix();
    for(int i=0;i<mSize;i++){
        mNorm += mProjectionHist(i);
    }
    //double normError = std::sqrt((mProjectionHistError.array()*mProjectionHistError.array()).sum());
    for(int i=0;i<mSize;i++){
        mNormHist(i) = mProjectionHist(i)/(mNorm*mSingleBinWidth(i));
        //error calculation
        mNormHistError(i) = std::sqrt(std::pow(mProjectionHistError(i)/mProjectionHist(i),2)+std::pow(std::sqrt(mNorm)/mNorm,2))*mNormHist(i);
    }
}

void MpaCdbHist::calculateFoldover(){
    calculateNorm();
    if(mSize%2 == 1){
        qDebug() << "Odd size is not supposed to happen";

    }
    else{
        for(int i=0;i<mSize/2;i++){
            mFoldoverHist(i) = mProjectionHist(mSize/2+i) + mProjectionHist(mSize/2-1-i);
            mFoldoverHistError(i) = std::sqrt(std::pow(mProjectionHistError(mSize/2+i),2)+std::pow(mProjectionHistError(mSize/2-1-i),2));
            mNormFoldoverHist(i) = (mProjectionHist(mSize/2+i) + mProjectionHist(mSize/2-1-i))/(mNorm*mSingleBinWidth(mSize/2+i));
            mNormFoldoverHistError(i) = std::sqrt(std::pow(mFoldoverHistError(i)/mFoldoverHist(i),2)+std::pow(std::sqrt(mNorm)/mNorm,2))*mNormFoldoverHist(i);
        }
    }
}

//MpaCdbHist operator+(MpaCdbHist const& lhs,MpaCdbHist const& rhs){
//    if(lhs.mSize != rhs.mSize){
//        qDebug()<< "Error: Hist sizes do not match";
//        throw std::invalid_argument("Histogramms need to have the same size");
//    }
//    if(lhs.mRoiLength != rhs.mRoiLength) qDebug()<< "Warning: Different roi length";
//    if(lhs.mRoiWidth != rhs.mRoiWidth) qDebug()<< "Warning: Different roi width";
//    if(lhs.mEnergyBinWidth != rhs.mEnergyBinWidth) qDebug()<< "Warning: Different bin width";
//    QString histName = lhs.mName+"+"+rhs.mName;
//    MpaCdbHist hist(histName);
//    hist.setSize(std::max(lhs.mSize,rhs.mSize));
//    hist.setRoiInformation(lhs.mRoiWidth,lhs.mRoiLength,lhs.mEnergyBinWidth);
//    hist.mEnergyScale = lhs.mEnergyScale;
//    for(int i=0;i<hist.mSize;i++)
//    {
//        double bin = lhs.mProjectionHist(i)+rhs.mProjectionHist(i);
//        hist.setBinContent(i,bin);
//    }
//    hist.calculateFoldover();
//    return hist;
//}

//MpaRefHist* MpaCdbHist::operator /(MpaCdbHist const& lhs,MpaCdbHist const& rhs){
//    if(lhs_>mSize != rhs->mSize){
//        qDebug()<< "Error: Hist sizes do not match";
//        throw std::invalid_argument("Histogramms need to have the same size");
//    }
//    if(lhs->mRoiLength != rhs->mRoiLength) qDebug()<< "Warning: Different roi length";
//    if(lhs->mRoiWidth != rhs->mRoiWidth) qDebug()<< "Warning: Different roi width";
//    if(lhs->mEnergyBinWidth != rhs->mEnergyBinWidth) qDebug()<< "Warning: Different bin width";
//}

