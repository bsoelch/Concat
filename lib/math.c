#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <math.h>

double concat_math_dfloor(double x){
  return floor(x);
}
double concat_math_dceil(double x){
  return ceil(x);
}
double concat_math_dround(double x){
  return round(x);
}
double concat_math_dabs(double x){
  return fabs(x);
}
double concat_math_dmin(double x,double p){
  return fmin(x,p);
}
double concat_math_dmax(double x,double p){
  return fmax(x,p);
}

double concat_math_dlog2(double x){
  return log2(x);
}
double concat_math_dlog(double x){
  return log(x);
}
double concat_math_dexp(double x){
  return exp(x);
}

double concat_math_dsqrt(double x){
  return sqrt(x);
}
double concat_math_dcbrt(double x){
  return cbrt(x);
}

double concat_math_dpow(double x,double p){
  return pow(x,p);
}

double concat_math_dsin(double x){
  return sin(x);
}
double concat_math_dcos(double x){
  return cos(x);
}
double concat_math_dtan(double x){
  return tan(x);
}
double concat_math_dasin(double x){
  return asin(x);
}
double concat_math_daos(double x){
  return acos(x);
}
double concat_math_datan(double x){
  return atan(x);
}
double concat_math_dsinh(double x){
  return sinh(x);
}
double concat_math_dcosh(double x){
  return cosh(x);
}
double concat_math_dtanh(double x){
  return tanh(x);
}
double concat_math_dasinh(double x){
  return asinh(x);
}
double concat_math_daosh(double x){
  return acosh(x);
}
double concat_math_datanh(double x){
  return atanh(x);
}
double concat_math_datan2(double y,double x){
  return atan2(y,x);
}


bool concat_float_disNaN(double x){
  return isnan(x);
}
bool concat_float_disInfinite(double x){
  return isinf(x)!=0;
}
bool concat_float_disFinite(double x){
  return isfinite(x);
}
