#pragma once

extern "C" double __stdcall ConvertYtoX(double y, double b, double a, double pi2);
extern "C" double __stdcall ValuesInitialization(int* m, int n, double b, double a, int* ddelta, double* y);
extern "C" int __stdcall FactorsGeneration(int endVal, int n, int begin, double* x, double* f, double* aa, double* bb);
extern "C" double __stdcall PolynomialValue(double t, int m, int ddelta, double* aa, double* bb);