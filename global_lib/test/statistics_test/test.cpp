#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "../../statistics_tools.h"

using namespace std;

void printArray(const double* data, long long N, std::string pre=std::string("")) {
    printf("%s( ", pre.c_str());
    for (long long i=0; i<N; i++) {
        if (i>0) printf(", ");
        printf("%lf", data[i]);
    }
    printf(" )\n");
}

double f_fitexp(double x, const double* p) {
    return p[0]*exp(p[1]*x);
}

double f_fitexp_d0(double x, const double* p) {
    return exp(p[1]*x);
}
double f_fitexp_d1(double x, const double* p) {
    return p[0]*exp(p[1]*x)*x;
}

int main() {
  {
        printf("test basic statistics()\n");
    double a[] = {9, 8, 5, 6, 7, 1, 2, 0, 3, 4};
    int N=10;
    printArray(a, 10, "input  = ");
    statisticsSort(a, 10);
    printArray(a, 10, "sorted = ");
    printf("   mean    = %lf\n", statisticsAverage(a, N));
    printf("   var     = %lf\n", statisticsVariance(a, N));
    printf("   std     = %lf\n", sqrt(statisticsVariance(a, N)));
    printf("   median  = %lf\n", statisticsSortedMedian(a, N));
    printf("   min     = %lf\n", statisticsSortedMin(a, N));
    printf("   max     = %lf\n", statisticsSortedMax(a, N));
    printf("   q25     = %lf\n", statisticsSortedQuantile(a, N, 0.25));
    printf("   q75     = %lf\n", statisticsSortedQuantile(a, N, 0.75));
    printf("\n\n");
  }
    
  { 
        printf("\n\ntest statisticsSort()\n");
    double a[] = {3, 1, 4};
    int N=3;
    printArray(a, N, "input  = ");
    statisticsSort(a, N);
    printArray(a, N, "sorted = ");
    printf("\n\n");
  }
    
  { 
    double a[] = {1, 2, 3};
    int N=3;
    printArray(a, N, "input  = ");
    statisticsSort(a, N);
    printArray(a, N, "sorted = ");
    printf("\n\n");
  }
    
  { 
    double a[] = {4, 2};
    int N=2;
    printArray(a, N, "input  = ");
    statisticsSort(a, N);
    printArray(a, N, "sorted = ");
    printf("\n\n");
  }
    
  { 
    double a[] = {2, 4};
    int N=2;
    printArray(a, N, "input  = ");
    statisticsSort(a, N);
    printArray(a, N, "sorted = ");
    printf("\n\n");
  }
        
  { 
    double a[] = {4};
    int N=1;
    printArray(a, N, "input  = ");
    statisticsSort(a, N);
    printArray(a, N, "sorted = ");
    printf("\n\n");
  }
        
  { 
    double *a=NULL;
    int N=0;
    printArray(a, N, "input  = ");
    statisticsSort(a, N);
    printArray(a, N, "sorted = ");
    printf("\n\n");
  }
    {
        printf("\n\ntest statisticsSort2()\n");
        double x[] = {9, 8, 5, 6, 7, 1, 2, 0, 3, 4};
        double y[] = {0, 1, 4, 3, 2, 8, 7, 9, 6, 5};

        printArray(y, 10, "input y  = ");
        statisticsSort2(x, y, 10);
        printArray(y, 10, "sorted y = ");
        printArray(x, 10, "       x = ");


        printf("\n\ntest binary search:\n");

        printf("  search x=3 (exact)   = %d\n", (int)statisticsBinarySearch(3.0, x, 10, true));
        printf("  search x=0 (exact)   = %d\n", (int)statisticsBinarySearch(0.0, x, 10, true));
        printf("  search x=-1 (exact)  = %d\n", (int)statisticsBinarySearch(-1.0, x, 10, true));
        printf("  search x=8 (exact)   = %d\n", (int)statisticsBinarySearch(8.0, x, 10, true));
        printf("  search x=9 (exact)   = %d\n", (int)statisticsBinarySearch(9.0, x, 10, true));
        printf("  search x=10 (exact)  = %d\n", (int)statisticsBinarySearch(10.0, x, 10, true));
        printf("  search x=5.5 (exact) = %d\n", (int)statisticsBinarySearch(5.5, x, 10, true));

        printf("\n  search x=3 (nonexact)   = %d\n", (int)statisticsBinarySearch(3.0, x, 10, false));
          printf("  search x=0 (nonexact)   = %d\n", (int)statisticsBinarySearch(0.0, x, 10, false));
          printf("  search x=-1 (nonexact)  = %d\n", (int)statisticsBinarySearch(-1.0, x, 10, false));
          printf("  search x=8 (nonexact)   = %d\n", (int)statisticsBinarySearch(8.0, x, 10, false));
          printf("  search x=9 (nonexact)   = %d\n", (int)statisticsBinarySearch(9.0, x, 10, false));
          printf("  search x=10 (nonexact)  = %d\n", (int)statisticsBinarySearch(10.0, x, 10, false));
          printf("  search x=5.5 (nonexact) = %d\n", (int)statisticsBinarySearch(5.5, x, 10, false));


        printf("\n\ntest linear interpolation:\n");
        printf("\n  interpolate @ x=3 (extended)   = %lf\n", statisticsLinearInterpolateSorted(3.0, x, y, 10, true));
          printf("  interpolate @ x=0 (extended)   = %lf\n", statisticsLinearInterpolateSorted(0.0, x, y, 10, true));
          printf("  interpolate @ x=-1 (extended)  = %lf\n", statisticsLinearInterpolateSorted(-1.0, x, y, 10, true));
          printf("  interpolate @ x=8 (extended)   = %lf\n", statisticsLinearInterpolateSorted(8.0, x, y, 10, true));
          printf("  interpolate @ x=9 (extended)   = %lf\n", statisticsLinearInterpolateSorted(9.0, x, y, 10, true));
          printf("  interpolate @ x=10 (extended)  = %lf\n", statisticsLinearInterpolateSorted(10.0, x, y, 10, true));
          printf("  interpolate @ x=5.5 (extended) = %lf\n", statisticsLinearInterpolateSorted(5.5, x, y, 10, true));
        printf("\n  interpolate @ x=3 (nonextended)   = %lf\n", statisticsLinearInterpolateSorted(3.0, x, y, 10, false));
          printf("  interpolate @ x=0 (nonextended)   = %lf\n", statisticsLinearInterpolateSorted(0.0, x, y, 10, false));
          printf("  interpolate @ x=-1 (nonextended)  = %lf\n", statisticsLinearInterpolateSorted(-1.0, x, y, 10, false));
          printf("  interpolate @ x=8 (nonextended)   = %lf\n", statisticsLinearInterpolateSorted(8.0, x, y, 10, false));
          printf("  interpolate @ x=9 (nonextended)   = %lf\n", statisticsLinearInterpolateSorted(9.0, x, y, 10, false));
          printf("  interpolate @ x=10 (nonextended)  = %lf\n", statisticsLinearInterpolateSorted(10.0, x, y, 10, false));
          printf("  interpolate @ x=5.5 (nonextended) = %lf\n", statisticsLinearInterpolateSorted(5.5, x, y, 10, false));


          printf("\n\ntest linear interpolation:\n");
          printf("\n  find root y=3    = %lf\n", statisticsFindRootSorted(3.0, x, y, 10));
            printf("  find root y=0    = %lf\n", statisticsFindRootSorted(0.0, x, y, 10));
            printf("  find root y=-1   = %lf\n", statisticsFindRootSorted(-1.0, x, y, 10));
            printf("  find root y=8    = %lf\n", statisticsFindRootSorted(8.0, x, y, 10));
            printf("  find root y=7.2  = %lf\n", statisticsFindRootSorted(7.2, x, y, 10));
            printf("  find root y=7.8  = %lf\n", statisticsFindRootSorted(7.8, x, y, 10));
            printf("  find root y=9    = %lf\n", statisticsFindRootSorted(9.0, x, y, 10));
            printf("  find root y=10   = %lf\n", statisticsFindRootSorted(10.0, x, y, 10));
            printf("  find root y=5.5  = %lf\n", statisticsFindRootSorted(5.5, x, y, 10));
            printf("  x at 50%% dY (full)      = %lf\n", statisticsXatY50(x, y, 10));
            printf("  x at 50%% dY (half)      = %lf\n", statisticsXatY50(x, y, 5));
            printf("\n\n");
    }


  {
        printf("\n\ntest regression\n");
    #define REGRESSION_N 50
	double x[REGRESSION_N];
	double y[REGRESSION_N], y2[REGRESSION_N], y3[REGRESSION_N], y0[REGRESSION_N];
	srand(1234);
	for (int i=0; i<REGRESSION_N; i++) {
		x[i]=i;
		y0[i]=2.0+4.5*x[i];
		y[i]=2.0+4.5*x[i]+double(rand())/double(RAND_MAX)*0.2;
		y2[i]=2.0+4.5*x[i]+double(rand())/double(RAND_MAX)*1.0;
		y3[i]=2.0+4.5*x[i]+double(rand())/double(RAND_MAX)*10.0;
	}
	double coeffA=0, coeffB=0;
	statisticsLinearRegression(x, y, REGRESSION_N, coeffA, coeffB);
    printf("regression: ideal result a=2, b=4.5\nregression: error level 0.2\nregression: result a=%lf, b=%lf\n\n", coeffA, coeffB);
	coeffA=0; coeffB=0;
	statisticsLinearRegression(x, y2, REGRESSION_N, coeffA, coeffB);
    printf("regression: ideal result a=2, b=4.5\nregression: error level 1\nregression: result a=%lf, b=%lf\n\n", coeffA, coeffB);
	coeffA=0; coeffB=0;
	statisticsLinearRegression(x, y3, REGRESSION_N, coeffA, coeffB);
    printf("regression: ideal result a=2, b=4.5\nregression: error level 10\nregression: result a=%lf, b=%lf\n\n", coeffA, coeffB);
	coeffA=0; coeffB=0;
	statisticsLinearRegression(x, y0, REGRESSION_N, coeffA, coeffB);
    printf("regression: ideal result a=2, b=4.5\nregression: error level 0\nregression: result a=%lf, b=%lf\n\n", coeffA, coeffB);
  }

    {
        printf("\n\ntest peak search\n");
        double x[] = {0,   0.1,   0.2,   0.3,   0.4,   0.5,   0.6,   0.7,   0.8,   0.9};
        double y[] = {0,     1,-0.5,1,     2,     5,     4,     5,     3,     2,     2};

        double pW=0;
        double pB=0;
        double pH=0;
        double pP=statisticsPeakFindSorted<double>(pW, x, y, 10, 0.0, NAN, &pB, &pH);
        printf("found peak @ %lf,   width=%lf,   baseline=%lf,    height=%lf\n", pP, pW, pB, pH);
        printf("\n\n");
    }
    {
        printf("\n\ntest 2-peak search\n");
        double x[] = {0,   0.1,   0.2,   0.3,   0.4,   0.5,   0.6,   0.7,   0.8,   0.9};
        double y[] = {0,     1,     4,     1,  -0.5,     5,     4,     5,     3,     2,     2};

        double pW=0;
        double pB=0;
        double pH=0;
        double pP=statisticsPeakFindSorted<double>(pW, x, y, 10, 0.0, NAN, &pB, &pH);
        printf("found single-peak @ %lf,   width=%lf,   baseline=%lf,    height=%lf\n", pP, pW, pB, pH);
        double pW2=0;
        double pH2=0;
        double pP2=0;
        statistics2PeakFindSorted<double>(pP, pW, pP2, pW2, x, y, 10, 0.0, NAN, &pB, &pH, &pH2);
        printf("found peak 1 @ %lf,   width=%lf,    height=%lf,   baseline=%lf\n", pP, pW, pH, pB);
        printf("found peak 2 @ %lf,   width=%lf,    height=%lf\n", pP2, pW2, pH2);
        printf("\n\n");
    }

    {
        printf("\n\nmatrix inversion\ninput:\n");
        /*
          Matlab test code:
            M=[1,2,3;2,2,3;4,1,1]
            inv(M)

            M=[10,2,3,4,5,6;2,30,4,5,6,7;3,4,50,6,7,8;4,5,6,9,8,9;5,6,7,8,9,10;6,7,8,9,10,11]
            inv(M)
          */
        double M[] = {1, 2, 3,    2, 2, 3,    4, 1, 1};
        double M1[] = {1, 2, 3,    2, 2, 3,    1, 2, 3};
        double M2[] = {1,0,0,    0,1,0,    0,0,1};
        double M3[] = {10,2,3,4,5,6,   2,30,4,5,6,7,   3,4,50,6,7,8,   4,5,6,9,8,9,   5,6,7,8,9,10,   6,7,8,9,10,11};
        linalgPrintMatrix(M, 3, 3);
        bool ok=linalgMatrixInversion(M,3);
        printf("output matrix:\n");
        linalgPrintMatrix(M, 3, 3);
        printf("\nreturn value: ");
        if (ok) printf("SUCCESS\n"); else printf("FAILED\n");
        printf("\n\nRESULT SHOULD BE:\n"
               "-1    1    0\n"
               " 10  -11   3\n"
               "-6    7   -2\n\n\n\n");


        printf("input matrix:\n");
        linalgPrintMatrix(M1, 3, 3);
        ok=linalgMatrixInversion(M1,3);
        printf("output matrix:\n");
        linalgPrintMatrix(M1, 3, 3);
        printf("\nreturn value: ");
        if (ok) printf("SUCCESS\n"); else printf("FAILED\n");
        printf("\n\n");



        printf("input matrix:\n");
        linalgPrintMatrix(M2, 3, 3);
        ok=linalgMatrixInversion(M2,3);
        printf("output matrix:\n");
        linalgPrintMatrix(M2, 3, 3);
        printf("\nreturn value: ");
        if (ok) printf("SUCCESS\n"); else printf("FAILED\n");
        printf("\n\n");



        printf("input matrix:\n");
        linalgPrintMatrix(M3, 6,6);
        ok=linalgMatrixInversion(M3,6);
        printf("output matrix:\n");
        linalgPrintMatrix(M3, 6,6);
        printf("\nreturn value: ");
        if (ok) printf("SUCCESS\n"); else printf("FAILED\n");
        printf("\n\nRESULT SHOULD BE:\n"
              "1.1111e-001    1.1189e-016   4.8572e-017   5.7593e-016   -5.5556e-001   4.4444e-001\n"
              "9.7145e-017    3.7037e-002   1.3444e-017   1.2490e-016   -1.4815e-001   1.1111e-001\n"
              "4.1633e-017    1.0408e-017   2.2222e-002   5.3776e-017   -6.6667e-002   4.4444e-002\n"
              "4.4409e-016    1.2490e-016   5.5511e-017   5.0000e-001   -1.0000e+000   5.0000e-001\n"
              "-5.5556e-001   -1.4815e-001  -6.6667e-002  -1.0000e+000  -5.4296e+000   6.2000e+000\n"
              "4.4444e-001    1.1111e-001   4.4444e-002   5.0000e-001   6.2000e+000   -6.3000e+000\n\n\n\n");
        printf("\n\n");
    }


    {
        /*
          Matlab test code:
            A=[1,1,1;   4,2,1;   9,3,1];
            B1= [0,1,3]';
            B2 = [0,1,3;   2,3,4]';
            B3 = [0,1,3;   2,3,4;  3,2,0;  1,2,3]';
            A\B1
            A\B2
            A\B3
         */
        printf("\n\nsolve linear equations\ncoefficients:\n");
        double M[] = {1,1,1,   4,2,1,   9,3,1};
        double B1[] = {0,1,3};
        double B2[] = {0,1,3,   2,3,4};
        double B3[] = {0,1,3,   2,3,4,  3,2,0,  1,2,3};
        linalgTransposeMatrix(B2, 2,3);
        linalgTransposeMatrix(B3, 4,3);
        linalgPrintMatrix(M, 3, 3);
        printf("rhs matrix:\n");
        linalgPrintMatrix(B1, 3, 1);
        bool ok=linalgLinSolve(M,B1,3);
        printf("output matrix [from Octave: 0.5  -0.5 -1.6653e-016]:\n\n");
        linalgPrintMatrix(B1, 3, 1);
        printf("\nreturn value: ");
        if (ok) printf("SUCCESS\n"); else printf("FAILED\n");
        printf("\n\n");


        printf("rhs matrix:\n");
        linalgPrintMatrix(B2, 3, 2);
        ok=linalgLinSolve(M,B2,3, 2);
        printf("output matrix [from Octave:\n"
               "  0.50000    0.00000\n"
               "  -0.50000   1.00000\n"
               "  -0.00000   1.00000]:\n\n");
        linalgPrintMatrix(B2, 3, 2);
        printf("\nreturn value: ");
        if (ok) printf("SUCCESS\n"); else printf("FAILED\n");
        printf("\n\n");


        printf("rhs matrix:\n");
        linalgPrintMatrix(B3, 3, 4);
        ok=linalgLinSolve(M,B3,3, 4);
        printf("output matrix [from Octave:\n"
               "   0.50000   0.00000  -0.50000   0.00000\n"
               "  -0.50000   1.00000   0.50000   1.00000\n"
               "  -0.00000   1.00000   3.00000   0.00000]:\n\n");
        linalgPrintMatrix(B3, 3, 4);
        printf("\nreturn value: ");
        if (ok) printf("SUCCESS\n"); else printf("FAILED\n");
        printf("\n\n");


    }



    {
        /*
          Matlab test code:
            X=[1,2,3,4,5,6,7];
            xx=0:0.2:10;
            Y=[0.5,5,8,15,26,36,49];
            p2=polyfit(X,Y,2)
            p3=polyfit(X,Y,3)
            p4=polyfit(X,Y,4)
            plot(X,Y,'r+');
            hold on
            plot(xx,polyval(p2,xx));
            plot(xx,polyval(p3,xx));
            plot(xx,polyval(p4,xx));
            hold off
          */
        printf("\n\npolynomial fit (x^2+noise):\n");
        double X[] = {1,2,3,4,5,6,7};
        double Y[] = {0.5,5,8,15,26,36,49};
        double P[]={0,0,0,0,0};
        printf("poly(2) coefficients [expected from Octave:  -0.071429 -0.089286  1.017857]:\n");
        statisticsPolyFit(X,Y,7,2,P);
        linalgPrintMatrix(P, 1, 3);
        printf("poly(3) coefficients [expected from Octave:  0.428571  -0.658730  1.184524   -0.013889]:\n");
        statisticsPolyFit(X,Y,7,3,P);
        linalgPrintMatrix(P, 1, 4);
        printf("poly(4) coefficients [expected from Octave:  -5.785714   9.196248  -3.562500   0.864899   -0.054924]:\n");
        statisticsPolyFit(X,Y,7,4,P);
        linalgPrintMatrix(P, 1, 5);
        printf("\n\n");


    }
//    {
//        printf("\n\nmatrix LU decomposition\ninput:\n");
//        /*
//          Matlab test code:
//            M=[1,2,3;4,5,6;7,8,9]
//            lu(M)

//            M=[1,2;3,4]
//            lu(M)

//            M=[-2,2,-3;-1,1,3;2,0,-1];
//            lu(M)
//          */
//        double M[] = {1, 2, 3,    4,5,6,    7,8,9};
//        double M1[] = {1,2,3,4};
//        double M2[] = {-2,2,-3,-1,1,3,2,0,-1};
//        double P3[] = {0,0,0,0,0,0,0,0,0};
//        double L3[] = {0,0,0,0,0,0,0,0,0};
//        double U3[] = {0,0,0,0,0,0,0,0,0};
//        double P2[] = {0,0,0,0};
//        double L2[] = {0,0,0,0};
//        double U2[] = {0,0,0,0};
//        double PP3[] = {0,0,0,0};
//        double QQ3[] = {0,0,0,0};
//        double PP2[] = {0,0,0,0};
//        double QQ2[] = {0,0,0,0};

//        linalgPrintMatrix(M, 3, 3);
//        printf("output LU(matrix): \n");
//        linalgMatrixLUDecomposition(M,3,L3,U3,PP3,QQ3);
//        linalgPrintMatrix(L3, 3, 3);
//        printf("\n");
//        linalgPrintMatrix(U3, 3, 3);
//        printf("\nproduct = \n");
//        linalgMatrixProduct(U3,QQ3,3,P3);
//        linalgMatrixProduct(L3,P3,3,P3);
//        linalgMatrixProduct(PP3,P3,3,P3);
//        linalgPrintMatrix(P3, 3, 3);
//        printf("\n\n\n");

//        linalgPrintMatrix(M2, 3, 3);
//        printf("output LU(matrix): \n");
//        linalgMatrixLUDecomposition(M2,3,L3,U3,PP3,QQ3);
//        linalgPrintMatrix(L3, 3, 3);
//        printf("\n");
//        linalgPrintMatrix(U3, 3, 3);
//        printf("\nproduct = \n");
//        linalgMatrixProduct(L3,U3,3,P3);
//        linalgPrintMatrix(P3, 3, 3);
//        printf("\n\n\n");

//        linalgPrintMatrix(M1, 2,2);
//        printf("output LU(matrix): \n");
//        linalgMatrixLUDecomposition(M1,2,L2,U2,PP2,QQ2);
//        linalgPrintMatrix(L2, 2, 2);
//        printf("\n");
//        linalgPrintMatrix(U2, 2, 2);
//        printf("\nproduct = \n");
//        linalgMatrixProduct(L2,U2,2,P2);
//        linalgPrintMatrix(P2, 2, 2);
//        printf("\n\n\n");

//    }
    {
        printf("\n\nmatrix determinant\ninput:\n");
        /*
          Matlab test code:
            M=[1,2,3;4,5,6;7,8,9]
            det(M)

            M=[1,2;3,4]
            det(M)

            M=[-2,2,-3;-1,1,3;2,0,-1];
            det(M)
          */
        double M[] = {1, 2, 3,    4,5,6,    7,8,9};
        double M1[] = {1,2,3,4};
        double M2[] = {-2,2,-3,-1,1,3,2,0,-1};
        linalgPrintMatrix(M, 3, 3);
        printf("output det(matrix): %lf   (should be 6.6613e-016)\n\n\n\n",linalgMatrixDeterminant(M, 3));

        linalgPrintMatrix(M1, 2, 2);
        printf("output det(matrix): %lf   (should be -2)\n\n\n\n",linalgMatrixDeterminant(M1, 2));

        linalgPrintMatrix(M2, 3, 3);
        printf("output det(matrix): %lf   (should be 18)\n\n\n\n",linalgMatrixDeterminant(M2, 3));
    }

    {
        printf("\n\nfit covariance matrix\ninput:\n");
        /*
          Octave test code:
            graphics_toolkit("qt")
            pkg load optim

            x=[1,2,3,4,5,6,7,8,9,10];
            % y=2*exp(x/10)
            y_ideal=[2.2103,2.4428,2.6997,2.9836,3.2974,3.6442,4.0275,4.4511,4.9192,5.4366];
            y=[2.3103,2.3428,2.7997,2.8836,3.3974,3.7442,3.9275,4.5511,4.8192,5.5366];


            plot(x,y,'r-',x,y_ideal,'b-');
            plot(x,y,'r-',x,y_ideal,'b-');

            function [y]=exp_func(x,par)
              y=par(1)*exp(x*par(2));
            end

            [f,p,cvg,iter,corp,covp]=leasqr(x,y,[1,1],"exp_func");

            p
            covp
          */
        double x[] = {1,2,3,4,5,6,7,8,9,10};
        double y[] = {2.3103,2.3428,2.7997,2.8836,3.3974,3.7442,3.9275,4.5511,4.8192,5.5366};
        {
            double A=1,B=1;
            statisticsExponentialRegression(x,y, 10, A, B);
            printf("exp. fit function: %lf * exp( %lf * x )   (should be 2*exp(0.1*x))\n",A,B);
            double p[] ={A,B};
            double chi2=0;
            printf("chi2 = %lf\n",chi2=statisticsGetFitProblemChi2(x,y,10,f_fitexp, p));
            double* J=(double*)statisticsCalloc(2*10,sizeof(double));
            double* CV=(double*)statisticsCalloc(2*2,sizeof(double));
            statisticsGetFitProblemJacobiMatrix(J, x, 10, f_fitexp, p, 2);
            statisticsGetFitProblemVarCovMatrix(CV, J, 10,2, chi2);
            printf("jacobi matrix J:\n");
            linalgPrintMatrix(J, 10, 2, 9,3,'g');
            printf("ideal jacobi matrix J:\n");
            for (int i=0; i<10; i++) {
                printf("%9.3lf %9.3lf\n", f_fitexp_d0(x[i], p), f_fitexp_d1(x[i], p));
            }
            printf("var-cov matrix C:\n");
            linalgPrintMatrix(CV, 2, 2, 9,3,'g');
            printf("fit params +/- errors:\n  amplitude = %lf +/- %lf\n     factor = %lf +/- %lf\n\n", A, statisticsGetFitProblemParamErrors(0, CV, 2), B, statisticsGetFitProblemParamErrors(1, CV, 2));
            statisticsFree(J);
            statisticsFree(CV);
        }

        printf("fit again: now to ideal data!\n");
        double y2[] = {2.2103,2.4428,2.6997,2.9836,3.2974,3.6442,4.0275,4.4511,4.9192,5.4366};
        {
            double A=1,B=1;
            statisticsExponentialRegression(x,y2, 10, A, B);
            printf("exp. fit function: %lf * exp( %lf * x )   (should be 2*exp(0.1*x))\n",A,B);
            double p[] ={A,B};
            double chi2=0;
            printf("chi2 = %lf\n",chi2=statisticsGetFitProblemChi2(x,y2,10,f_fitexp, p));
            double* J=(double*)statisticsCalloc(2*10,sizeof(double));
            double* CV=(double*)statisticsCalloc(2*2,sizeof(double));
            statisticsGetFitProblemJacobiMatrix(J, x, 10, f_fitexp, p, 2);
            statisticsGetFitProblemVarCovMatrix(CV, J, 10,2, chi2);
            printf("jacobi matrix J:\n");
            linalgPrintMatrix(J, 10, 2, 9,3,'g');
            printf("ideal jacobi matrix J:\n");
            for (int i=0; i<10; i++) {
                printf("%9.3lf %9.3lf\n", f_fitexp_d0(x[i], p), f_fitexp_d1(x[i], p));
            }
            printf("var-cov matrix C:\n");
            linalgPrintMatrix(CV, 2, 2, 9,3,'g');
            printf("fit params +/- errors:\n  amplitude = %lf +/- %lf\n     factor = %lf +/- %lf\n\n", A, statisticsGetFitProblemParamErrors(0, CV, 2), B, statisticsGetFitProblemParamErrors(1, CV, 2));
            statisticsFree(J);
            statisticsFree(CV);
        }



    }
    return 0;
}
 
