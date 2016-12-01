#include <Eigen/Dense>
#include <unsupported/Eigen/NonLinearOptimization>
#include <unsupported/Eigen/NumericalDiff>
#include "qcustomplot.h"

// Generic functor
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

int main(int argc, char *argv[]){
    Eigen::VectorXd x(3),fvec(101);
    x << 20,52,10;
    Eigen::VectorXd  y(101), p(3);
    p(0) = 100;
    p(1) = 50;
    p(2) = 5;
    for (int i=0; i<101; ++i)
    {
      y(i) = (p(0)*std::exp(-(std::pow((i-p(1)),2)/(2*pow(p(2),2))))); // let's plot a quadratic function
    }
    y = y + 4*Eigen::VectorXd::Random(101);
    lmdif_gauss_functor functor(y,101);
    Eigen::NumericalDiff<lmdif_gauss_functor> numDiff(functor);
    Eigen::LevenbergMarquardt<Eigen::NumericalDiff<lmdif_gauss_functor>,double> lm(numDiff);
    lm.parameters.maxfev = 2000;
    lm.parameters.xtol = 1.0e-80;

    int ret = lm.minimize(x);
    functor(x,fvec);
    QVector<double> data(101),energy(101),fit(101);
    for (int i=0; i<101; ++i)
    {
        data[i] = y(i);
        fit[i] = fvec(i);
        energy[i] = i;
    }
    QCustomPlot *plot = new QCustomPlot();
    plot->addGraph();
    plot->graph(0)->setData(energy, data);
    plot->addGraph();
    plot->graph(1)->setData(energy, fit);
    // give the axes some labels:
    plot->xAxis->setLabel("x");
    plot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    plot->xAxis->setRange(-5, 5);
    plot->yAxis->setRange(0, 1);
    plot->replot();
    plot->show();
    int i =1;
    i++;

    return 1;
}
