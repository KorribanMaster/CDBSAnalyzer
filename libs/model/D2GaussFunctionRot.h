/*
 * File: D2GaussFunctionRot.h
 *
 * MATLAB Coder version            : 2.7
 * C/C++ source code generated on  : 13-Apr-2017 13:53:20
 */

#ifndef __D2GAUSSFUNCTIONROT_H__
#define __D2GAUSSFUNCTIONROT_H__

/* Include Files */
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "rt_nonfinite.h"
#include "rtwtypes.h"
#include "D2Model_types.h"

/* Function Declarations */
#ifdef __cplusplus

extern "C" {

#endif

  extern void D2GaussFunctionRot(const double x[6], const double xdata[80000],
    double F[40000]);
  extern void b_D2GaussFunctionRot(const double x[6], const double xdata[80000],
    double F[40000]);

#ifdef __cplusplus

}
#endif
#endif

/*
 * File trailer for D2GaussFunctionRot.h
 *
 * [EOF]
 */
