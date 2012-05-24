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

/*! \brief check whether the dlotaing point number is OK (i.e. non-inf, non-NAN)
    \ingroup qf3lib_mathtools
 */
#define QFFloatIsOK(v) (std::isfinite(v) && (!std::isinf(v)) && (!std::isnan(v)))

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

    \f[ \overline{v}=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot v_i}{\sum\limits_{i=0}^{N-1}w_i} \f]
    \f[ \text{Var}(v)=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot (v_i-\overline{v})^2}{\sum\limits_{i=0}^{N-1}w_i} \f]

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
