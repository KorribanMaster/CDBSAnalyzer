/*
 * File: D2GaussErfc.c
 *
 * MATLAB Coder version            : 2.7
 * C/C++ source code generated on  : 12-Apr-2017 16:03:13
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "D2GaussErfc.h"
#include "D2GaussFunctionRot.h"
#include "D2Model.h"
#include "erfc.h"
#include "exp.h"
#include "power.h"

/* Function Definitions */

/*
 * % x = [AmpX, x0, wx, ,AmpY, y0, wy]
 * [X,Y] = meshgrid(x,y)
 *   xdata(:,:,1) = X
 *   xdata(:,:,2) = Y
 * Arguments    : const double x[6]
 *                const double xdata[80000]
 *                double F[40000]
 * Return Type  : void
 */
void D2GaussErfc(const double x[6], const double xdata[80000], double F[40000])
{
  double a;
  static double b_xdata[40000];
  int i0;
  int i1;
  static double A[40000];
  static double B[40000];
  double b_B;
  a = x[0] * 0.1;
  for (i0 = 0; i0 < 200; i0++) {
    for (i1 = 0; i1 < 200; i1++) {
      b_xdata[i1 + 200 * i0] = (xdata[40000 + (i1 + 200 * i0)] - x[4]) / x[5];
    }
  }

  b_erfc(b_xdata, A);
  for (i0 = 0; i0 < 200; i0++) {
    for (i1 = 0; i1 < 200; i1++) {
      b_xdata[i1 + 200 * i0] = xdata[i1 + 200 * i0] - x[1];
    }
  }

  power(b_xdata, B);
  b_B = 2.0 * (x[2] * x[2]);
  for (i0 = 0; i0 < 40000; i0++) {
    B[i0] = -(B[i0] / b_B);
  }

  b_exp(B);
  b_B = x[0] * 0.9;
  for (i0 = 0; i0 < 40000; i0++) {
    A[i0] = (a * A[i0] + b_B) * B[i0];
  }

  for (i0 = 0; i0 < 200; i0++) {
    for (i1 = 0; i1 < 200; i1++) {
      b_xdata[i1 + 200 * i0] = xdata[40000 + (i1 + 200 * i0)] - x[4];
    }
  }

  power(b_xdata, B);
  b_B = 2.0 * (x[5] * x[5]);
  for (i0 = 0; i0 < 40000; i0++) {
    B[i0] = -(B[i0] / b_B);
  }

  b_exp(B);
  a = x[3] * 0.1;
  for (i0 = 0; i0 < 200; i0++) {
    for (i1 = 0; i1 < 200; i1++) {
      b_xdata[i1 + 200 * i0] = (xdata[i1 + 200 * i0] - x[1]) / x[2];
    }
  }

  b_erfc(b_xdata, F);
  b_B = x[3] * 0.9;
  for (i0 = 0; i0 < 40000; i0++) {
    F[i0] = (a * F[i0] + b_B) * B[i0] + A[i0];
  }

  /* *exp(-((xdata(:,:,2)-x(5)).^2/(2*x(6)^2))); */
}

/*
 * File trailer for D2GaussErfc.c
 *
 * [EOF]
 */
