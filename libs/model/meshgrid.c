/*
 * File: meshgrid.c
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
 * Arguments    : const double x[200]
 *                double xx[40000]
 *                double yy[40000]
 * Return Type  : void
 */
void meshgrid(const double x[200], double xx[40000], double yy[40000])
{
  int jcol;
  int ibtile;
  int itilerow;
  for (jcol = 0; jcol < 200; jcol++) {
    ibtile = jcol * 200;
    for (itilerow = 0; itilerow < 200; itilerow++) {
      xx[ibtile + itilerow] = x[jcol];
    }
  }

  for (jcol = 0; jcol < 200; jcol++) {
    ibtile = jcol * 200;
    memcpy(&yy[ibtile], &x[0], 200U * sizeof(double));
  }
}

/*
 * File trailer for meshgrid.c
 *
 * [EOF]
 */
