/*
 * File: exp.c
 *
 * MATLAB Coder version            : 2.7
 * C/C++ source code generated on  : 12-Apr-2017 16:03:13
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "D2GaussErfc.h"
#include "D2GaussFunctionRot.h"
#include "D2Model.h"
#include "exp.h"

/* Function Definitions */

/*
 * Arguments    : double x[40000]
 * Return Type  : void
 */
void b_exp(double x[40000])
{
  int k;
  for (k = 0; k < 40000; k++) {
    x[k] = exp(x[k]);
  }
}

/*
 * File trailer for exp.c
 *
 * [EOF]
 */
