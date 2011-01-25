#include <iostream>
#include <cmath>
#include <cfloat>
#include <cstdio>
using namespace std;

double roundError(double error, int addSignifcant) {
    if (fabs(error)<DBL_MIN*10.0) return error;
    int sbits_error=ceil(log(fabs(error))/log(10.0));
    double f=pow(10.0, sbits_error-1-addSignifcant);
    return round(error/f)*f;
}

double roundWithError(double value, double error, int addSignifcant)  {
    if (fabs(error)<DBL_MIN*10) return value;

    int sbits_error=ceil(log(fabs(error))/log(10.0));
    double f=pow(10.0, sbits_error-1-addSignifcant);

    return round(value/f)*f;
}

int main()
{
    double e1=123456.789123456789;
    printf("%.10lf -> %.10lf\n", e1, roundError(e1, 3) );
    double e2=-0.000123456789;
    printf("%.10lf -> %.10lf\n", e2, roundError(e2, 2) );
    e1=1.234;
    printf("%.10lf -> %.10lf\n", e1, roundError(e1, 1) );
    e1=12.34;
    printf("%.10lf -> %.10lf\n", e1, roundError(e1, 1) );
    e1=123.4;
    printf("%.10lf -> %.10lf\n", e1, roundError(e1, 1) );
    e1=1234;
    printf("%.10lf -> %.10lf\n", e1, roundError(e1, 1) );
    e1=0.12345;
    printf("%.10lf -> %.10lf\n", e1, roundError(e1, 1) );
    e1=0.01234;
    printf("%.10lf -> %.10lf\n", e1, roundError(e1, 1) );
    e1=0.001234;
    printf("%.10lf -> %.10lf\n", e1, roundError(e1, 1) );
    e1=0.0001234;
    printf("%.10lf -> %.10lf\n", e1, roundError(e1, 1) );
    e1=0.00001234;
    printf("%.10lf -> %.10lf\n", e1, roundError(e1, 1) );

    double v,e;
    int bits=2;
    v=-1234.56789; e=123456789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=12345678.9;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=1234567.89;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=123456.789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=12345.6789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=1234.56789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=123.456789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=12.3456789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=1.23456789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=0.123456789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=0.0123456789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=0.00123456789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );

    v=-0.00123456789; e=123456789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=12345678.9;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=1234567.89;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=123456.789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=12345.6789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=1234.56789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=123.456789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=12.3456789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=1.23456789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=0.123456789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=0.0123456789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=0.00123456789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=0.000123456789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=0.0000123456789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    e=0.00000123456789;
    printf("%20.10lf +/- %20.10lf   ->   %20.10lf +/- %20.10lf\n", v, e, roundWithError(v,e, bits), roundError(e, bits) );
    return 0;
}
