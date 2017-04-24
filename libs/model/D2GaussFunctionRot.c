/*
 * File: D2GaussFunctionRot.c
 *
 * MATLAB Coder version            : 2.7
 * C/C++ source code generated on  : 13-Apr-2017 13:53:20
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "D2GaussErfc.h"
#include "D2GaussFunctionRot.h"
#include "D2Model.h"
#include "exp.h"
#include "power.h"

/* Function Definitions */

/*
 * % x = [Amp, x0, wx, y0, wy, fi]
 * [X,Y] = meshgrid(x,y)
 *   xdata(:,:,1) = X
 *   xdata(:,:,2) = Y
 *  Mrot = [cos(fi) -sin(fi); sin(fi) cos(fi)]
 * %
 * coder.varsize('xdatarot');
 * Arguments    : const double x[6]
 *                const double xdata[80000]
 *                double F[40000]
 * Return Type  : void
 */
void D2GaussFunctionRot(const double x[6], const double xdata[80000], double F
  [40000])
{
  static double xdatarot[80000];
  double b_x;
  double c_x;
  double d_x;
  double e_x;
  double x0rot;
  double y0rot;
  static double b_xdatarot[40000];
  static double c_xdatarot[40000];
  int i2;
  int i3;
  memset(&xdatarot[0], 0, 80000U * sizeof(double));
  b_x = cos(x[5]);
  c_x = sin(x[5]);
  d_x = sin(x[5]);
  e_x = cos(x[5]);
  x0rot = x[1] * cos(x[5]) - x[3] * sin(x[5]);
  y0rot = x[1] * sin(x[5]) + x[3] * cos(x[5]);
  for (i2 = 0; i2 < 200; i2++) {
    for (i3 = 0; i3 < 200; i3++) {
      xdatarot[i3 + 200 * i2] = xdata[i3 + 200 * i2] * b_x - xdata[40000 + (i3 +
        200 * i2)] * c_x;
      xdatarot[40000 + (i3 + 200 * i2)] = xdata[i3 + 200 * i2] * d_x + xdata
        [40000 + (i3 + 200 * i2)] * e_x;
      b_xdatarot[i3 + 200 * i2] = xdatarot[i3 + 200 * i2] - x0rot;
      c_xdatarot[i3 + 200 * i2] = xdatarot[40000 + (i3 + 200 * i2)] - y0rot;
    }
  }

  power(b_xdatarot, F);
  b_x = 2.0 * (x[2] * x[2]);
  power(c_xdatarot, b_xdatarot);
  c_x = 2.0 * (x[4] * x[4]);
  for (i2 = 0; i2 < 40000; i2++) {
    F[i2] = -(F[i2] / b_x + b_xdatarot[i2] / c_x);
  }

  b_exp(F);
  for (i2 = 0; i2 < 40000; i2++) {
    F[i2] *= x[0];
  }

  /*  figure(3) */
  /*  alpha(0) */
  /*  imagesc(F) */
  /*  colormap('gray') */
  /*  figure(gcf)%bring current figure to front */
  /*  drawnow */
  /*  beep */
  /*  pause %Wait for keystroke */
}

/*
 * % x = [Amp, x0, wx, y0, wy, fi]
 * [X,Y] = meshgrid(x,y)
 *   xdata(:,:,1) = X
 *   xdata(:,:,2) = Y
 *  Mrot = [cos(fi) -sin(fi); sin(fi) cos(fi)]
 * %
 * coder.varsize('xdatarot');
 * Arguments    : const double x[6]
 *                const double xdata[80000]
 *                double F[40000]
 * Return Type  : void
 */
void b_D2GaussFunctionRot(const double x[6], const double xdata[80000], double
  F[40000])
{
  static double xdatarot[80000];
  double b_x;
  double c_x;
  double d_x;
  double e_x;
  double x0rot;
  double y0rot;
  static double b_xdatarot[40000];
  static double c_xdatarot[40000];
  int i6;
  int i7;
  memset(&xdatarot[0], 0, 80000U * sizeof(double));
  b_x = cos(x[5]);
  c_x = sin(x[5]);
  d_x = sin(x[5]);
  e_x = cos(x[5]);
  x0rot = x[1] * cos(x[5]) - x[3] * sin(x[5]);
  y0rot = x[1] * sin(x[5]) + x[3] * cos(x[5]);
  for (i6 = 0; i6 < 200; i6++) {
    for (i7 = 0; i7 < 200; i7++) {
      xdatarot[i7 + 200 * i6] = xdata[i7 + 200 * i6] * b_x - xdata[40000 + (i7 +
        200 * i6)] * c_x;
      xdatarot[40000 + (i7 + 200 * i6)] = xdata[i7 + 200 * i6] * d_x + xdata
        [40000 + (i7 + 200 * i6)] * e_x;
      b_xdatarot[i7 + 200 * i6] = xdatarot[i7 + 200 * i6] - x0rot;
      c_xdatarot[i7 + 200 * i6] = xdatarot[40000 + (i7 + 200 * i6)] - y0rot;
    }
  }

  power(b_xdatarot, F);
  b_x = 2.0 * (x[2] * x[2]);
  power(c_xdatarot, b_xdatarot);
  c_x = 2.0 * (x[4] * x[4]);
  for (i6 = 0; i6 < 40000; i6++) {
    F[i6] = -(F[i6] / b_x + b_xdatarot[i6] / c_x);
  }

  b_exp(F);
  for (i6 = 0; i6 < 40000; i6++) {
    F[i6] *= x[0];
  }

  /*  figure(3) */
  /*  alpha(0) */
  /*  imagesc(F) */
  /*  colormap('gray') */
  /*  figure(gcf)%bring current figure to front */
  /*  drawnow */
  /*  beep */
  /*  pause %Wait for keystroke */
}

/*
 * File trailer for D2GaussFunctionRot.c
 *
 * [EOF]
 */
