/*
 * File: power.c
 *
 * MATLAB Coder version            : 2.7
 * C/C++ source code generated on  : 12-Apr-2017 16:03:13
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "D2GaussErfc.h"
#include "D2GaussFunctionRot.h"
#include "D2Model.h"
#include "power.h"

/* Function Definitions */

/*
 * Arguments    : const double a[40000]
 *                double y[40000]
 * Return Type  : void
 */
void power(const double a[40000], double y[40000])
{
  int k;
  for (k = 0; k < 40000; k++) {
    y[k] = a[k] * a[k];
  }
}

/*
 * File trailer for power.c
 *
 * [EOF]
 */
