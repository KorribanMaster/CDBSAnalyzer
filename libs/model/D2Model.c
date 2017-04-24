/*
 * File: D2Model.c
 *
 * MATLAB Coder version            : 2.7
 * C/C++ source code generated on  : 13-Apr-2017 12:41:46
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "D2GaussErfc.h"
#include "D2GaussFunctionRot.h"
#include "D2Model.h"
#include "meshgrid.h"

/* Function Definitions */

/*
 * Arguments    : const double x[10]
 *                double F[40000]
 * Return Type  : void
 */
void D2Model(const double x[10], double F[40000])
{
  double dv0[200];
  int i4;
  static double Y[40000];
  static double X[40000];
  static double xdata[80000];
  int i5;
  double b_x[6];
  for (i4 = 0; i4 < 200; i4++) {
    dv0[i4] = i4;
  }

  meshgrid(dv0, X, Y);
  for (i4 = 0; i4 < 200; i4++) {
    for (i5 = 0; i5 < 200; i5++) {
      xdata[i5 + 200 * i4] = X[i5 + 200 * i4];
      xdata[40000 + (i5 + 200 * i4)] = Y[i5 + 200 * i4];
    }
  }

  b_D2GaussFunctionRot(*(double (*)[6])&x[0], xdata, F);
  b_x[0] = x[6];
  b_x[1] = x[1];
  b_x[2] = x[7];
  b_x[3] = x[8];
  b_x[4] = x[3];
  b_x[5] = x[9];
  D2GaussErfc(b_x, xdata, X);
  for (i4 = 0; i4 < 40000; i4++) {
    F[i4] += X[i4];
  }
}

/*
 * File trailer for D2Model.c
 *
 * [EOF]
 */
