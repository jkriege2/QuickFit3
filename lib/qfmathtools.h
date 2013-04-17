#ifndef QFMATHTOOLS_H
#define QFMATHTOOLS_H


#include <cmath>
#include <cfloat>
#include <QSettings>
#include <QWidget>
#include <QSplitter>
#include <QFile>
#include "lib_imexport.h"
#include <QFileDialog>
#include <cstdlib>

/*! \brief Avogadro's number \f$ N_A=6.022\cdot10^{23} \f$
    \ingroup qf3lib_mathtools
 */
#define QF_NAVOGADRO (6.02214179e23)

/*! \brief square root of pi \f$ \sqrt{\pi}=1.772453851 \f$
    \ingroup qf3lib_mathtools
 */
#define QF_SQRTPI (1.772453851)

/*! \brief check whether the dlotaing point number is OK (i.e. non-inf, non-NAN)
    \ingroup qf3lib_mathtools
 */
#define QFFloatIsOK(v) (std::isfinite(v) && (!std::isinf(v)) && (!std::isnan(v)))

/*! \brief squares the argument
    \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT inline double qfSqr(double x) { return x*x; }
QFLIB_EXPORT inline float qfSqr(float x) { return x*x; }

/*! \brief takes the argument to the third power
    \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT inline double qfCube(double x) { return x*x*x; }
QFLIB_EXPORT inline float qfCube(float x) { return x*x*x; }

/*! \brief takes the argument to the fourth power
    \ingroup qf3lib_mathtools
 */
QFLIB_EXPORT inline double qfPow4(double x) { double x2=x*x; return x2*x2; }
QFLIB_EXPORT inline float qfPow4(float x) { float x2=x*x; return x2*x2; }


QFLIB_EXPORT double qfSinc(double x);

/*! \brief calculate the average and variance of a given array
    \ingroup qf3lib_mathtools

    \f[ \overline{v}=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot v_i}{\sum\limits_{i=0}^{N-1}w_i} \f]
    \f[ \text{Var}(v)=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot (v_i-\overline{v})^2}{\sum\limits_{i=0}^{N-1}w_i} \f]

*/
template <class T>
double qfstatisticsAverageVariance(double& var, const T value) {
    long long N=value.size();
    if (N<=1) return 0;
    register double sum=0;
    register double sum2=0;
    long long NN=0;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i])) {
            sum2=sum2+(double)(value[i]*value[i]);
            sum=sum+(double)(value[i]);
            NN++;
        }
    }
    var= ( sum2 - sum*sum/(double)NN ) / (double)(NN-1);
    return sum/(double)NN;
}

/*! \brief calculate the average and variance of a given array
    \ingroup qf3lib_mathtools

    \f[ \overline{v}=\frac{1}{N}\cdot\sum\limits_{i=0}^{N-1} v_i \f]
*/
template <class T>
double qfstatisticsAverage(const T value) {
    long long N=value.size();
    if (N<=1) return 0;
    register double sum=0;
    long long NN=0;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i])) {
            sum=sum+(double)(value[i]);
            NN++;
        }
    }
    return sum/(double)NN;
}


/*! \brief calculate the number of elements in \a value that contain a valid float
    \ingroup qf3lib_mathtools

    \f[ \overline{v}=\cdot\sum\limits_{i=0}^{N-1} v_i \f]
*/
template <class T>
long long qfstatisticsCount(const T value) {
    long long N=value.size();
    if (N<=0) return 0;
    long long NN=0;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i])) {
            NN++;
        }
    }
    return NN;
}

/*! \brief calculate the minimum of the data in \a value
    \ingroup qf3lib_mathtools

*/
template <class T>
T qfstatisticsMin(const QList<T> value) {
    long long N=value.size();
    if (N<=0) return 0;
    if (N==1) return value[0];
    T m=0;
    bool first=true;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i])) {
            if (first)  {
                m=value[i];
                first=false;
            } else if (value[i]<m) {
                m=value[i];
            }
        }
    }
    return m;
}


/*! \brief calculate the maximum of the data in \a value
    \ingroup qf3lib_mathtools

*/
template <class T>
T qfstatisticsMax(const QList<T> value) {
    long long N=value.size();
    if (N<=0) return 0;
    if (N==1) return value[0];
    T m=0;
    bool first=true;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i])) {
            if (first)  {
                m=value[i];
                first=false;
            } else if (value[i]>m) {
                m=value[i];
            }
        }
    }
    return m;
}

/*! \brief calculate the sum of data
    \ingroup qf3lib_mathtools

    \f[ \overline{v}=\cdot\sum\limits_{i=0}^{N-1} v_i \f]
*/
template <class T>
double qfstatisticsSum(const T value) {
    long long N=value.size();
    if (N<=1) return 0;
    register double sum=0;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i])) {
            sum=sum+(double)(value[i]);
        }
    }
    return sum;
}
/*! \brief calculate the sum of data squares
    \ingroup qf3lib_mathtools

    \f[ \overline{v}=\cdot\sum\limits_{i=0}^{N-1} v_i^2 \f]
*/
template <class T>
double qfstatisticsSum2(const T value) {
    long long N=value.size();
    if (N<=1) return 0;
    register double sum=0;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i])) {
            sum=sum+(double)(value[i])*(double)(value[i]);
        }
    }
    return sum;
}

/*! \brief calculate the average and variance of a given array
    \ingroup qf3lib_mathtools

    \f[ \overline{v}=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot v_i}{\sum\limits_{i=0}^{N-1}w_i} \f]
    \f[ \text{Var}(v)=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot (v_i-\overline{v})^2}{\sum\limits_{i=0}^{N-1}w_i} \f]

*/
template <class T>
double qfstatisticsVariance(const T value) {
    long long N=value.size();
    if (N<=1) return 0;
    register double sum=0;
    register double sum2=0;
    long long NN=0;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i])) {
            sum2=sum2+(double)(value[i]*value[i]);
            sum=sum+(double)(value[i]);
            NN++;
        }
    }
    return ( sum2 - sum*sum/(double)NN ) / (double)(NN-1);
}









/*! \brief return the smallest element from a sorted array
    \ingroup tools_math_stat
*/
template <typename T>
typename T::value_type qfstatisticsSortedMin(T input) {
    if (input.size()<=0) return 0;
    return input[0];
}


/*! \brief return the largest element from a sorted array
    \ingroup tools_math_stat
*/
template <typename T>
typename T::value_type qfstatisticsSortedMax(T input) {
    if (input.size()<=0) return 0;
    return input[input.size()-1];
}


/*! \brief return the median from a sorted array
    \ingroup tools_math_stat

    if \a N is odd, then the center element is returned, otherwise the function returns the average of the two centering elements
*/
template <typename T>
typename T::value_type qfstatisticsSortedMedian(T input) {
    long long N=input.size();
    if (N<=0) return 0;
    if (N==1) return input[0];
    if (N%2==0) {
        return (input[N/2-1]+input[N/2])/2.0;
    } else {
        return input[N/2];
    }
}


/*! \brief return the median from a array
    \ingroup tools_math_stat

    if \a N is odd, then the center element is returned, otherwise the function returns the average of the two centering elements
*/
template <typename T>
typename T::value_type qfstatisticsMedian(T input_in) {
    T input=input_in;
    qSort(input);
    return qfstatisticsSortedMedian(input);
}


/*! \brief return the given quantile from a sorted array
    \ingroup tools_math_stat

*/
template <typename T>
typename T::value_type qfstatisticsSortedQuantile(T input, double quantile) {
    long long N=input.size();
    if (N<=0) return 0;
    if (N==1) return input[0];

    long long NQ=(long long)ceil(quantile*(double)N);
    if (NQ<0) NQ=0;
    if (NQ>=N) NQ=N-1;

    return input[NQ];
}


/*! \brief return the given quantile from an array
    \ingroup tools_math_stat

*/
template <typename T>
typename T::value_type qfstatisticsQuantile(T input_in, double quantile) {
    T input=input_in;
    qSort(input);
    return qfstatisticsSortedQuantile(input, quantile);
}










/*! \brief round a value according to the error
    \ingroup qf3lib_mathtools

    this returns a rounded value that equals the input value with \a addSignifcant mor
    significant digits than the error

    So with \a addSignifcant = 2 The value 1.23456789&pm;0.001
    will be rounded to 1.23456&pm;0.001
*/
QFLIB_EXPORT double roundWithError(double value, double error, int addSignifcant=1) ;

/*! \brief round a value to its first 1+\a addSignifcant significant digits
    \ingroup qf3lib_mathtools

*/
QFLIB_EXPORT double roundError(double error, int addSignifcant=1) ;

/** \brief used as result type for the function calcFitStatistics()
 *  \ingroup qf3lib_mathtools
 */
struct QFLIB_EXPORT QFFitStatistics {
    public:
        /** \brief default constructor/initializor: initialize all with 0/\c NULL */
        QFFitStatistics();

        /** \brief free all heap memory allocated in this struct */
        void free();

        int runAvgStart;   /**<  */
        double* fitfunc;   /**<  */
        double* residuals;   /**<  */
        double* residuals_weighted;   /**<  */
        int runAvgMaxN;   /**<  */
        int runAvgN;   /**<  */
        double* tau_runavg;   /**<  */
        double* residuals_runavg;   /**<  */
        double* residuals_runavg_weighted;   /**<  */
        int fitparamN;   /**<  */
        int dataSize;   /**<  */
        int degFreedom;   /**<  */
        double residSqrSum;        /**<  sum of squared residuals */
        double residWeightSqrSum;  /**<  sum of squared weighted residuals */
        double residSum;           /**<  sum of residuals */
        double residWeightSum;     /**<  sum of weightedresiduals */
        double gSum;               /**<  sum of measured values */
        double gSqrSum;            /**<  sum of squared measured values */


        double rmin;       /**<  min of residuals */
        double rmax;       /**<  max of residuals */
        double rminw;      /**<  min of weighted residuals */
        double rmaxw;      /**<  max of weighted residuals */
        double residAverage;   /**<  */
        double residWeightAverage;   /**<  */
        double residStdDev;   /**<  */
        double residWeightStdDev;   /**<  */
        double TSS;   /**<  */
        double Rsquared;   /**<  */

        double residHistBinWidth;   /**<  */
        double residHistWBinWidth;   /**<  */
        double* resHistogram;   /**<  */
        double resHistogramCount;   /**<  */
        double* resWHistogram;   /**<  */
        double resWHistogramCount;   /**<  */
        double* resCorrelation;   /**<  */
        double* resWCorrelation;   /**<  */
        int resN;   /**<  */
};

/*! \brief calculate fit statistics for the given measurement dataset, using this fit function with the given parameters

    \return a wide set of fit statistics in a struct of type FitStatistics
    \param N number of datapoints in corrdata and weights
    \param tauvals input data set x (the values f(x) are given in corrdata)
    \param corrdata model function evaluated for \a tauvals
    \param corrdata measurement dataset f(x)
    \param weights weights for the input dataset
    \param datacut_min first data point to use
    \param datacut_max last datapoint to use
    \param fullParams parameter vector
    \param errors errors of the fit parameters in fullParams
    \param paramsFix which parameters are fixed
    \param runAvgWidth width of the averaging in the running averge
    \param residualHistogramBins bins in the residual histogram

    \note the arrays in the resulting struct are allocated using \c malloc(), so you will have to free them using \c free() !!!
  */
QFLIB_EXPORT QFFitStatistics calculateFitStatistics(long N, double* tauvals, double* model, double* corrdata, double* weights, int datacut_min, int datacut_max, int paramCount, int runAvgWidth, int residualHistogramBins);

#endif // QFMATHTOOLS_H
