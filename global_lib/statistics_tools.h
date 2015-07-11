/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef STATISTICS_TOOLS_H
#define STATISTICS_TOOLS_H
#include <stdint.h>
#include <cmath>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <limits>
#include <vector>
#include <utility>
#include <cfloat>
#include <ostream>
#include <iomanip>
#include <sstream>

#ifdef _OPENMP
# include <omp.h>
#endif



#include "lib_imexport.h"
#ifndef __LINUX__
# if defined(linux)
#  define __LINUX__
# endif
#endif

#ifdef __LINUX__
#include "malloc.h"
#include "stdlib.h"
#endif

#ifndef QF_ALIGNMENT_BYTES
#define QF_ALIGNMENT_BYTES 32
#endif


#ifdef STATISTICS_TOOLS_MAY_USE_EIGEN3
#  include <Eigen/Core>
#  include <Eigen/LU>
#  include <Eigen/QR>
#endif

#ifdef STATISTICS_TOOLS_USE_QFTOOLS_H
#  include "qftools.h"
#endif

//#if !defined(QF_DONT_USE_ALIGNED_MALLOC)
//#warning("using aligned _aligned_malloc, _aligned_free, _aligned_realloc")
//#else
//#warning("using non-aligned malloc, free, realloc")
//#endif






/*!
    \defgroup tools_math_basic Basic Math Functions
    \ingroup tools
    \defgroup tools_math_array Data Array Tools
    \ingroup tools
    \defgroup tools_math_stat Statistics Tools
    \ingroup tools
    \defgroup tools_math_linalg Linear Algebra Tools
    \ingroup tools

  */

/*! \brief malloc() for use herein (aligned on some systems!)
    \ingroup tools_math_array

*/
inline void* statisticsMalloc(size_t size) {
//    std::cout<<"statisticsMalloc("<<size<<")\n";
#ifdef STATISTICS_TOOLS_USE_QFTOOLS_H
    return qfMalloc(size);
#else
    if (size<=0) return NULL;
  #ifdef __LINUX__
    #if  !defined(QF_DONT_USE_ALIGNED_MALLOC)
    return aligned_alloc(QF_ALIGNMENT_BYTES, size);
    #else
    return malloc(size);
    #endif
  #else
    #if  !defined(QF_DONT_USE_ALIGNED_MALLOC)
    return _aligned_malloc(size, QF_ALIGNMENT_BYTES);
    #else
    return malloc(size);
    #endif
  #endif
#endif
}

/*! \brief calloc() for use herein (aligned on some systems!)
    \ingroup tools_math_array

*/
inline void* statisticsCalloc(size_t num, size_t size) {
//    std::cout<<"statisticsCalloc("<<num<<", "<<size<<")\n";
#ifdef STATISTICS_TOOLS_USE_QFTOOLS_H
    return qfCalloc(num,size);
#else
    if (size*size<=0) return NULL;
    void* res=statisticsMalloc(num*size);
    memset(res, 0, num*size);
    return res;
#endif
}

/*! \brief free() for use herein (aligned on some systems!)
    \ingroup tools_math_array

*/
inline void statisticsFree(void* data) {
#ifdef STATISTICS_TOOLS_USE_QFTOOLS_H
    qfFree(data);
#else
    if (!data) return;
    #ifdef __LINUX__
        #if !defined(QF_DONT_USE_ALIGNED_MALLOC)
        free(data);
        #else
        free(data);
        #endif
    #else
        #if !defined(QF_DONT_USE_ALIGNED_MALLOC)
        _aligned_free(data);
        #else
        free(data);
        #endif
    #endif
#endif
}






/*! \brief convert an integer to a std::string
    \ingroup tools_math_basic

*/
inline std::string linalgIntToStr(int data){
  std::ostringstream ost;
  ost<<data;
  return ost.str();
}





/*! \brief tests for a good float (not NAN, not INF and finite)
    \ingroup tools_math_basic

*/
#define statisticsFloatIsOK(v) (std::isfinite(v) && (!std::isinf(v)) && (!std::isnan(v)))

/*! \brief \f$ \sqrt{\pi}=2.50662827463 \f$
    \ingroup tools_math_basic

*/
#define STATISTICS_SQRT_2PI 2.50662827463

/*! \brief minimum of two numbers
    \ingroup tools_math_basic

*/
template <class T>
inline T statisticsMin(T v1, T v2) {
    if (v1<v2) return v1;
    return v2;
}

/*! \brief 4-th power of a number
    \ingroup tools_math_basic

*/
template <class T>
inline T statisticsPow4(T x) {
    const T xx=x*x;
    return xx*xx;
}

/*! \brief cube of a number
    \ingroup tools_math_basic

*/
template <class T>
inline T statisticsCube(T x) {
    return x*x*x;
}

/*! \brief square of a number
    \ingroup tools_math_basic

*/
template <class T>
inline T statisticsSqr(T x) {
    return x*x;
}

/*! \brief maximum of two numbers
    \ingroup tools_math_basic

*/
template <class T>
inline T statisticsMax(T v1, T v2) {
    if (v1>v2) return v1;
    return v2;
}


/*! \brief swap two elements \a l and \a r in an array \a a
    \ingroup tools_math_basic

*/
template <class T>
inline void statisticsSwap(T* a, long long l, long long r){
    const T tmp=a[l];
    a[l]=a[r];
    a[r]=tmp;
}

/*! \brief swap two elements \a l and \a r in an array \a a
    \ingroup tools_math_basic

*/
template <class T>
inline void statisticsSwapV(std::vector<T>& a, long long l, long long r){
    const T tmp=a[l];
    a[l]=a[r];
    a[r]=tmp;
}

/*! \brief calculates the sign of number \a x
    \ingroup tools_math_basic
*/
template <class T>
inline T statisticsSign(T x) {
    if (x<0) return -1;
    //else if (x==0) return 0;
    else return 1;
}

/*! \brief duplicate an array of data
    \ingroup tools_math_array

    \note use statisticsFree() to free the memory!!!
*/
template <class T>
inline T* statisticsDuplicateArray(const T* dataIn, long long N) {
//    std::cout<<"statisticsDuplicateArray("<<dataIn<<", "<<N<<")\n";
    if (N<=0 || !dataIn) return NULL;
    T* out=(T*)statisticsMalloc(N*sizeof(T));
    if (out) memcpy(out, dataIn, N*sizeof(T));
    return out;
}

/** \brief returns \c true, if all elements of \a dataIn equal \a dataEqual
 * \ingroup tools_math_array
 * */
template <class T>
inline bool statisticsIsAllEqualIgnoreNan(const T* dataIn, long long N, T dataEqual) {
    if (!dataIn || N<=0) return false;
    long long cnt=0;
    for (long long i=0; i<N; i++) {
        if (statisticsFloatIsOK(dataIn[i])) {
            if (dataIn[i]!=dataEqual) return false;
            cnt++;
        }
    }
    return cnt>0;
}

/** \brief returns \c true, if all elements of \a dataIn equal \a dataEqual
 * \ingroup tools_math_array
 * */
template <class T>
inline bool statisticsIsAllEqual(const T* dataIn, long long N, T dataEqual) {
    if (!dataIn || N<=0) return false;
    for (long long i=0; i<N; i++) {
        if ((dataIn[i]!=dataEqual) || (!statisticsFloatIsOK(dataIn[i]))) return false;
    }
    return true;
}

/*! \brief calculates the average of a given timeseries \a data
    \ingroup tools_math_stat

    \param[in] data input timeseries \f$ X_i \f$ of length \a N
    \param[in] N size of input data \a data
    \return average of \a data

    This function implements:
      \f[ \overline{X}=\frac{1}{N}\cdot\sum\limits_{i=0}^{N-1}X_i \f]
*/
template <class T>
inline double statisticsAverage(const T* data, long long N) {
    if (!data) return 0;
    if (N<=0) return 0;
    register double sum=0;
    long long NN=0;
    register long long i;
    //#pragma omp simd reduction(+:sum)
    for (i=0; i<N; i++) {
        register const double v=data[i];
        if (statisticsFloatIsOK(v)) {
            sum=sum+v;
            NN++;
        }
    }
    return sum/(double)NN;
}


/*! \brief calculates the sum of a given timeseries \a data
    \ingroup tools_math_stat

    \param[in] data input timeseries \f$ X_i \f$ of length \a N
    \param[in] N size of input data \a data
    \return average of \a data

    This function implements:
      \f[ \overline{X}=\sum\limits_{i=0}^{N-1}X_i \f]
*/
template <class T>
inline double statisticsSum(const T* data, long long N) {
    if (!data) return 0;
    if (N<=0) return 0;
    register double sum=0;
    long long NN=0;
    register long long i;
    //#pragma omp simd reduction(+:sum)
    for (i=0; i<N; i++) {
        register const double v=data[i];
        if (statisticsFloatIsOK(v)) {
            sum=sum+v;
        }
    }
    return sum;
}
/*! \brief calculate the weighted average of a given array

    \f[ \overline{v}=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot v_i}{\sum\limits_{i=0}^{N-1}w_i} \f]
*/
template <class T>
inline double statisticsAverage(const T* weights, const T* value, long long N) {
    if ((!value)||(!weights)) return 0;
    if (N<=0) return 0;
    register double sum=0;
    register double sum2=0;
    register long long i;
    //#pragma omp simd reduction(+:sum) reduction(+:sum2)
    for (i=0; i<N; i++) {
        register const T v=value[i];
        if (statisticsFloatIsOK(v)) {
            sum=sum+(double)weights[i];
            sum2=sum2+(double)(v*weights[i]);
        }
    }
    return sum2/sum;
}

/*! \brief calculate the weighted average of a given array

    \f[ \overline{v}=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot v_i}{\sum\limits_{i=0}^{N-1}w_i} \f]
*/
template <class T>
inline double statisticsAverageMasked(const bool* mask, const T* value, long long N, bool maskUseValue=true) {
    //std::cout<<"statisticsAverageMasked:  "<<value<<!value<<mask<<!mask<<std::endl;
    //std::cout.flush();
    if ((!value)||(!mask)) return 0;
    if (N<=0) return 0;
    register double sum=0;
    register double sum2=0;
    register long long i;
    //#pragma omp simd reduction(+:sum) reduction(+:sum2)
    for (i=0; i<N; i++) {
        register const T v=value[i];
        if (statisticsFloatIsOK(v)) {
            const T w=(mask[i]==maskUseValue)?1:0;
            sum=sum+(double)w;
            sum2=sum2+(double)(v*w);
        }
    }
    return sum2/sum;
}




/*! \brief calculate weighted variance of a given array

    \f[ \text{Var}(v)=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot (v_i-\overline{v})^2}{\sum\limits_{i=0}^{N-1}w_i} \f]
*/
template <class T>
inline double statisticsVariance(const T* weights, const T* value, long long N) {
    if ((!value)||(!weights)) return 0;
    if (N<=1) return 0;
    register double avg=statisticsAverage(weights, value, N);
    register double sum=0;
    register double sum2=0;
    register long long i;
    //#pragma omp simd reduction(+:sum) reduction(+:sum2)
    for (i=0; i<N; i++) {
        register const T v=value[i]-avg;
        if (statisticsFloatIsOK(v)) {
            sum=sum+(double)weights[i];
            sum2=sum2+(double)(v*v*weights[i]);
        }
    }
    if (sum==0 || !statisticsFloatIsOK(sum2)  || sum2<0.0) return 0;
    return sum2/sum;
}

/*! \brief calculate weighted standard deviation of a given array

    \f[ \sigma(v)=\sqrt{\frac{\sum\limits_{i=0}^{N-1}w_i\cdot (v_i-\overline{v})^2}{\sum\limits_{i=0}^{N-1}w_i}} \f]
*/
template <class T>
inline double statisticsStdDev(const T* weights, const T* value, long long N) {
    return sqrt(statisticsVariance(weights, value, N));
}

/*! \brief calculate  variance of a given array

    \f[ \text{Var}(v)=\frac{\sum\limits_{i=0}^{N-1}(v_i-\overline{v})^2}{N-1} \f]
*/
template <class T>
inline double statisticsVariance(const T* value, long long N) {
    if (!value) return 0;
    if (N<=1) return 0;
    register double sum=0;
    register double sum2=0;
    long long NN=0;
    register long long i;
    //#pragma omp simd reduction(+:sum) reduction(+:sum2)
    for (i=0; i<N; i++) {
        register const T v=value[i];
        if (statisticsFloatIsOK(v)) {
            sum2=sum2+(double)(v*v);
            sum=sum+(double)(v);
            NN++;
        }
    }
    const double v=( sum2 - sum*sum/(double)NN ) / (double)(NN-1);
    if (statisticsFloatIsOK(v) && v>=0) {
        return v;
    } else {
        return 0;
    }
}


template <class T>
inline double statisticsVarianceMasked(const bool* mask, const T* value, long long N, bool maskUseValue=true) {
    if (!mask) return statisticsVariance(value, N);
    if (!value) return 0;
    if (N<=1) return 0;
    register double sum=0;
    register double sum2=0;
    long long NN=0;
    register long long i;
    //#pragma omp simd reduction(+:sum) reduction(+:sum2)
    for (i=0; i<N; i++) {
        register const T v=value[i];
        if (statisticsFloatIsOK(v) && maskUseValue==mask[i]) {
            sum2=sum2+(double)(v*v);
            sum=sum+(double)(v);
            NN++;
        }
    }
    const double v=( sum2 - sum*sum/(double)NN ) / (double)(NN-1);
    if (statisticsFloatIsOK(v) && v>=0) {
        return v;
    } else {
        return 0;
    }
}

/*! \brief calculate weighted standard deviation of a given array

    \f[ \sigma(v)=\sqrt{\frac{\sum\limits_{i=0}^{N-1}(v_i-\overline{v})^2}{N-1}} \f]
*/
template <class T>
inline double statisticsStdDev(const T* value, long long N) {
    return sqrt(statisticsVariance(value, N));
}



/*! \brief calculate the average and variance of a given array

    \f[ \overline{v}=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot v_i}{\sum\limits_{i=0}^{N-1}w_i} \f]
    \f[ \text{Var}(v)=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot (v_i-\overline{v})^2}{\sum\limits_{i=0}^{N-1}w_i} \f]

*/
template <class T>
inline double statisticsAverageVariance(double& var, const T* value, long long N) {
    if (!value) return 0;
    if (N<=1) return 0;
    register double sum=0;
    register double sum2=0;
    long long NN=0;
    register long long i;
    //#pragma omp simd reduction(+:sum) reduction(+:sum2)
    for (i=0; i<N; i++) {
        register const T v=value[i];
        if (statisticsFloatIsOK(v)) {
            sum2=sum2+(double)(v*v);
            sum=sum+(double)(v);
            NN++;
        }
    }
    var= ( sum2 - sum*sum/(double)NN ) / (double)(NN-1);
    if (NN<=1 || var<0.0 || !statisticsFloatIsOK(var)) var=0;
    return sum/(double)NN;
}

/*! \brief calculate the weighted average and variance of a given array

    \f[ \overline{v}=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot v_i}{\sum\limits_{i=0}^{N-1}w_i} \f]
    \f[ \text{Var}(v)=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot (v_i-\overline{v})^2}{\sum\limits_{i=0}^{N-1}w_i} \f]

*/
template <class T>
inline double statisticsAverageVariance(double& var, const T* weights, const T* value, long long N) {
    if ((!value)||(!weights)) return 0;
    if (N<=1) return 0;
    register double avg=statisticsAverage(weights, value, N);
    register double sum=0;
    register double sum2=0;
    register long long i;
    //#pragma omp simd reduction(+:sum) reduction(+:sum2)
    for (i=0; i<N; i++) {
        register const T v=value[i]-avg;
        if (statisticsFloatIsOK(v)) {
            sum=sum+(double)weights[i];
            sum2=sum2+(double)(v*v*weights[i]);
        }
    }
    var=sum2/sum;
    if (!statisticsFloatIsOK(var) || var<=0.0) {
        var=0;
    }
    return avg;
}

/*! \brief calculate the weighted average and variance of a given array

    \f[ \overline{v}=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot v_i}{\sum\limits_{i=0}^{N-1}w_i} \f]
    \f[ \text{Var}(v)=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot (v_i-\overline{v})^2}{\sum\limits_{i=0}^{N-1}w_i} \f]

*/
template <class T>
inline double statisticsAverageVarianceMasked(double& var, const bool* mask, const T* value, long long N, bool maskUseValue=true) {
    if ((!value)||(!mask)) return 0;
    if (N<=1) return 0;
    register double avg=statisticsAverageMasked(mask, value, N, maskUseValue);
    register double sum=0;
    register double sum2=0;
    register long long i;
    //#pragma omp simd reduction(+:sum) reduction(+:sum2)
    for (i=0; i<N; i++) {
        register const T v=value[i]-avg;
        if (statisticsFloatIsOK(v)) {
            const T w=(mask[i]==maskUseValue)?1:0;
            sum=sum+w;
            sum2=sum2+(double)(v*v*w);
        }
    }
    var=sum2/sum;
    if (!statisticsFloatIsOK(var) || var<=0.0) {
        var=0;
    }
    return avg;
}

/*! \brief calculate the skewness of a dataset

    \f[ \gamma_1=\mathbb{E}\left[\left(\frac{X-\mu}{\sigma}\right)^3\right]= \frac{m_3}{m_2^{3/2}}  = \frac{\frac{1}{n} \sum_{i=1}^n (x_i-\overline{x})^3}{\left(\frac{1}{n} \sum_{i=1}^n (x_i-\overline{x})^2\right)^{3/2}} \f]
    where \f$\mu\f$ is the mean and \f$\sigma\f$ the standard deviation of a random variable \f$X\f$ and \f$\overline{x}\f$ is the average (calculated using statisticsAverage() ) of
    the input dataset \f$ x_i\f$.
*/
template <class T>
inline double statisticsSkewness(const T* value, long long N, double* average=NULL) {
    if ((!value)) return 0;
    if (N<=1) return 0;
    register double avg=statisticsAverage(value, N);
    register double sum3=0;
    register double sum2=0;
    register double NN=0;
    register long long i;
    //#pragma omp simd reduction(+:sum3) reduction(+:sum2)
    for (i=0; i<N; i++) {
        register const T v=value[i]-avg;
        if (statisticsFloatIsOK(v)) {
            sum3=sum3+(double)(v*v*v);
            sum2=sum2+(double)(v*v);
            NN++;
        }
    }
    if (average) *average=avg;
    const double down=statisticsCube(sum2/double(NN));
    return sum3/double(NN)/sqrt(down);
}

/*! \brief calculate the given central  moment

    \f[ \langle X^n\rangle= \mathbb{E}\left[\left(X-\mu\right)^n\right] \f]
    where \f$\mu\f$ is the mean of a random variable \f$X\f$ and \f$\overline{x}\f$ is the average (calculated using statisticsAverage() ) of
    the input dataset \f$ x_i\f$.
*/
template <class T>
inline double statisticsCentralMoment(const T* value, long long N, int order) {
    if ((!value)) return 0;
    if (N<=1) return 0;
    register double avg=statisticsAverage(value, N);
    register double sum=0;
    register double NN=0;
    register long long i;
    //#pragma omp simd reduction(+:sum)
    for (i=0; i<N; i++) {
        register const T v=value[i]-avg;
        if (statisticsFloatIsOK(v)) {
            sum=sum+pow(v, order);
            NN++;
        }
    }
    return sum/double(NN);
}


/*! \brief calculate the given non-central  moment

    \f[ \langle X^n\rangle= \mathbb{E}\left[X^n\right] \f]
    where \f$\mu\f$ is the mean of a random variable \f$X\f$ and \f$\overline{x}\f$ is the average (calculated using statisticsAverage() ) of
    the input dataset \f$ x_i\f$.
*/
template <class T>
inline double statisticsMoment(const T* value, long long N, int order) {
    if ((!value)) return 0;
    if (N<=1) return 0;
    register double sum=0;
    register double NN=0;
    register long long i;
    //#pragma omp simd reduction(+:sum)
    for (i=0; i<N; i++) {
        register const T v=value[i];
        if (statisticsFloatIsOK(v)) {
            sum=sum+pow(v, order);
            NN++;
        }
    }
    return sum/double(NN);
}


/*! \brief calculate empirical (Pearson's) correlation coefficient

    \f[ \text{Kor}(x,y)=\frac{\sum\limits_{i=0}^{N-1}(x_i-\overline{x})(y_i-\overline{y})}{\sqrt{\sum\limits_{i=0}^{N-1}(x_i-\overline{x})^2\cdot\sum\limits_{i=0}^{N-1}(y_i-\overline{y})^2}} \f]
*/
template <class T>
inline double statisticsCorrelationCoefficient(const T* dataX, const T* dataY, long long N) {
    if (!dataX || !dataY) return 0;
    if (N<=1) return 0;
    register double xbar=0;
    register double ybar=0;
    register double NN=0;
    for (long long i=0; i<N; i++) {
        const double xm=dataX[i];
        const double ym=dataY[i];
        if (statisticsFloatIsOK(xm) && statisticsFloatIsOK(ym)) {
            xbar=xbar+xm;
            ybar=ybar+ym;
            NN++;
        }
    }
    xbar=xbar/NN;
    ybar=ybar/NN;
    register double sumxy=0;
    register double sumx=0;
    register double sumy=0;
    for (long long i=0; i<N; i++) {
        const double xm=dataX[i]-xbar;
        const double ym=dataY[i]-ybar;
        if (statisticsFloatIsOK(xm) && statisticsFloatIsOK(ym)) {
            sumxy=sumxy+xm*ym;
            sumx=sumx+xm*xm;
            sumy=sumy+ym*ym;
        }
    }
    return sumxy/sqrt(sumx*sumy);
}



/*! \brief calculate empirical Manders overlap coefficient

    \f[ \mbox{MOC}_{G,R}=\frac{\sum_i\left(G_i\cdot R_i\right)}{\sqrt{\left[\sum_iG_i^2\right]\cdot\left[\sum_iR_i^2\right]}} \f]
*/
template <class T>
inline double statisticsMandersOverlapCoefficient(const T* dataX, const T* dataY, long long N) {
    if (!dataX || !dataY) return 0;
    if (N<=1) return 0;
    register double sumxy=0;
    register double sumx=0;
    register double sumy=0;
    for (long long i=0; i<N; i++) {
        const double xm=dataX[i];
        const double ym=dataY[i];
        if (statisticsFloatIsOK(xm) && statisticsFloatIsOK(ym)) {
            sumxy=sumxy+xm*ym;
            sumx=sumx+xm*xm;
            sumy=sumy+ym*ym;
        }
    }
    return sumxy/sqrt(sumx*sumy);
}


/** \brief calculate min/max of a given array */
template <class T>
inline void statisticsMinMax(const T* data, long long N, T& min, T& max) {
    if (!data) return ;
    if (N<=0) return;
    min=data[0];
    max=data[0];
    bool first=true;
    for (register long long i=1; i<N; i++) {
        register const T v=data[i];
        if (statisticsFloatIsOK(v)) {
            if (first) {
                min=v;
                max=v;
                first=false;
            } else {
                if (v<min) min=v;
                if (v>max) max=v;
            }
        }
    }
}

/** \brief calculate min/max of a given array, only taking into account the pixels for which \c mask[i]==maskUseValue */
template <class T>
inline void statisticsMaskedMinMax(const T* data, const bool* mask, long long N, T& min, T& max, bool maskUseValue=false) {
    if (!mask) {
        statisticsMinMax(data, N, min, max);
    } else {
        if (N<=0) return;
        if (data==NULL || mask==NULL) return;
        bool first=true;
        min=0;
        max=0;
        for (register long long i=0; i<N; i++) {
            if (mask[i]==maskUseValue) {
                register const T v=data[i];
                if (statisticsFloatIsOK(v)) {
                    if (first) {
                        min=v;
                        max=v;
                        first=false;
                    } else {
                        if (v<min) min=v;
                        if (v>max) max=v;
                    }
                }
            }
        }
    }
}

/** \brief calculate min/max of a given array, only taking into account the pixels for which \c mask[i]==maskUseValue */
template <class T>
inline void statisticsModMaskedMinMax(const T* data, const bool* mask, long long N, long long Nmask, T& min, T& max, bool maskUseValue=false) {
    if (!mask) {
        statisticsMinMax(data, N, min, max);
    } else {
        if (N<=0) return;
        if (data==NULL || mask==NULL) return;
        bool first=true;
        min=0;
        max=0;
        for (register long long i=0; i<N; i++) {
            if (mask[i%Nmask]==maskUseValue) {
                register const T v=data[i];
                if (statisticsFloatIsOK(v)) {
                    if (first) {
                        min=v;
                        max=v;
                        first=false;
                    } else {
                        if (v<min) min=v;
                        if (v>max) max=v;
                    }
                }
            }
        }
    }
}
/** \brief calculate min of a given array */
template <class T>
inline T statisticsMin(const T* data, long long N, long long* minpos=NULL) {
    if (!data) return 0;
    if (N<=0) return 0;
    T min=data[0];
    long long mp=0;
    for (register long long i=1; i<N; i++) {
        if (data[i]<min) {
            min=data[i];
            mp=i;
        }
    }
    if (minpos) *minpos=mp;
    return min;
}

/** \brief calculate max of a given array */
template <class T>
inline T statisticsMax(const T* data, long long N, long long* maxpos=NULL) {
    if (!data) return 0;
    if (N<=0) return 0;
    T max=data[0];
    long long mp=0;
    for (register long long i=1; i<N; i++) {
        if (data[i]>max) {
            max=data[i];
            mp=i;
        }
    }
    if (maxpos) *maxpos=mp;
    return max;
}


/*! \brief calculates the autocorrelation function of a given timeseries \a data
    \ingroup tools_math_stat

    \param[out] dataout the autocorreltion function \f$ \hat{\gamma}(k) \f$ is written into this array, which has to have length \c ceil(N/2)
    \param[in] data input timeseries \f$ X_i \f$ of length \a N
    \param[in] N size of input data \a data

    This function implements:
      \f[ \hat{\gamma}(k)=\frac{1}{N}\cdot\sum\limits_{i=0}^{N-k-1}(X_i-\overline{X})\cdot(X_{i+k}-\overline{X}) \f]
    Where \f$ \overline{X} \f$ is an estimator of the average (internally statisticsAverage() is called ):
      \f[ \overline{X}=\frac{1}{N}\cdot\sum\limits_{i=0}^{N-1}X_i \f]
 */
LIB_EXPORT void statisticsAutocorrelate(double* dataout, const double* data, uint32_t N);

/*! \brief the same as statisticsAutocorrelate(), but the memory for the autocorrelation is allocated using malloc() and a pointer is returned
    \see statisticsAutocorrelate()
 */
LIB_EXPORT double* statisticsAllocAutocorrelate(const double* data, long long N);



/*! \brief calculates the multi-tau autocorrelation function of a given timeseries \a data, using symmetric normalization
    \ingroup tools_math_stat

    \param[out] dataout the autocorreltion function \f$ \hat{\gamma}(k) \f$ is written into this array, which has to have length \c Ntau
    \param[in] data1 input timeseries \f$ X_i \f$ of length \a N
    \param[in] data2 input timeseries \f$ Y_i \f$ of length \a N
    \param[in] N size of input data \a data
    \param[in] tau array containing the lag times (in increasing order) at which to evaluate the correlation function
    \param[in] Ntau length of the array \a tau
    \param[in] stride stride to go over \a data1 and \a data2

    This function implements:
      \f[ \hat{g}(\tau)=\frac{(T-\tau)\cdot\sum\limits_{i=\tau}^{N}X_i\cdot Y_{i-\tau}}{\left[\sum\limits_{i=\tau}^{N}X_i\right]\cdot\left[\sum\limits_{i=\tau}^{N}Y_{i-\tau}\right]} \f]

    You can use statisticsAutocorrelateCreateMultiTau() to generate an array of tau values to be used with this function.
 */
template <class T, class Tsumming>
inline void statisticsCrosscorrelateMultiTauSymmetric(double* dataout, const T* data1, const T* data2, uint32_t N, const long* tau, uint32_t Ntau, uint32_t stride=1) {

    for (uint32_t i=0; i<Ntau; i++) {
        const long ctau=tau[i];
        register Tsumming sum=0;
        register Tsumming sum_delayed=0;
        register Tsumming msd=0;
        if (ctau<long(N)) {
            register uint64_t j;
            //#pragma omp simd reduction(+:msd) reduction(+:sum)
            for (j=ctau; j<N; j++) {
                sum=sum+double(data1[j*stride]);
                sum_delayed=sum_delayed+Tsumming(data2[(j-ctau)*stride]);
                msd=msd+Tsumming(data1[j*stride])*Tsumming(data2[(j-ctau)*stride]);
            }
            dataout[i]=(double)(N-ctau)*(double)msd/(double)(sum)/double(sum_delayed);
        } else {
            dataout[i]=1.0;
        }
    }

}



/*! \brief calculates the mean squared displacement (MSD) of a given 1D timeseries \a data_x using a time average
    \ingroup tools_math_stat

    \param[out] dataout the autocorreltion function \f$ \langle\hat{r}^2(\tau)\rangle \f$ is written into this array, which has to have length \c Ntau
    \param[in] data_x input timeseries \f$ X_i \f$ of length \a N
    \param[in] N size of input data \a data
    \param[in] tau array containing the lag times (in increasing order) at which to evaluate the MSD function
    \param[in] Ntau length of the array \a tau
    \param[in] stride stride to go over \a data_x

    This function implements:
      \f[ \langle\hat{r}^2(\tau)\rangle=\frac{1}{N-\tau}\cdot\sum\limits_{i=\tau}^{N}(X_i-X_{i-\tau})^2 \f]

    You can use statisticsAutocorrelateCreateMultiTau64() to generate an array of tau values to be used with this function.
 */
template <class T, class Tsumming>
inline void statisticsMSDMultiTau1D(double* dataout, const T* data_x, uint64_t N, const uint64_t* tau, uint32_t Ntau, uint32_t stride=1) {

    for (uint64_t i=0; i<Ntau; i++) {
        const uint64_t ctau=tau[i];
        register Tsumming msd=0;
        if (ctau<N) {
            register uint64_t j;
            //#pragma omp simd reduction(+:msd)
            for (j=ctau; j<N; j++) {
                register Tsumming r=Tsumming(data_x[j*stride])-Tsumming(data_x[(j-ctau)*stride]);
                msd=msd+(r*r);
            }
            dataout[i]=(double)msd/(double)(N-ctau);
        } else {
            dataout[i]=0.0;
        }
    }
}



/*! \brief calculates the mean squared displacement (MSD) of a given 2D timeseries \a data_x , \a data_y using a time average
    \ingroup tools_math_stat

    \param[out] dataout the autocorreltion function \f$ \langle\hat{r}^2(\tau)\rangle \f$ is written into this array, which has to have length \c Ntau
    \param[in] data_x input timeseries \f$ X_i \f$ of length \a N
    \param[in] data_y input timeseries \f$ Y_i \f$ of length \a N
    \param[in] N size of input data \a data
    \param[in] tau array containing the lag times (in increasing order) at which to evaluate the MSD function
    \param[in] Ntau length of the array \a tau
    \param[in] stride stride to go over \a data_x

    This function implements:
      \f[ \langle\hat{r}^2(\tau)\rangle=\frac{1}{N-\tau}\cdot\sum\limits_{i=\tau}^{N}(X_i-X_{i-\tau})^2+(Y_i-Y_{i-\tau})^2 \f]

    You can use statisticsAutocorrelateCreateMultiTau64() to generate an array of tau values to be used with this function.
 */
template <class T, class Tsumming, class Tout>
inline void statisticsMSDMultiTau2D(Tout* dataout, const T* data_x, const T* data_y, uint64_t N, const uint64_t* tau, uint32_t Ntau, uint32_t stride=1) {

    for (uint64_t i=0; i<Ntau; i++) {
        const uint64_t ctau=tau[i];
        register Tsumming msd=0;
        if (ctau<N) {
            register uint64_t j;
            //#pragma omp simd reduction(+:msd)
            for (j=ctau; j<N; j++) {
                register Tsumming r=Tsumming(data_x[j*stride])-Tsumming(data_x[(j-ctau)*stride]);
                register Tsumming ry=Tsumming(data_y[j*stride])-Tsumming(data_y[(j-ctau)*stride]);
                msd=msd+(r*r)+(ry*ry);
            }
            dataout[i]=(Tout)msd/(Tout)(N-ctau);
        } else {
            dataout[i]=0.0;
        }
    }
}



/*! \brief calculates the mean squared displacement (MSD) of a given 3D timeseries \a data_x , \a data_y , \a data_z using a time average
    \ingroup tools_math_stat

    \param[out] dataout the autocorreltion function \f$ \langle\hat{r}^2(\tau)\rangle \f$ is written into this array, which has to have length \c Ntau
    \param[in] data_x input timeseries \f$ X_i \f$ of length \a N
    \param[in] data_y input timeseries \f$ Y_i \f$ of length \a N
    \param[in] data_z input timeseries \f$ Z_i \f$ of length \a N
    \param[in] N size of input data \a data
    \param[in] tau array containing the lag times (in increasing order) at which to evaluate the MSD function
    \param[in] Ntau length of the array \a tau
    \param[in] stride stride to go over \a data_x

    This function implements:
      \f[ \langle\hat{r}^2(\tau)\rangle=\frac{1}{N-\tau}\cdot\sum\limits_{i=\tau}^{N}(X_i-X_{i-\tau})^2+(Y_i-Y_{i-\tau})^2+(Z_i-Z_{i-\tau})^2 \f]

    You can use statisticsAutocorrelateCreateMultiTau64() to generate an array of tau values to be used with this function.
 */
template <class T, class Tsumming>
inline void statisticsMSDMultiTau3D(double* dataout, const T* data_x, const T* data_y, const T* data_z, uint64_t N, const uint64_t* tau, uint32_t Ntau, uint32_t stride=1) {

    for (uint64_t i=0; i<Ntau; i++) {
        const uint64_t ctau=tau[i];
        register Tsumming msd=0;
        if (ctau<N) {
            register uint64_t j;
            //#pragma omp simd reduction(+:msd)
            for (j=ctau; j<N; j++) {
                register Tsumming r=Tsumming(data_x[j*stride])-Tsumming(data_x[(j-ctau)*stride]);
                register Tsumming ry=Tsumming(data_y[j*stride])-Tsumming(data_y[(j-ctau)*stride]);
                register Tsumming rz=Tsumming(data_z[j*stride])-Tsumming(data_z[(j-ctau)*stride]);
                msd=msd+(r*r)+(ry*ry)+(rz*rz);
            }
            dataout[i]=(double)msd/(double)(N-ctau);
        } else {
            dataout[i]=0.0;
        }
    }
}

/*! \brief calculates the multi-tau autocorrelation function of a given timeseries \a data, using symmetric normalization
    \ingroup tools_math_stat

    \param[out] dataout the autocorreltion function \f$ \hat{\gamma}(k) \f$ is written into this array, which has to have length \c Ntau
    \param[in] data input timeseries \f$ X_i \f$ of length \a N
    \param[in] N size of input data \a data
    \param[in] tau array containing the lag times (in increasing order) at which to evaluate the correlation function
    \param[in] Ntau length of the array \a tau
    \param[in] stride stride to go over \a data

    This function implements:
      \f[ \hat{g}(\tau)=\frac{(T-\tau)\cdot\sum\limits_{i=\tau}^{N}X_i\cdot X_{i-\tau}}{\left[\sum\limits_{i=\tau}^{N}X_i\right]\cdot\left[\sum\limits_{i=\tau}^{N}X_{i-\tau}\right]} \f]

    You can use statisticsAutocorrelateCreateMultiTau() to generate an array of tau values to be used with this function.
 */
template <class T, class Tsumming>
inline void statisticsAutocorrelateMultiTauSymmetric(double* dataout, const T* data, uint32_t N, const long* tau, uint32_t Ntau, uint32_t stride=1){
    statisticsCrosscorrelateMultiTauSymmetric<T, Tsumming>(dataout, data, data, N, tau, Ntau, stride);
}


/*! \brief calculates the multi-tau autocorrelation function of a given timeseries \a data, using symmetric normalization
    \ingroup tools_math_stat

    In contrast to statisticsAutocorrelateMultiTauSymmetric() this function first makes a copy of the
    input array. This may lead to better performance for strides larger than 1, due to better memory
    locality.

    \note Expect \c N*sizeof(T) bytes of extra memory consumption.

 */
template <class T, class Tsumming>
inline void statisticsAutocorrelateMultiTauSymmetricMemOptimized(double* dataout, const T* data, uint32_t N, const long* tau, uint32_t Ntau, uint32_t stride=1){
    if (stride>2) {
        T* d=(T*)statisticsMalloc(N*sizeof(T));
        for (register long i=0; i<N; i++) {
            d[i]=data[i*stride];
        }
        statisticsCrosscorrelateMultiTauSymmetric<T, Tsumming>(dataout, d, d, N, tau, Ntau, 1);
        statisticsFree(d);
    } else {
        statisticsCrosscorrelateMultiTauSymmetric<T, Tsumming>(dataout, data, data, N, tau, Ntau, stride);
    }
}




/*! \brief calculates the multi-tau autocorrelation function of a given timeseries \a data, using symmetric normalization
    \ingroup tools_math_stat

    In contrast to statisticsCrosscorrelateMultiTauSymmetric() this function first makes a copy of the
    input array. This may lead to better performance for strides larger than 1, due to better memory
    locality.

    \note Expect \c 2*N*sizeof(T) bytes of extra memory consumption.

 */
template <class T, class Tsumming>
inline void statisticsCrosscorrelateMultiTauSymmetricMemOptimized(double* dataout, const T* data, const T* data2, uint32_t N, const long* tau, uint32_t Ntau, uint32_t stride=1){
    if (stride>2) {
        T* d=(T*)statisticsMalloc(N*sizeof(T));
        T* d2=(T*)statisticsMalloc(N*sizeof(T));
        for (register uint32_t i=0; i<N; i++) {
            d[i]=data[i*stride];
            d2[i]=data2[i*stride];
        }
        statisticsCrosscorrelateMultiTauSymmetric<T, Tsumming>(dataout, d, d2, N, tau, Ntau, 1);
        statisticsFree(d);
        statisticsFree(d2);
    } else {
        statisticsCrosscorrelateMultiTauSymmetric<T, Tsumming>(dataout, data, data2, N, tau, Ntau, stride);
    }
}


/*! \brief blocks the data, i.e. generates an average \f$ d_i' = (d_{2i}+d_{2i+1})/2 \f$
    \ingroup tools_math_stat

    \param[in] data data to be blocked. NOte that the result will overwrite the input array!!!
    \param[in] N size of the input array
    \return returns the new number of entries in the array after blocking (=N/2)

    This function implements the blocking transform proposed by:
      <center>Flyvbjerg H, Petersen H G  (1989) Error estimates on averages of correlated data. <i>The Journal of Chemical Physics</i> <b>91</b><b>:</b> 461, doi: 10.1063/1.457480.</center>

 */
template <class T>
uint32_t statisticsBlockingTransformStep(T* data, uint32_t N) {
    uint32_t Nnew=N/2;
    register uint32_t j=0;
    for (j=0; j<Nnew; j++) {
        data[j]=(data[2*j]+data[2*j+1])/T(2);
    }
    return Nnew;
}


/*! \brief calculates the multi-tau autocorrelation function of a given timeseries \a data, using symmetric normalization and calculate an error estimate using the blocking method
    \ingroup tools_math_stat

    \param[out] dataout the autocorreltion function \f$ \hat{\gamma}(k) \f$ is written into this array, which has to have length \c Ntau
    \param[in] data1 input timeseries \f$ X_i \f$ of length \a N
    \param[in] data2 input timeseries \f$ Y_i \f$ of length \a N
    \param[in] N size of input data \a data
    \param[in] tau array containing the lag times (in increasing order) at which to evaluate the correlation function
    \param[in] Ntau length of the array \a tau
    \param[in] stride stride to go over \a data1 and \a data2
    \param[in] maxblock maximum blocking level
    \param[out] outBlockLevel optional output for the automatically determined blocking level for the errors
    \param[out] outBlockingSucceeded optional output for whether the block search (see below) succeeded (\c true ) or a standard blocking level is used (\c false ).
    \param[out] outBlockingCurveT optional output for the blocking time (\f$ 2^b\f$ where \f$b\f$ is the blocking level
    \param[out] outBlockingCurve optional output for the blocking curve of standard errors
    \param[out] outBlockingCurveE optional output for the blocking curve errors of the standard error

    This function implements:
      \f[ \hat{g}(\tau)=\frac{(T-\tau)\cdot\sum\limits_{i=\tau}^{N}X_i\cdot Y_{i-\tau}}{\left[\sum\limits_{i=\tau}^{N}X_i\right]\cdot\left[\sum\limits_{i=\tau}^{N}Y_{i-\tau}\right]} \f]

    You can use statisticsAutocorrelateCreateMultiTau() to generate an array of tau values to be used with this function.


    This function calculates the same correlation function (CF) as statisticsCrosscorrelateMultiTauSymmetric(), but in addition tries to estimate the
    standard error of each entry in the CF. To get a reliable estimator of this standard error, the blocking method is used. IT was first
    described in
      <center>Flyvbjerg H, Petersen H G  (1989) Error estimates on averages of correlated data. <i>The Journal of Chemical Physics</i> <b>91</b><b>:</b> 461, doi: 10.1063/1.457480.</center>
    This method basically estimates the error of the average in the correlation function, i.e. the error of
      \f[ \langle I_n\cdot J_{n+\tau}\rangle=\frac{1}{N}\cdot\sum\limits_n I_n\cdot J_{n+\tau} \f]
    as
      \f[ \mbox{SE}(\langle I_n\cdot J_{n+\tau}\rangle) = \sqrt{\frac{c_0'}{N'-1}} \f]
    where \f$ c_0'\f' is the variance of the blocked series of products \f$\{I_n'\cdot J_{n+\tau}'\}\f$ and \f$n'\f$ is the number of entries in
    this series. Blocking means that this function does not use the raw series \f$\{I_n\cdot J_{n+\tau}\}\f$ but step-by-step blocks (i.e.
    averages) this series using the blocking transform:
      \f[ I_n'\cdot J_{n+\tau}' = \frac{1}{2}\cdot\{I_{2n}\cdot J_{2(n+\tau)+1} \f]
      \f[ n' = \lfloor n/2\rfloor \f]
    This helps to remove any correlations from the raw series that might otherwise hamper the estimate of the standard error! Note that the
    actual correlation sum can be calculated from any step in this series!

    This function also uses a method to automatically determine the level of blocking that was proposed in
      <center>Guo S, Bag N, Mishra A, Wohland T, Bathe M  (2014) Bayesian Total Internal Reflection Fluorescence Correlation Spectroscopy Reveals hIAPP-Induced Plasma Membrane Domain Organization in Live Cells. <i>Biophysical Journal</i> <b>106</b><b>:</b> 190-200</center>
    Basically the function calculates the full blocking curve including its error, i.e. [see Flyvbjerg and Petersen 1989]
      \f[ \sqrt{\frac{c_0'}{N'-1}}\cdot\left(1\pm \frac{1}{\sqrt{2(N'-1)}}\right) \f]
    Then it finds the blocking level at which three consecutive points on the curve have overlapping errorbars, i.e. whether
      \f[ \left|\sqrt{\frac{c_0'}{N'-1}}-\sqrt{\frac{c_0''}{N''-1}}\right|< \sqrt{\frac{c_0'}{N'-1}}\cdot\frac{1}{\sqrt{2(N'-1)}}+\sqrt{\frac{c_0''}{N''-1}}\cdot\frac{1}{\sqrt{2(N''-1)}} \f]
    In addition this search algorithms also stops when \f$n'<8\f$, because otherwise there may not be enough datapoints to estimate the variance!
    If the algorithm does not find a blocking level that fulfills the above conditions, a high standard blocking level is choose, hoping that all
    correlations be decayed until then.

 */
template <class T, class Tsumming>
inline void statisticsCrosscorrelateMultiTauSymmetricWithBlocking(double* dataout, double* errorout, const T* data1, const T* data2, uint32_t N, const long* tau, uint32_t Ntau, uint32_t stride=1, uint32_t maxblock=25, uint32_t* outBlockLevel=NULL, bool* outBlockingSucceeded=NULL, std::vector<int>* outBlockingCurveT=NULL, std::vector<double>* outBlockingCurve=NULL, std::vector<double>* outBlockingCurveE=NULL) {

    double* prods=(double*)statisticsMalloc(N*sizeof(double)); // for blocking
    uint32_t blockinglevel=maxblock;
    for (uint32_t i=0; i<Ntau; i++) {
        const long ctau=tau[i];
        register Tsumming sum=0;
        register Tsumming sum_delayed=0;
        register Tsumming corr_sum=0;

        if (ctau<long(N)) {
            register uint64_t j;
            register uint64_t jj=0;
            //register Tsumming d=0, ddel=0, dprod;
            //#pragma omp simd reduction(+:msd) reduction(+:sum)
            for (j=ctau; j<N; j++) {
                const Tsumming d=data1[j*stride];
                const Tsumming ddel=data2[(j-ctau)*stride];
                const Tsumming dprod=d*ddel;
                sum=sum+double(d);
                sum_delayed=sum_delayed+ddel;
                prods[jj]=dprod;
                corr_sum=corr_sum+dprod;
                jj++;
            }
            dataout[i]=(double)(N-ctau)*(double)corr_sum/(double)(sum)/double(sum_delayed);
            errorout[i]=1.0/double(Ntau);

            for (j=0; j<jj; j++) { // normalize products
                prods[j]=(double)(N-ctau)*double(N)*prods[j]/(double)(sum)/double(sum_delayed);
            }

            if (i==0) { // we estimate the blocking level for the first lag
                uint32_t Nb=jj;
                std::vector<double> lastvar, lastvarerr;
                std::vector<uint8_t> bcondition1;
                std::vector<int> bcurveT;
                lastvar.reserve(maxblock);
                lastvarerr.reserve(maxblock);
                bool stoppedTooFewPoints=false;

                //std::cout<<"generainhg blocking curve:\n";
                for (register uint32_t b=0; b<maxblock-1; b++) {
                    const double var=sqrt(statisticsVariance(prods, Nb)/double(Nb-1)); // variance of products /(N-1) = standard error of the mean!
                    const double varerr=var/sqrt(2.0*double(Nb-1)); // and its error



                    lastvar.push_back(var);
                    lastvarerr.push_back(varerr);
                    bcurveT.push_back(pow(2,b));

                    //std::cout<<b<<"; "<<pow(2,b)<<"; "<<(var)<<"; "<<(varerr)<<"; "<<Nb;
                    if (b>0)  {
                        bcondition1.push_back((fabs(lastvar[b-1]-lastvar[b])<=fabs(lastvarerr[b-1]+lastvarerr[b]))?1:0);
                        //std::cout<<"; "<<((fabs(lastvar[b-1]-lastvar[b])<=fabs(lastvarerr[b-1]+lastvarerr[b]))?1:0);
                    } else {
                        bcondition1.push_back(0);
                        //std::cout<<"; 0";
                    }
                    //std::cout<<"\n";
                    // check for good blocking


                    Nb=statisticsBlockingTransformStep(prods, Nb);
                    if (Nb<4) {
                        //blockinglevel=b;
                        stoppedTooFewPoints=true;
                        //std::cout<<"### stopped blocking search because too few datapoints!!!\n";
                        break;
                    }
                }

                blockinglevel=bcondition1.size()-1;
                if (outBlockingSucceeded) (*outBlockingSucceeded)=false;
                if (bcondition1.size()>3){
                    for (register size_t b=0; b<bcondition1.size()-3; b++)  {
                        if (bcondition1[b]*bcondition1[b+1]*bcondition1[b+2]!=0) {
                            blockinglevel=b;
                            //std::cout<<"### condition1 OK at b="<<b<<"\n";
                            if (outBlockingSucceeded) (*outBlockingSucceeded)=true;
                            break;
                        }
                    }
                } else {
                    if (bcondition1.size()>0) blockinglevel=bcondition1.size()-1;
                    else blockinglevel=0;
                    //std::cout<<"### using default blocking!!!\n";
                }
                errorout[i]=(lastvar[blockinglevel]);
                //std::cout<<"### using block-level "<<blockinglevel<<"  (max: "<<maxblock<<" cond.size="<<bcondition1.size()<<")  SE["<<i<<"]="<<errorout[i]<<"\n";

                if (outBlockingCurveT) (*outBlockingCurveT)=bcurveT;
                if (outBlockingCurve) (*outBlockingCurve)=lastvar;
                if (outBlockingCurveE) (*outBlockingCurveE)=lastvarerr;
            } else { // we already know the blocking level, so we can use it!
                uint32_t Nb=jj;

                const uint32_t bmax=statisticsMin(maxblock, blockinglevel);
                //std::cout<<"### applying to "<<i<<"/"<<Ntau<<" blocking-level "<<bmax;
                if (bmax>0 && Nb>2) {
                    for (register uint32_t b=0; b<=bmax; b++) {
                        Nb=statisticsBlockingTransformStep(prods, Nb);
                        if (Nb<8) break;
                    }
                }
                errorout[i]=sqrt(statisticsVariance(prods, Nb)/double(Nb-1));
                //std::cout<<":   "<<errorout[i]<<"  Nb="<<Nb<<"/"<<jj<<"\n";
            }
            //std::cout.flush();
        } else {
            dataout[i]=1.0;
            errorout[i]=1.0/double(Ntau);
        }
    }
    if (outBlockLevel) (*outBlockLevel)=statisticsMin(maxblock, blockinglevel);
    statisticsFree(prods);
    //std::cout<<"###############\n\n\n";
    //std::cout.flush();
}

/*! \brief calculates the multi-tau cross-correlation function of a given timeseries \a data, using symmetric normalization and calculate an error estimate using the blocking method
    \ingroup tools_math_stat

    \param[out] dataout the autocorreltion function \f$ \hat{\gamma}(k) \f$ is written into this array, which has to have length \c Ntau
    \param[in] data1 input timeseries \f$ X_i \f$ of length \a N
    \param[in] data2 input timeseries \f$ Y_i \f$ of length \a N
    \param[in] N size of input data \a data
    \param[in] tau array containing the lag times (in increasing order) at which to evaluate the correlation function
    \param[in] Ntau length of the array \a tau
    \param[in] stride stride to go over \a data1 and \a data2
    \param[in] maxblock maximum blocking level
    \param[out] outBlockLevel optional output for the automatically determined blocking level for the errors
    \param[out] outBlockingSucceeded optional output for whether the block search (see below) succeeded (\c true ) or a standard blocking level is used (\c false ).
    \param[out] outBlockingCurveT optional output for the blocking time (\f$ 2^b\f$ where \f$b\f$ is the blocking level
    \param[out] outBlockingCurve optional output for the blocking curve of standard errors
    \param[out] outBlockingCurveE optional output for the blocking curve errors of the standard error


    In contrast to statisticsCrosscorrelateMultiTauSymmetricWithBlocking() this function first makes a copy of the
    input array (if required, i.e. stride>1!). This may lead to better performance for strides larger than 1, due to better memory
    locality.

    \note Expect \c 2*N*sizeof(T) bytes of extra memory consumption.

    \see documentation of statisticsCrosscorrelateMultiTauSymmetricWithBlocking() for details

 */
template <class T, class Tsumming>
inline void statisticsCrosscorrelateMultiTauSymmetricWithBlockingMemOptimized(double* dataout, double* errorout, const T* data, const T* data2, uint32_t N, const long* tau, uint32_t Ntau, uint32_t stride=1, uint32_t maxblock=25, uint32_t* outBlockLevel=NULL, bool* outBlockingSucceeded=NULL, std::vector<int>* outBlockingCurveT=NULL, std::vector<double>* outBlockingCurve=NULL, std::vector<double>* outBlockingCurveE=NULL){
    if (stride>2) {
        T* d=(T*)statisticsMalloc(N*sizeof(T));
        T* d2=(T*)statisticsMalloc(N*sizeof(T));
        for (register uint32_t i=0; i<N; i++) {
            d[i]=data[i*stride];
            d2[i]=data2[i*stride];
        }
        statisticsCrosscorrelateMultiTauSymmetricWithBlocking<T, Tsumming>(dataout, errorout, d, d2, N, tau, Ntau, 1, maxblock, outBlockLevel, outBlockingSucceeded, outBlockingCurveT, outBlockingCurve, outBlockingCurveE);
        statisticsFree(d);
        statisticsFree(d2);
    } else {
        statisticsCrosscorrelateMultiTauSymmetricWithBlocking<T, Tsumming>(dataout, errorout, data, data2, N, tau, Ntau, stride, maxblock, outBlockLevel, outBlockingSucceeded, outBlockingCurveT, outBlockingCurve, outBlockingCurveE);
    }
}


/*! \brief create semi-logarithmically spaced bins for statisticsAutocorrelateMultiTauSymmetric()
    \ingroup tools_math_stat

    This function generates a set of tau values for use with statisticsAutocorrelateMultiTauSymmetric(). They are semi-logarithmically
    spaced with \a S blocks each containing \a P tau-values. Between two blocks the \f$ \Delta\tau \f$ is multiplied by \a m.

    So the output array \a tau should contain >=S*P entries.
*/
LIB_EXPORT void statisticsAutocorrelateCreateMultiTau(long* tau, int S, int m, int P);
LIB_EXPORT void statisticsAutocorrelateCreateMultiTau64(uint64_t* tau, int S, int m, int P);
LIB_EXPORT void statisticsAutocorrelateCreateMultiTau(double* tau, int S, int m, int P, double deltaT);









/*! \brief calculates the multi-tau crosscorrelation function of a given timeseries \a data, using symmetric normalization and implicit averaging of the data
    \ingroup tools_math_stat

    \tparam T specifies the type of the input data
    \tparam Tint specifies the data type used to calculate the averaged input data (e.g. if \c uint8_t data is provided, it makes sense to use \c uint32_t or even \c uint64_t for the averaging, as the numbers might get quite large (averaging over \f$ m^S \f$ in the last block!!!)
    \tparam Tsumming specifies the data type used to calculate the sums and products appearing in the calculation of the correlation function
    \param[out] dataout the autocorreltion function \f$ \hat{\gamma}(k) \f$ is written into this array, which has to have length \c Ntau
    \param[in] data1 input timeseries \f$ X_i \f$ of length \a N
    \param[in] data2 input timeseries \f$ Y_i \f$ of length \a N
    \param[in] N size of input data \a data
    \param[in] S number of linear correlator blocks
    \param[in] m increase if lag time from block to block
    \param[in] P number of linearly spaced lags per block
    \param[in] stride stride to go over \a data1 and \a data2

    You can use statisticsAutocorrelateCreateMultiTau() to generate an array of tau values to be used with the output of this function.

    In contrast to statisticsCrosscorrelateMultiTauSymmetric() this function uses an implicit averaging scheme when
    calculating the CCF:
      -# First the CCFs for \f$\Delta\tau=1...P\f$ are calculated, as in statisticsAutocorrelateCreateMultiTau()
      -# Now the data is averaged by summin groups of \f$ m \f$ samples, so its size is reduzed to
         \f$ \lfloor N/m\rfloor \f$
      -# Now again \f$P\f$ linearly spaced lags are calculated on this averaged dataset
      -# we return to step 2
    .
    This described loop (steps 2-4) is executed \f$S-1\f$ times, which results in \f$S\f$ linear correlation blocks
    that have a quasi-logarithmic spacing of lag times. This table shows for the first 3 blocks, which
    lags are actually calculated (1. column). The second column shows how this real lag corresponds to a local
    lag on the averaged data and the third column shows a value used for internal caluclations.
\verbatim
    p1=1..P                        p1/1   = 1...P                        lastPMax=0
    p2=P+m ... P+m*P               p2/m   = P/m+1 ... P/m+P              lastPMax=P
    p3=P+m*P+m^2 ... P+m*P+m^2*P   p3/m^2 = P/m^2+P/m+1 ... P/m^2+P/m+P
                                          = (P+P/m)/m+1 ... (P+P/m)/m+P  lastPMax=P/m+P
    p4=...
\endverbatim

    This implicit averaging scheme leads to an estimate of the ACF, which is convolved with a triangular kernel, where the width
    of the kernel increases from block to block by a factor of \f$ m \f$ .

    \note This function creates a copy of the input data, so expect \c 2*N*sizeof(Tint) additional memory consumtion.
          This is done, as the averaged input data has to be stored somewhere and for strides>1 this may even improve performance,
          due to better memory locallity! If this is used for ACF calculation (\c data1==data2 ) only \c N*sizeof(Tint) additional
          memory bytes are used!

    */
template <class T, class Tint, class Tsumming>
inline void statisticsCrosscorrelateMultiTauAvgSymmetric(double* dataout, const T* data1, const T* data2, uint32_t N, int S, int m, int P, long long stride=1) {
    // copy first data array
    Tint* d1=(Tint*)statisticsMalloc(N*sizeof(Tint));
    for (register uint32_t i=0; i<N; i++) {
        d1[i]=data1[i*stride];
        //if (i%1000==0) std::cout<<i<<" ";
    }
    //std::cout<<"\n d1="<<d1<<"\n";
    Tint* d2=d1;
    //std::cout<<"\n d2="<<d2<<"\n";
    // if we calculate a cross-correlation (data1!=data2), we need a second copy:
    if (data1!=data2) {
        d2=(Tint*)statisticsMalloc(N*sizeof(Tint));
        for (register uint32_t i=0; i<N; i++) {
            d2[i]=data2[i*stride];
        }
    }
    //std::cout<<"\n init dataout\n";
    for (int i=0; i<S*P; i++) dataout[i]=1;
    //std::cout<<"\n init dataout done\n";

    //const int PP=2*P;
    double* dout=(double*)statisticsCalloc(P, sizeof(double));

    long Ncurrent=N;
    double pow2=1.0;
    long lastPMax=0;
    // iterate over linear correlators
    for (int s=0; s<S; s++) {

        // calculate linear correlator
        int PMin=lastPMax/m+1;
        int PMax=lastPMax/m+P;
        for (register int p=PMin; p<=PMax; p++) {
            register Tsumming sum=0;
            register Tsumming sum_delayed=0;
            register Tsumming msd=0;
            if (p<Ncurrent) {
                register long j;
                //#pragma omp simd reduction(+:sum) reduction(+:sum_delayed) reduction(+:msd)
                for (j=p; j<Ncurrent; j++) {
                    sum=sum+d1[j];
                    sum_delayed=sum_delayed+Tsumming(d2[j-p]);
                    msd=msd+Tsumming(d1[j])*Tsumming(d2[j-p]);
                }
                dout[p-PMin]=(double)(Ncurrent-p)*double(msd)/(double(sum)*double(sum_delayed));
            } else {
                dout[p-PMin]=1.0;
            }
        }
        lastPMax=lastPMax/m+P;


        // copy results to correct positions
        for (register int p=0; p<P; p++) {
            dataout[s*P+p]=dout[p];
        }

        // sum data over $m$ cells
        if (Ncurrent>m) {
            if (data1!=data2) {
                for (register long i=0; i<Ncurrent; i++) {
                    if (i%m>0) {
                        d1[i/m]=d1[i/m]+d1[i];
                        d2[i/m]=d2[i/m]+d2[i];
                    } else {
                        d1[i/m]=d1[i];
                        d2[i/m]=d2[i];
                    }
                }
            } else {
                // Optimization for Autocorrelations: we don't need to use two real data arrays, but only one!
                for (register long i=0; i<Ncurrent; i++) {
                    if (i%m>0) {
                        d1[i/m]=d1[i/m]+d1[i];
                    } else {
                        d1[i/m]=d1[i];
                    }
                }
            }
        } else {
            break;
        }
        Ncurrent=Ncurrent/m;
        pow2=pow2*m;
    }

    statisticsFree(d1);
    if (d1!=d2) statisticsFree(d2);
    statisticsFree(dout);
}

/*! \brief calculates the multi-tau autocorrelation function of a given timeseries \a data, using symmetric normalization and implicit averaging of the data
    \ingroup tools_math_stat

    \note For a detailed description, see the documentation of statisticsCrosscorrelateMultiTauAvgSymmetric() !

    \tparam T specifies the type of the input data
    \tparam Tint specifies the data type used to calculate the averaged input data (e.g. if \c uint8_t data is provided, it makes sense to use \c uint32_t or even \c uint64_t for the averaging, as the numbers might get quite large (averaging over \f$ m^S \f$ in the last block!!!)
    \tparam Tsumming specifies the data type used to calculate the sums and products appearing in the calculation of the correlation function
    \param[out] dataout the autocorreltion function \f$ \hat{\gamma}(k) \f$ is written into this array, which has to have length \c Ntau
    \param[in] data input timeseries \f$ X_i \f$ of length \a N
    \param[in] N size of input data \a data
    \param[in] S number of linear correlator blocks
    \param[in] m increase if lag time from block to block
    \param[in] P number of linearly spaced lags per block
    \param[in] stride stride to go over \a data1 and \a data2

    You can use statisticsAutocorrelateCreateMultiTau() to generate an array of tau values to be used with the output of this function.
 */
template <class T, class Tint, class Tsumming>
inline void statisticsAutocorrelateMultiTauAvgSymmetric(double* dataout, const T* data, long N, int S, int m, int P, long long stride=1){
    statisticsCrosscorrelateMultiTauAvgSymmetric<T, Tint, Tsumming>(dataout, data, data, N, S, m, P, stride);
}








/*! \brief calculates the multi-tau crosscorrelation function of a given timeseries \a data, using symmetric normalization and implicit averaging of the data and uses the blocking method to estimate the standard error of the correlation function
    \ingroup tools_math_stat

    \tparam T specifies the type of the input data
    \tparam Tint specifies the data type used to calculate the averaged input data (e.g. if \c uint8_t data is provided, it makes sense to use \c uint32_t or even \c uint64_t for the averaging, as the numbers might get quite large (averaging over \f$ m^S \f$ in the last block!!!)
    \tparam Tsumming specifies the data type used to calculate the sums and products appearing in the calculation of the correlation function
    \param[out] dataout the autocorreltion function \f$ \hat{\gamma}(k) \f$ is written into this array, which has to have length \c Ntau
    \param[in] data1 input timeseries \f$ X_i \f$ of length \a N
    \param[in] data2 input timeseries \f$ Y_i \f$ of length \a N
    \param[in] N size of input data \a data
    \param[in] S number of linear correlator blocks
    \param[in] m increase if lag time from block to block
    \param[in] P number of linearly spaced lags per block
    \param[in] stride stride to go over \a data1 and \a data2
    \param[in] maxblock maximum blocking level
    \param[out] outBlockLevel optional output for the automatically determined blocking level for the errors
    \param[out] outBlockingSucceeded optional output for whether the block search (see below) succeeded (\c true ) or a standard blocking level is used (\c false ).
    \param[out] outBlockingCurveT optional output for the blocking time (\f$ 2^b\f$ where \f$b\f$ is the blocking level
    \param[out] outBlockingCurve optional output for the blocking curve of standard errors
    \param[out] outBlockingCurveE optional output for the blocking curve errors of the standard error

    You can use statisticsAutocorrelateCreateMultiTau() to generate an array of tau values to be used with the output of this function.

    In contrast to statisticsCrosscorrelateMultiTauSymmetric() this function uses an implicit averaging scheme when
    calculating the CCF:
      -# First the CCFs for \f$\Delta\tau=1...P\f$ are calculated, as in statisticsAutocorrelateCreateMultiTau()
      -# Now the data is averaged by summin groups of \f$ m \f$ samples, so its size is reduzed to
         \f$ \lfloor N/m\rfloor \f$
      -# Now again \f$P\f$ linearly spaced lags are calculated on this averaged dataset
      -# we return to step 2
    .
    This described loop (steps 2-4) is executed \f$S-1\f$ times, which results in \f$S\f$ linear correlation blocks
    that have a quasi-logarithmic spacing of lag times. This table shows for the first 3 blocks, which
    lags are actually calculated (1. column). The second column shows how this real lag corresponds to a local
    lag on the averaged data and the third column shows a value used for internal caluclations.
\verbatim
    p1=1..P                        p1/1   = 1...P                        lastPMax=0
    p2=P+m ... P+m*P               p2/m   = P/m+1 ... P/m+P              lastPMax=P
    p3=P+m*P+m^2 ... P+m*P+m^2*P   p3/m^2 = P/m^2+P/m+1 ... P/m^2+P/m+P
                                          = (P+P/m)/m+1 ... (P+P/m)/m+P  lastPMax=P/m+P
    p4=...
\endverbatim

    This implicit averaging scheme leads to an estimate of the ACF, which is convolved with a triangular kernel, where the width
    of the kernel increases from block to block by a factor of \f$ m \f$ .

    \note This function creates a copy of the input data, so expect \c 2*N*sizeof(Tint) additional memory consumtion.
          This is done, as the averaged input data has to be stored somewhere and for strides>1 this may even improve performance,
          due to better memory locallity! If this is used for ACF calculation (\c data1==data2 ) only \c N*sizeof(Tint) additional
          memory bytes are used!




    This function calculates the same correlation function (CF) as statisticsCrosscorrelateMultiTauSymmetric(), but in addition tries to estimate the
    standard error of each entry in the CF. To get a reliable estimator of this standard error, the blocking method is used. IT was first
    described in
      <center>Flyvbjerg H, Petersen H G  (1989) Error estimates on averages of correlated data. <i>The Journal of Chemical Physics</i> <b>91</b><b>:</b> 461, doi: 10.1063/1.457480.</center>
    This method basically estimates the error of the average in the correlation function, i.e. the error of
      \f[ \langle I_n\cdot J_{n+\tau}\rangle=\frac{1}{N}\cdot\sum\limits_n I_n\cdot J_{n+\tau} \f]
    as
      \f[ \mbox{SE}(\langle I_n\cdot J_{n+\tau}\rangle) = \sqrt{\frac{c_0'}{N'-1}} \f]
    where \f$ c_0'\f' is the variance of the blocked series of products \f$\{I_n'\cdot J_{n+\tau}'\}\f$ and \f$n'\f$ is the number of entries in
    this series. Blocking means that this function does not use the raw series \f$\{I_n\cdot J_{n+\tau}\}\f$ but step-by-step blocks (i.e.
    averages) this series using the blocking transform:
      \f[ I_n'\cdot J_{n+\tau}' = \frac{1}{2}\cdot\{I_{2n}\cdot J_{2(n+\tau)+1} \f]
      \f[ n' = \lfloor n/2\rfloor \f]
    This helps to remove any correlations from the raw series that might otherwise hamper the estimate of the standard error! Note that the
    actual correlation sum can be calculated from any step in this series!

    This function also uses a method to automatically determine the level of blocking that was proposed in
      <center>Guo S, Bag N, Mishra A, Wohland T, Bathe M  (2014) Bayesian Total Internal Reflection Fluorescence Correlation Spectroscopy Reveals hIAPP-Induced Plasma Membrane Domain Organization in Live Cells. <i>Biophysical Journal</i> <b>106</b><b>:</b> 190-200</center>
    Basically the function calculates the full blocking curve including its error, i.e. [see Flyvbjerg and Petersen 1989]
      \f[ \sqrt{\frac{c_0'}{N'-1}}\cdot\left(1\pm \frac{1}{\sqrt{2(N'-1)}}\right) \f]
    Then it finds the blocking level at which three consecutive points on the curve have overlapping errorbars, i.e. whether
      \f[ \left|\sqrt{\frac{c_0'}{N'-1}}-\sqrt{\frac{c_0''}{N''-1}}\right|< \sqrt{\frac{c_0'}{N'-1}}\cdot\frac{1}{\sqrt{2(N'-1)}}+\sqrt{\frac{c_0''}{N''-1}}\cdot\frac{1}{\sqrt{2(N''-1)}} \f]
    In addition this search algorithms also stops when \f$n'<8\f$, because otherwise there may not be enough datapoints to estimate the variance!
    If the algorithm does not find a blocking level that fulfills the above conditions, a high standard blocking level is choose, hoping that all
    correlations be decayed until then.

    The finally estimated number of blocking levels is reduced by the number of blockings already done during the correlation
    function calculation. This is an estimate, as in the CF calculation one averages the input data and in the blocking the
    products of input data, but should work properly in most cases, as both imply a smoothing of the data.
    */
template <class T, class Tint, class Tsumming>
inline void statisticsCrosscorrelateMultiTauAvgSymmetricWithBlocking(double* dataout, double* errorout, const T* data1, const T* data2, uint32_t N, int S, int m, int P, long long stride=1, uint32_t maxblock=25, uint32_t* outBlockLevel=NULL, bool* outBlockingSucceeded=NULL, std::vector<int>* outBlockingCurveT=NULL, std::vector<double>* outBlockingCurve=NULL, std::vector<double>* outBlockingCurveE=NULL) {
    uint32_t blockinglevel=maxblock;
    double* prods=(double*)statisticsMalloc(N*sizeof(double)); // for blocking
    // copy first data array
    Tint* d1=(Tint*)statisticsMalloc(N*sizeof(Tint));
    for (register uint32_t i=0; i<N; i++) {
        d1[i]=data1[i*stride];
        //if (i%1000==0) std::cout<<i<<" ";
    }
    //std::cout<<"\n d1="<<d1<<"\n";
    Tint* d2=d1;
    //std::cout<<"\n d2="<<d2<<"\n";
    // if we calculate a cross-correlation (data1!=data2), we need a second copy:
    if (data1!=data2) {
        d2=(Tint*)statisticsMalloc(N*sizeof(Tint));
        for (register uint32_t i=0; i<N; i++) {
            d2[i]=data2[i*stride];
        }
    }
    //std::cout<<"\n init dataout\n";
    for (int i=0; i<S*P; i++) dataout[i]=1;
    //std::cout<<"\n init dataout done\n";

    //const int PP=2*P;
    double* dout=(double*)statisticsCalloc(P, sizeof(double));
    double* derr=(double*)statisticsCalloc(P, sizeof(double));

    long Ncurrent=N;
    double pow2=1.0;
    long lastPMax=0;
    int avgCount=0;
    // iterate over linear correlators
    for (int s=0; s<S; s++) {

        // calculate linear correlator
        int PMin=lastPMax/m+1;
        int PMax=lastPMax/m+P;
        for (register int p=PMin; p<=PMax; p++) {
            register Tsumming sum=0;
            register Tsumming sum_delayed=0;
            register Tsumming corrsum=0;
            register uint64_t jj=0;
            if (p<Ncurrent) {
                register int64_t j;
                //#pragma omp simd reduction(+:sum) reduction(+:sum_delayed) reduction(+:msd)
                for (j=p; j<Ncurrent; j++) {
                    const Tsumming d=d1[j];
                    const Tsumming ddel=d2[j-p];
                    const Tsumming dprod=d*ddel;
                    sum=sum+d;
                    sum_delayed=sum_delayed+ddel;
                    corrsum=corrsum+dprod;
                    prods[jj]=dprod;
                    jj++;
                }
                dout[p-PMin]=(double)(Ncurrent-p)*double(corrsum)/(double(sum)*double(sum_delayed));
                derr[p-PMin]=1.0/double(S*P);
                for (j=0; j<(int64_t)jj; j++) { // normalize products
                    prods[j]=(double)(Ncurrent-p)*double(Ncurrent)*prods[j]/(double)(sum)/double(sum_delayed);
                }

                if (s==0 && p==PMin) { // we estimate the blocking level for the first lag
                    uint32_t Nb=jj;
                    std::vector<double> lastvar, lastvarerr;
                    std::vector<uint8_t> bcondition1;
                    std::vector<int> bcurveT;
                    lastvar.reserve(maxblock);
                    lastvarerr.reserve(maxblock);
                    bool stoppedTooFewPoints=false;

                    //std::cout<<"generainhg blocking curve:\n";
                    for (register uint32_t b=0; b<maxblock-1; b++) {
                        const double var=sqrt(statisticsVariance(prods, Nb)/double(Nb-1)); // variance of products /(N-1) = standard error of the mean!
                        const double varerr=var/sqrt(2.0*double(Nb-1)); // and its error



                        lastvar.push_back(var);
                        lastvarerr.push_back(varerr);
                        bcurveT.push_back(pow(2,b));

                        //std::cout<<b<<"; "<<pow(2,b)<<"; "<<(var)<<"; "<<(varerr)<<"; "<<Nb;
                        if (b>0)  {
                            bcondition1.push_back((fabs(lastvar[b-1]-lastvar[b])<=fabs(lastvarerr[b-1]+lastvarerr[b]))?1:0);
                            //std::cout<<"; "<<((fabs(lastvar[b-1]-lastvar[b])<=fabs(lastvarerr[b-1]+lastvarerr[b]))?1:0);
                        } else {
                            bcondition1.push_back(0);
                            //std::cout<<"; 0";
                        }
                        //std::cout<<"\n";
                        // check for good blocking


                        Nb=statisticsBlockingTransformStep(prods, Nb);
                        if (Nb<4) {
                            //blockinglevel=b;
                            stoppedTooFewPoints=true;
                            //std::cout<<"### stopped blocking search because too few datapoints!!!\n";
                            break;
                        }
                    }

                    blockinglevel=bcondition1.size()-1;
                    if (outBlockingSucceeded) (*outBlockingSucceeded)=false;
                    if (bcondition1.size()>3){
                        for (register size_t b=0; b<bcondition1.size()-3; b++)  {
                            if (bcondition1[b]*bcondition1[b+1]*bcondition1[b+2]!=0) {
                                blockinglevel=b;
                                //std::cout<<"### condition1 OK at b="<<b<<"\n";
                                if (outBlockingSucceeded) (*outBlockingSucceeded)=true;
                                break;
                            }
                        }
                    } else {
                        if (bcondition1.size()>0) blockinglevel=bcondition1.size()-1;
                        else blockinglevel=0;
                        //std::cout<<"### using default blocking!!!\n";
                    }
                    derr[p-PMin]=(lastvar[blockinglevel]);
                    //std::cout<<"### using block-level "<<blockinglevel<<"  (max: "<<maxblock<<" cond.size="<<bcondition1.size()<<")  SE["<<(int64_t)(p-PMin)<<"]="<<derr[(int64_t)(p-PMin)]<<"\n";

                    if (outBlockingCurveT) (*outBlockingCurveT)=bcurveT;
                    if (outBlockingCurve) (*outBlockingCurve)=lastvar;
                    if (outBlockingCurveE) (*outBlockingCurveE)=lastvarerr;
                } else { // we already know the blocking level, so we can use it!
                    uint32_t Nb=jj;

                    const uint32_t bmax=statisticsMax((int64_t)0, statisticsMin((int64_t)maxblock, (int64_t)blockinglevel)-(int64_t)avgCount);
                    //std::cout<<"### applying to "<<(s*P+p)<<"/"<<(S*P)<<" blocking-level "<<bmax;
                    if (bmax>0 && Nb>2) {
                        for (register uint32_t b=0; b<=bmax; b++) {
                            Nb=statisticsBlockingTransformStep(prods, Nb);
                            if (Nb<8) break;
                        }
                    }
                    derr[p-PMin]=sqrt(statisticsVariance(prods, Nb)/double(Nb-1));
                    //std::cout<<":   "<<derr[p-PMin]<<"  Nb="<<Nb<<"/"<<jj<<"\n";
                }

            } else {
                dout[p-PMin]=1.0;
                derr[p-PMin]=1.0/double(S*P);
            }
        }
        lastPMax=lastPMax/m+P;


        // copy results to correct positions
        for (register int p=0; p<P; p++) {
            dataout[s*P+p]=dout[p];
            errorout[s*P+p]=derr[p];
        }

        // sum data over $m$ cells
        if (Ncurrent>m) {
            if (data1!=data2) {
                for (register long i=0; i<Ncurrent; i++) {
                    if (i%m>0) {
                        d1[i/m]=d1[i/m]+d1[i];
                        d2[i/m]=d2[i/m]+d2[i];
                    } else {
                        d1[i/m]=d1[i];
                        d2[i/m]=d2[i];
                    }
                }
            } else {
                // Optimization for Autocorrelations: we don't need to use two real data arrays, but only one!
                for (register long i=0; i<Ncurrent; i++) {
                    if (i%m>0) {
                        d1[i/m]=d1[i/m]+d1[i];
                    } else {
                        d1[i/m]=d1[i];
                    }
                }
            }

        } else {
            break;
        }
        Ncurrent=Ncurrent/m;
        pow2=pow2*m;
        avgCount++;
    }

    statisticsFree(d1);
    if (d1!=d2) statisticsFree(d2);
    statisticsFree(dout);
    statisticsFree(derr);
    statisticsFree(prods);

    if (outBlockLevel) (*outBlockLevel)=blockinglevel;
}








/*! \brief QuickSort (recursive implementation)
    \ingroup tools_math_stat

    implementation from http://www.linux-related.de/index.html?/coding/sort/sort_quick.htm
*/
template <class T>
inline void statisticsQuicksort(T* a, long long l, long long r){
    if(r>l){
        long long i=l-1;
        long long j=r;

        for(;;){
            while(a[++i]<a[r]);
            while(a[--j]>a[r] && j>i);
            if(i>=j) break;
            statisticsSwap(a, i, j);
        }
        statisticsSwap(a, i, r);

        statisticsQuicksort(a, l, i-1);
        statisticsQuicksort(a, i+1, r);
    }
}


/*! \brief QuickSort (recursive implementation), sorts \a a2 alongside \a a, using \a a as sort criterion
    \ingroup tools_math_stat

    implementation from http://www.linux-related.de/index.html?/coding/sort/sort_quick.htm
*/
template <class T>
inline void statisticsQuicksort(T* a, T* a2, long long l, long long r){
    if(r>l){
        long long i=l-1;
        long long j=r;

        for(;;){
            while(a[++i]<a[r]);
            while(a[--j]>a[r] && j>i);
            if(i>=j) break;
            statisticsSwap(a, i, j);
            statisticsSwap(a2, i, j);
        }
        statisticsSwap(a, i, r);
        statisticsSwap(a2, i, r);

        statisticsQuicksort(a, a2, l, i-1);
        statisticsQuicksort(a, a2, i+1, r);
    }
}







/*! \brief sort the given arrays, using \a input as sort criterion
    \ingroup tools_math_stat

    \param input array to be sorted
    \param input2 array to be sorted
    \param N size of the array input
    \param output if \c !=NULL data is written here (the memory location pointed at by \a output has to have at least the length \a N !!!),
                  otherwise the array input is sorted inplace.
    \param output2 if \c !=NULL data is written here (the memory location pointed at by \a output has to have at least the length \a N !!!),
                  otherwise the array input is sorted inplace.

 */
template <class T>
inline void statisticsSort(T* input, T* input2, long long N, T* output=NULL, T* output2=NULL) {
    if ((!input)) return ;
    if (N<=0) return;
    T* data=input;
    if (output!=NULL) {
        data=output;
        memcpy(output, input, N*sizeof(T));
    }
    T* data2=input2;
    if (output2!=NULL && input2!=NULL) {
        data2=output2;
        memcpy(output2, input2, N*sizeof(T));
    }
    statisticsQuicksort(data, data2, 0, N-1);
}



/*! \brief sort the given array
    \ingroup tools_math_stat

    \param input array to be sorted
    \param N size of the array input
    \param output if \c !=NULL data is written here (the memory location pointed at by \a output has to have at least the length \a N !!!),
                  otherwise the array input is sorted inplace.

 */
template <class T>
inline void statisticsSort(T* input, long long N) {
    if ((!input)) return ;
    if (N<=0) return;
    T* data=input;
    statisticsQuicksort(data, 0, N-1);
}


/*! \brief sort the given array
    \ingroup tools_math_stat

    \param input array to be sorted
    \param N size of the array input
    \param output if \c !=NULL data is written here (the memory location pointed at by \a output has to have at least the length \a N !!!),
                  otherwise the array input is sorted inplace.

 */
template <class T>
inline void statisticsSort(const T* input, long long N, T* output) {
    if ((!input)) return ;
    if (N<=0) return;
    T* data=output;
    memcpy(output, input, N*sizeof(T));
    statisticsQuicksort(data, 0, N-1);
}















/*! \brief swap two elements \a l and \a r in an array \a a
    \ingroup tools_math_stat

*/
template <class T, class T2>
inline void statisticsSwap2(T* a, T2*a2, long long l, long long r){
    statisticsSwap(a, l, r);
    statisticsSwap(a2, l, r);
}


/*! \brief QuickSort (recursive implementation)
    \ingroup tools_math_stat

    implementation from http://www.linux-related.de/index.html?/coding/sort/sort_quick.htm
*/
template <class T, class T2>
inline void statisticsQuicksort2(T* a, T2*a2, long long l, long long r){
    if(r>l){
        long long i=l-1;
        long long j=r;

        for(;;){
            while(a[++i]<a[r]);
            while(a[--j]>a[r] && j>i);
            if(i>=j) break;
            statisticsSwap2(a,a2, i, j);
        }
        statisticsSwap2(a,a2, i, r);

        statisticsQuicksort2(a,a2, l, i-1);
        statisticsQuicksort2(a,a2, i+1, r);
    }
}




/*! \brief sort the given array
    \ingroup tools_math_stat

    \param input array to be sorted
    \param N size of the array input
    \param output if \c !=NULL data is written here (the memory location pointed at by \a output has to have at least the length \a N !!!),
                  otherwise the array input is sorted inplace.

 */
template <class T, class T2>
inline void statisticsSort2(T* input, T2* input2, long long N, T* output=NULL, T2* output2=NULL) {
    if ((!input)) return ;
    if (N<=0) return;
    T* data=input;
    if (output!=NULL) {
        data=output;
        memcpy(output, input, N*sizeof(T));
    }
    T2* data2=input2;
    if (output2!=NULL) {
        data2=output2;
        memcpy(output2, input2, N*sizeof(T2));
    }
    statisticsQuicksort2(data,data2, 0, N-1);
}





/*! \brief filters the given array \a input, by removing all non-OK (INF, NAN) floats. returns the new length of the array
    \ingroup tools_math_stat

    After a call to this function the array will contain all OK floats starting from item 0. the length of the array is reduced accordingly and returned by the function.

    here is an example:
    \code
     double X[7]=(INF,1,2,3,NAN,5,NAN);
     int nl=statisticsFilterGoodFloat(X, 7);
    \endcode
    the new length (nl) will be 7-3=4 and the array contents will be:
    \code
     X[7]=(1,2,3,5,NAN,5,NAN);
    \endcode
    So the order of the good elements is preserved and the end of the array contains rubbish!

*/
template <class T>
inline long long statisticsFilterGoodFloat(T* input, long long N) {
    int j=0;
    for (long long i=0; i<N; i++) {
        if (statisticsFloatIsOK(input[i])) {
            input[j]=input[i];
            j++;
        }
    }
    return j;
}


/*! \brief return the smallest element from a sorted array
    \ingroup tools_math_stat
*/
template <class T>
inline T statisticsSortedMin(T* input, long long N) {
    if ((!input)) return 0;
    if (N<=0) return 0;
    //int i=0;
    //while (i<N && !statisticsFloatIsOK(input[i])) i++;
    return input[0];
}


/*! \brief return the largest element from a sorted array
    \ingroup tools_math_stat
*/
template <class T>
inline T statisticsSortedMax(T* input, long long N) {
    if ((!input)) return 0;
    if (N<=0) return 0;
    //int i=N-1;
    //while (i>=0 && !statisticsFloatIsOK(input[i])) i--;

    return input[N-1];
}


/*! \brief return the median from a sorted array
    \ingroup tools_math_stat

    if \a N is odd, then the center element is returned, otherwise the function returns the average of the two centering elements
*/
template <class T>
inline T statisticsSortedMedian(T* input, long long N) {
    if ((!input)) return 0;
    if (N<=0) return 0;
    if (N==1) return input[0];
    if (N%2==0) {
        return (input[N/2-1]+input[N/2])/2.0;
    } else {
        return input[N/2];
    }
}

/*! \brief return the median from an array
    \ingroup tools_math_stat

    if \a N is odd, then the center element is returned, otherwise the function returns the average of the two centering elements
*/
template <class T>
inline T statisticsMedian(const T* input, long long N) {
    if ((!input)) return 0;
    if (N<=0) return 0;
    if (N==1) return input[0];
    T* sorted=(T*)statisticsMalloc(N*sizeof(T));
    statisticsSort(input, N, sorted);
    N=statisticsFilterGoodFloat(sorted, N);
    register T res=statisticsSortedMedian(sorted, N);
    if (sorted) statisticsFree(sorted);
    return res;
}


/*! \brief return the median from an array
    \ingroup tools_math_stat

    if \a N is odd, then the center element is returned, otherwise the function returns the average of the two centering elements
*/
template <class T>
inline T statisticsMedianMasked(const bool* mask, const T* input, long long N, bool maskUseValue=true) {
    if (!mask) return statisticsMedian(input, N);
    if ((!input)) return 0;
    if (N<=0) return 0;
    if (N==1) return input[0];
    T* sorted=(T*)statisticsMalloc(N*sizeof(T));
    long long NN=0;
    for (long long i=0; i<NN; i++) {
        if (mask[i]==maskUseValue) {
            sorted[NN]=input[i];
            NN++;
        }
    }
    statisticsSort(sorted, NN);
    NN=statisticsFilterGoodFloat(sorted, NN);
    register T res=statisticsSortedMedian(sorted, NN);
    if (sorted) statisticsFree(sorted);
    return res;
}

/*! \brief calculates the median absolute deviation about the median (MAD), a robust measure of sample deviation, the data has to be a sorted array!

    \f[ \mbox{MAD}(\vec{x})=\mbox{Med}\left\{|\vec{x}-\mbox{Med}(\vec{x})|\right\} \f]

    If \a median is \c !=NULL, the median is returned in this parameter

    \see Ricardo A. Maronna, R. Douglas Martin, Victor J. Yohai: "Robust Statistics: Theory and Methods", Wiley, 2006, ISBN: 978-0-470-01092-1
*/
template <class T>
inline T statisticsSortedMAD(const T* value, long long N, T* median=NULL) {
    if ((!value)) return 0;
    if (N<=1) return 0;
    register double med=statisticsSortedMedian(value, N);
    T* sorted=(T*)statisticsMalloc(N*sizeof(T));
    for (int i=0; i<N; i++) {
        sorted[i]=fabs(value[i]-med);
    }
    statisticsSort(sorted, N);
    register T res=statisticsSortedMedian(sorted, N);
    if (sorted) statisticsFree(sorted);

    if (median) *median=med;
    return res;
}

/*! \brief calculates the normalized median absolute deviation about the median (NMAD), a robust measure of sample deviation, the data has to be a sorted array!

    \f[ \mbox{NMAD}(\vec{x})=\frac{\mbox{MAD}(\vec{x})}{0.6745}=\frac{\mbox{Med}\left\{|\vec{x}-\mbox{Med}(\vec{x})|\right\}}{0.6745} \f]

    If \a median is \c !=NULL, the median is returned in this parameter

    \see Ricardo A. Maronna, R. Douglas Martin, Victor J. Yohai: "Robust Statistics: Theory and Methods", Wiley, 2006, ISBN: 978-0-470-01092-1
*/
template <class T>
inline T statisticsSortedNMAD(const T* value, long long N, T* median=NULL) {
    return statisticsSortedMAD(value, N, median)/0.6745;
}


/*! \brief calculates the median absolute deviation about the median (MAD), a robust measure of sample deviation, the data has to be a sorted array!

    \f[ \mbox{MAD}(\vec{x})=\mbox{Med}\left\{|\vec{x}-\mbox{Med}(\vec{x})|\right\} \f]

    If \a median is \c !=NULL, the median is returned in this parameter

    \see Ricardo A. Maronna, R. Douglas Martin, Victor J. Yohai: "Robust Statistics: Theory and Methods", Wiley, 2006, ISBN: 978-0-470-01092-1
*/
template <class T>
inline T statisticsMAD(const T* value, long long N, T* median=NULL) {
    if ((!value)) return 0;
    if (N<=1) return 0;
    T* sorted=(T*)statisticsMalloc(N*sizeof(T));
    for (int i=0; i<N; i++) {
        sorted[i]=value[i];
    }
    N=statisticsFilterGoodFloat(sorted, N);
    statisticsSort(sorted, N);
    T res=statisticsSortedMAD(sorted, N, median);
    if (sorted) statisticsFree(sorted);
    return res;
}

/*! \brief calculates the normalized median absolute deviation about the median (NMAD), a robust measure of sample deviation, the data has to be a sorted array!

    \f[ \mbox{NMAD}(\vec{x})=\frac{\mbox{MAD}(\vec{x})}{0.6745}=\frac{\mbox{Med}\left\{|\vec{x}-\mbox{Med}(\vec{x})|\right\}}{0.6745} \f]

    If \a median is \c !=NULL, the median is returned in this parameter

    \see Ricardo A. Maronna, R. Douglas Martin, Victor J. Yohai: "Robust Statistics: Theory and Methods", Wiley, 2006, ISBN: 978-0-470-01092-1
*/
template <class T>
inline double statisticsNMAD(const T* value, long long N, T* median=NULL) {
    return statisticsMAD(value, N, median)/0.6745;
}


/*! \brief return the given quantile from a sorted array
    \ingroup tools_math_stat

*/
template <class T>
inline T statisticsSortedQuantile(T* input, long long N, double quantile) {
    if ((!input)) return 0;
    if (N<=0) return 0;
    if (N==1) return input[0];

    long long NQ=(long long)ceil(quantile*(double)N);
    if (NQ<0) NQ=0;
    if (NQ>=N) NQ=N-1;

    return input[NQ];
}





/*! \brief return the median from an array
    \ingroup tools_math_stat

    if \a N is odd, then the center element is returned, otherwise the function returns the average of the two centering elements
*/
template <class T>
inline T statisticsQuantile(const T* input, long long N, double quantile) {
    if ((!input)) return 0;
    if (N<=0) return 0;
    if (N==1) return input[0];
    T* sorted=(T*)statisticsMalloc(N*sizeof(T));
    long long NN=N;
    for (long long i=0; i<NN; i++) {
        sorted[i]=input[i];
    }
    statisticsSort(sorted, NN);
    NN=statisticsFilterGoodFloat(sorted, NN);
    register T res=statisticsSortedQuantile(sorted, NN, quantile);
    if (sorted) statisticsFree(sorted);
    return res;
}


/*! \brief return the median from an array
    \ingroup tools_math_stat

    if \a N is odd, then the center element is returned, otherwise the function returns the average of the two centering elements
*/
template <class T>
inline T statisticsQuantileMasked(const bool* mask, const T* input, long long N, double quantile, bool maskUseValue=true) {
    //std::cout<<"statisticsQuantileMasked("<<mask<<", "<<input<<", "<<N<<", "<<quantile<<", "<<maskUseValue<<")\n";
    if (!mask) return statisticsQuantile(input, N, quantile);
    if (!input) return 0;
    if (N<=0) return 0;
    if (N==1) return input[0];
    T* sorted=(T*)statisticsMalloc(N*sizeof(T));
    long long NN=0;
    for (long long i=0; i<N; i++) {
        if (mask[i]==maskUseValue) {
            sorted[NN]=input[i];
            NN++;
        }
    }
    //std::cout<<"NN="<<NN<<std::endl;
    statisticsSort(sorted, NN);
    NN=statisticsFilterGoodFloat(sorted, NN);
    //std::cout<<"filtered NN="<<NN<<std::endl;;
    const T res=statisticsSortedQuantile(sorted, NN, quantile);
    if (sorted) statisticsFree(sorted);
    return res;
}

/*! \brief calculate a histogram from the given data array, data can be masked
    \ingroup tools_math_stat

    The histogram is normalized by the overall number of items \a N

    \param input input data array
    \param mask if \c !=NULL use only the datapoints, where \c mask[i]==maskUseValue
    \param N number of entries in mask and input
    \param mmin start of values range
    \param mmax end of values range
    \param histX the bin values (center value!) are written into this array, the size has to be bins items
    \param histY the histogram is written into this output array, the size has to be bins items
    \param bins number of bins in the output histogram (histX, histY)
    \param normalized indicates whether the histogram has to be normalized
    \param maskUseValue see mask

*/
template <class T, class THIST>
inline void statisticsHistogramMaskedRanged(const T* input, const bool* mask, long long N, T mmin, T mmax, THIST* histX, THIST* histY, long long bins=11, bool normalized=false, bool maskUseValue=false) {
    if ((!input)) return ;
    for (long long i=0; i<bins; i++)  {
        histY[i]=0;
    }
    if (N<=0) return;


    THIST range=mmax-mmin;
    THIST binw=range/(THIST)bins;
    for (long long i=0; i<bins; i++)  {
        histX[i]=mmin+(THIST)i*binw+binw/2.0;
    }
    for (long long i=0; i<N; i++)  {
        if (!mask || mask[i]==maskUseValue) {
            const T v=input[i];
            if (statisticsFloatIsOK(v)) {
                long long b=floor((v-mmin)/binw);
                if (b<0) b=0;
                if (b>=bins) b=bins-1;
                histY[b]=histY[b]+1;
            }
        }
    }
    if (normalized) {
        for (long long i=0; i<bins; i++)  {
            histY[i]=histY[i]/(THIST)N;
        }
    }
}



/*! \brief calculate a histogram from the given data array, data can be masked, mask only has size Nmask and indexe i=0..N are taken modulo Nmask
    \ingroup tools_math_stat

    The histogram is normalized by the overall number of items \a N

    \param input input data array
    \param mask if \c !=NULL use only the datapoints, where \c mask[i]==maskUseValue
    \param N number of entries in mask and input
    \param mmin start of values range
    \param mmax end of values range
    \param histX the bin values (center value!) are written into this array, the size has to be bins items
    \param histY the histogram is written into this output array, the size has to be bins items
    \param bins number of bins in the output histogram (histX, histY)
    \param normalized indicates whether the histogram has to be normalized
    \param maskUseValue see mask

*/
template <class T, class THIST>
inline void statisticsHistogramModMaskedRanged(const T* input, const bool* mask, long long N, long long Nmask, T mmin, T mmax, THIST* histX, THIST* histY, long long bins=11, bool normalized=false, bool maskUseValue=false) {
    if ((!input)) return ;
    for (long long i=0; i<bins; i++)  {
        histY[i]=0;
    }
    if (N<=0) return;


    THIST range=mmax-mmin;
    THIST binw=range/(THIST)bins;
    for (long long i=0; i<bins; i++)  {
        histX[i]=mmin+(THIST)i*binw+binw/2.0;
    }
    for (long long i=0; i<N; i++)  {
        if (!mask || mask[i%Nmask]==maskUseValue) {
            const T v=input[i];
            if (statisticsFloatIsOK(v)) {
                long long b=floor((v-mmin)/binw);
                if (b<0) b=0;
                if (b>=bins) b=bins-1;
                histY[b]=histY[b]+1;
            }
        }
    }
    if (normalized) {
        for (long long i=0; i<bins; i++)  {
            histY[i]=histY[i]/(THIST)N;
        }
    }
}



/*! \brief calculate a histogram from the given data array, range is determined from the data, data can be masked, mask only has size Nmask and indexe i=0..N are taken modulo Nmask
    \ingroup tools_math_stat

    The histogram is normalized by the overall number of items \a N

    \param input input data array
    \param mask if \c !=NULL use only the datapoints, where \c mask[i]==maskUseValue
    \param N number of entries in mask and input
    \param histX the bin values (center value!) are written into this array, the size has to be bins items
    \param histY the histogram is written into this output array, the size has to be bins items
    \param bins number of bins in the output histogram (histX, histY)
    \param normalized indicates whether the histogram has to be normalized
    \param maskUseValue see mask

*/
template <class T, class THIST>
inline void statisticsHistogramModMasked(const T* input, const bool* mask, long long N, long long Nmask, THIST* histX, THIST* histY, long long bins=11, bool normalized=false, bool maskUseValue=false) {
    if ((!input) || (N<=0)) return;
    T mmin, mmax;
    statisticsModMaskedMinMax(input, mask, N, Nmask, mmin, mmax, maskUseValue);
    statisticsHistogramModMaskedRanged(input, mask, N, Nmask, mmin, mmax, histX, histY, bins, normalized, maskUseValue);
}




/*! \brief calculate a histogram from the given data array, range is determined from the data
    \ingroup tools_math_stat

    The histogram is normalized by the overall number of items \a N

    \param input input data array
    \param N number of entries in mask and input
    \param histX the bin values (center value!) are written into this array, the size has to be bins items
    \param histY the histogram is written into this output array, the size has to be bins items
    \param bins number of bins in the output histogram (histX, histY)
    \param normalized indicates whether the histogram has to be normalized

*/
template <class T, class THIST>
inline void statisticsHistogram(const T* input, long long N, THIST* histX, THIST* histY, long long bins=11, bool normalized=false) {
    if ((!input) || (N<=0)) return;
    T mmin, mmax;
    statisticsMinMax(input, N, mmin, mmax);
    statisticsHistogramMaskedRanged(input, NULL, N, mmin, mmax, histX, histY, bins, normalized);
}

/*! \brief calculate a histogram from the given data array, range is determined from the data, data can be masked
    \ingroup tools_math_stat

    The histogram is normalized by the overall number of items \a N

    \param input input data array
    \param mask if \c !=NULL use only the datapoints, where \c mask[i]==maskUseValue
    \param N number of entries in mask and input
    \param histX the bin values (center value!) are written into this array, the size has to be bins items
    \param histY the histogram is written into this output array, the size has to be bins items
    \param bins number of bins in the output histogram (histX, histY)
    \param normalized indicates whether the histogram has to be normalized
    \param maskUseValue see mask

*/
template <class T, class THIST>
inline void statisticsHistogramMasked(const T* input, const bool* mask, long long N, THIST* histX, THIST* histY, long long bins=11, bool normalized=false, bool maskUseValue=false) {
    if ((!input) || (N<=0)) return;
    T mmin, mmax;
    statisticsMaskedMinMax(input, mask, N, mmin, mmax, maskUseValue);
    statisticsHistogramMaskedRanged(input, mask, N, mmin, mmax, histX, histY, bins, normalized, maskUseValue);
}

/*! \brief calculate a histogram from the given data array, only care for values that are in the given range, returns the number of values in the range
    \ingroup tools_math_stat

    the histogram is normalized by the number of values in the range, not the overall number of values \a N

    \param input input data array
    \param N number of entries in mask and input
    \param mmin start of values range
    \param mmax end of values range
    \param histX the bin values (center value!) are written into this array, the size has to be bins items
    \param histY the histogram is written into this output array, the size has to be bins items
    \param bins number of bins in the output histogram (histX, histY)
    \param normalized indicates whether the histogram has to be normalized

*/
template <class T, class THIST>
inline void statisticsHistogramRanged(const T* input, long long N, T mmin, T mmax, THIST* histX, THIST* histY, long long bins=11, bool normalized=false) {
   statisticsHistogramMaskedRanged(input, NULL, N, mmin, mmax, histX, histY, bins, normalized);
}


/*! \brief accumulate the given histogram
    \ingroup tools_math_stat

*/
template <class THIST>
inline void statisticsAccumulateHistogram(THIST* histY, long long bins) {
    for (int i=1; i<bins; i++) {
        histY[i]=histY[i]+histY[i-1];
    }
}


/*! \brief normalize the given histogram, so \f$ \sum\limits_ih_i=1 \f$
    \ingroup tools_math_stat

*/
template <class THIST>
inline void statisticsNormalize(THIST* histY, long long bins) {
    THIST sum=statisticsSum(histY, bins);
    for (int i=0; i<bins; i++) {
        histY[i]=histY[i]/sum;
    }
}





/*! \brief a 1D Gaussian kernel function, e.g. for Kernel Density Estimation
    \ingroup tools_math_stat

    \f[ k(t):=\frac{1}{\sqrt{2\pi}}\exp \left(-\frac{1}{2}t^2\right) \f]
*/
LIB_EXPORT double statisticsKernel1DGaussian(double t);
/*! \brief a 1D Cauchy kernel function, e.g. for Kernel Density Estimation
    \ingroup tools_math_stat

    \f[ k(t):=\frac{1}{\pi(1+t^2)} \f]
*/
LIB_EXPORT double statisticsKernel1DCauchy(double t);

/*! \brief a 1D Picard kernel function, e.g. for Kernel Density Estimation
    \ingroup tools_math_stat

    \f[ k(t):=\frac{1}{2}\exp(-|t|) \f]
*/
LIB_EXPORT double statisticsKernel1DPicard(double t);
/*! \brief a 1D Epanechnikov kernel function, e.g. for Kernel Density Estimation
    \ingroup tools_math_stat

    \f[ k(t) :=\begin{cases}\frac{3}{4} ( 1- t^2 ),  & \text{if }t\in [-1;1]\\0, & \text{else}\end{cases} \f]
*/
LIB_EXPORT double statisticsKernel1DEpanechnikov(double t);
/*! \brief a 1D uniform kernel function, e.g. for Kernel Density Estimation
    \ingroup tools_math_stat

    \f[ k(t) :=\begin{cases}1,  & \text{if }t\in [-1;1]\\0, & \text{else}\end{cases} \f]
*/
LIB_EXPORT double statisticsKernel1DUniform(double t);
/*! \brief a 1D Epanechnikov kernel function, e.g. for Kernel Density Estimation
    \ingroup tools_math_stat

    \f[ k(t) :=\begin{cases}1-|t|,  & \text{if }t\in [-1;1]\\0, & \text{else}\end{cases} \f]
*/
LIB_EXPORT double statisticsKernel1DTriangle(double t);

/*! \brief a 1D quartic kernel function, e.g. for Kernel Density Estimation
    \ingroup tools_math_stat

    \f[ k(t) :=\begin{cases}\frac{15}{16}(1-t^2)^2,  & \text{if }t\in [-1;1]\\0, & \text{else}\end{cases} \f]
*/
LIB_EXPORT double statisticsKernel1DQuartic(double t);
/*! \brief a 1D triweight kernel function, e.g. for Kernel Density Estimation
    \ingroup tools_math_stat

    \f[ k(t) :=\begin{cases}\frac{35}{32}(1-t^2)^3,  & \text{if }t\in [-1;1]\\0, & \text{else}\end{cases} \f]
*/
LIB_EXPORT double statisticsKernel1DTriweight(double t);

/*! \brief a 1D tricube kernel function, e.g. for Kernel Density Estimation
    \ingroup tools_math_stat

    \f[ k(t) :=\begin{cases}\frac{70}{81}(1-|t|^3)^3,  & \text{if }t\in [-1;1]\\0, & \text{else}\end{cases} \f]
*/
LIB_EXPORT double statisticsKernel1DTricube(double t);
/*! \brief a 1D cosine kernel function, e.g. for Kernel Density Estimation
    \ingroup tools_math_stat

    \f[ k(t) :=\begin{cases}\frac{\pi}{4}\cos\left(\frac{\pi}{2}t\right),  & \text{if }t\in [-1;1]\\0, & \text{else}\end{cases} \f]
*/
LIB_EXPORT double statisticsKernel1DCosine(double t);













/*! \brief a 1D Gaussian kernel function, e.g. for Kernel Density Estimation
    \ingroup tools_math_stat

    \f[ k(t_x, t_y):=\frac{1}{2\pi}\exp \left(-\frac{t_x^2+t_y^2}{2}\right) \f]
*/
LIB_EXPORT double statisticsKernel2DGaussian(double tx, double ty);

/*! \brief a 1D Gaussian kernel function, e.g. for Kernel Density Estimation
    \ingroup tools_math_stat

    \f[ k(t_x, t_y):=\begin{cases}\frac{1}{4},  & \text{if }t_x,t_y\in [-1;1]\\0, & \text{else}\end{cases} \f]
*/
LIB_EXPORT double statisticsKernel2DUniform(double tx, double ty);



















/*! \brief evaluates the Kernel Density Estimator (KDE) at a given position
    \ingroup tools_math_stat

    evaluates \f[ \tilde{f}(t):=\frac{1}{N\cdot\text{bandwidth}}\cdot\sum\limits_{i=0}^{N-1}K\left(\frac{t-x_i}{\text{bandwidth}}\right) \f]

    \param input input data array
    \param mask if \c !=NULL use only the datapoints, where \c mask[i]==maskUseValue
    \param N number of entries in mask and input
    \param kernel a kernel function or functor, e.g. statisticsKernel1DGaussian() or statisticsKernel1DEpanechnikov()
    \param bandwidth a smoothing parameter
    \param maskUseValue see mask

*/
template <class T, class THIST, typename TKERNEL>
inline THIST statisticsEvaluateKernelSum(THIST t, const T* input, const bool* mask, long long N, TKERNEL kernel, THIST bandwidth=1.0, bool maskUseValue=false) {
    if (!input || N<=0) return 0.0;

    register THIST res=0;
    register THIST cnt=0;
    for (long long i=0; i<N; i++)  {
        if ((!mask || mask[i]==maskUseValue) && statisticsFloatIsOK(input[i])) {
            const THIST vx=(t-(THIST)input[i])/bandwidth;
            res+=kernel(vx);
            cnt++;
        }
    }
    return res/cnt/bandwidth;
}


/*! \brief calculate a Kernel Density Estimation (KDE) histogram from the given data array
    \ingroup tools_math_stat

    This function performs <a href="https://en.wikipedia.org/wiki/Kernel_density_estimation">Kernel Density Estimation</a> for a given data array.
    Then the resulting density is evaluated on a regular grid spannung [mmin...mmax] with bins datapoints in between. This might give smoother results,
    than simple histograms, if few datapoints are available.

    \note this functions is getting very slow for large dataset, as for each point in the resulting histogram N kernel functions have to be evaluated.

    \param input input data array
    \param mask if \c !=NULL use only the datapoints, where \c mask[i]==maskUseValue
    \param N number of entries in mask and input
    \param mmin start of values range
    \param mmax end of values range
    \param histX the bin values (center value!) are written into this array, the size has to be bins items
    \param histY the histogram is written into this output array, the size has to be bins items
    \param bins number of bins in the output histogram (histX, histY)
    \param normalized indicates whether the histogram has to be normalized
    \param maskUseValue see mask


*/
template <class T, class THIST, typename TKERNEL>
inline void statisticsKDEHistogramMaskedRanged(const T* input, const bool* mask, long long N, TKERNEL kernel, THIST bandwidth, T mminX, T mmaxX, THIST* histX, THIST* histY, long long bins=101, bool maskUseValue=false) {
    if ((!input)) return ;
    for (long long i=0; i<bins; i++)  {
        histY[i]=0;
    }
    if (N<=0) return;


    THIST range=mmaxX-mminX;
    THIST binw=range/(THIST)bins;
    for (long long i=0; i<bins; i++)  {
        histX[i]=mminX+(THIST)i*binw+binw/2.0;
        histY[i]=statisticsEvaluateKernelSum(histX[i], input, mask, N, kernel, bandwidth, maskUseValue);
    }

}

/*! \brief calculate a Kernel Density Estimation (KDE) histogram from the given data array, range is determined automatically
    \ingroup tools_math_stat

    This function performs <a href="https://en.wikipedia.org/wiki/Kernel_density_estimation">Kernel Density Estimation</a> for a given data array.
    Then the resulting density is evaluated on a regular grid spannung [mmin...mmax] with bins datapoints in between. This might give smoother results,
    than simple histograms, if few datapoints are available.

    \note this functions is getting very slow for large dataset, as for each point in the resulting histogram N kernel functions have to be evaluated.

    \param input input data array
    \param mask if \c !=NULL use only the datapoints, where \c mask[i]==maskUseValue
    \param N number of entries in mask and input
    \param histX the bin values (center value!) are written into this array, the size has to be bins items
    \param histY the histogram is written into this output array, the size has to be bins items
    \param bins number of bins in the output histogram (histX, histY)
    \param normalized indicates whether the histogram has to be normalized
    \param maskUseValue see mask


*/
template <class T, class THIST, typename TKERNEL>
inline void statisticsKDEHistogramMasked(const T* input, const bool* mask, long long N, TKERNEL kernel, THIST bandwidth, THIST* histX, THIST* histY, long long bins=101, bool maskUseValue=false) {
    if ((!input)) return ;
    T mminX,  mmaxX;
    statisticsMaskedMinMax(input, mask, N, mminX, mmaxX, maskUseValue);
    statisticsKDEHistogramMaskedRanged(input, mask, N, kernel, bandwidth, mminX, mmaxX, histX, histY, bins, maskUseValue);
}



/*! \brief calculate a Kernel Density Estimation (KDE) histogram from the given data array, range is determined automatically, no mask
    \ingroup tools_math_stat

    This function performs <a href="https://en.wikipedia.org/wiki/Kernel_density_estimation">Kernel Density Estimation</a> for a given data array.
    Then the resulting density is evaluated on a regular grid spannung [mmin...mmax] with bins datapoints in between. This might give smoother results,
    than simple histograms, if few datapoints are available.

    \note this functions is getting very slow for large dataset, as for each point in the resulting histogram N kernel functions have to be evaluated.

    \param input input data array
    \param N number of entries in mask and input
    \param histX the bin values (center value!) are written into this array, the size has to be bins items
    \param histY the histogram is written into this output array, the size has to be bins items
    \param bins number of bins in the output histogram (histX, histY)
    \param normalized indicates whether the histogram has to be normalized


*/
template <class T, class THIST, typename TKERNEL>
inline void statisticsKDEHistogram(const T* input, long long N, TKERNEL kernel, THIST bandwidth, THIST* histX, THIST* histY, long long bins=101) {
    statisticsKDEHistogramMasked(input, NULL, N, kernel, bandwidth, histX, histY, bins);
}



/*! \brief calculate a Kernel Density Estimation (KDE) histogram from the given data array, no mask
    \ingroup tools_math_stat

    This function performs <a href="https://en.wikipedia.org/wiki/Kernel_density_estimation">Kernel Density Estimation</a> for a given data array.
    Then the resulting density is evaluated on a regular grid spannung [mmin...mmax] with bins datapoints in between. This might give smoother results,
    than simple histograms, if few datapoints are available.

    \note this functions is getting very slow for large dataset, as for each point in the resulting histogram N kernel functions have to be evaluated.

    \param input input data array
    \param N number of entries in mask and input
    \param histX the bin values (center value!) are written into this array, the size has to be bins items
    \param histY the histogram is written into this output array, the size has to be bins items
    \param bins number of bins in the output histogram (histX, histY)
    \param normalized indicates whether the histogram has to be normalized


*/
template <class T, class THIST, typename TKERNEL>
inline void statisticsKDEHistogramRanged(const T* input, long long N, TKERNEL kernel, THIST bandwidth, T mmin, T mmax, THIST* histX, THIST* histY, long long bins=101) {
    statisticsKDEHistogramMaskedRanged(input, NULL, N, kernel, bandwidth, mmin, mmax, histX, histY, bins);
}















/*! \brief evaluates the Kernel Density Estimator (KDE) at a given position
    \ingroup tools_math_stat

    evaluates \f[ \tilde{f}(t_x, t_y):=\frac{1}{N\cdot\sqrt{\text{bandwidthX}*\text{bandwidthY}}}\cdot\sum\limits_{i=0}^{N-1}K\left(\frac{t_x-x_i}{\text{bandwidthX}}, \frac{t_y-y_i}{\text{bandwidthY}}\right) \f]

    \param tx evaluate at this x-coordinate
    \param ty evaluate at this y-coordinate
    \param inputX input data array x-values
    \param inputY input data array y-values
    \param mask if \c !=NULL use only the datapoints, where \c mask[i]==maskUseValue
    \param N number of entries in mask and input
    \param kernel a kernel function or functor, e.g. statisticsKernel2DGaussian()
    \param bandwidthX a smoothing parameter in x direction
    \param bandwidthY a smoothing parameter in y direction
    \param maskUseValue see mask

*/
template <class T, class THIST, typename TKERNEL>
inline THIST statisticsEvaluate2DKernelSum(THIST tx, THIST ty, const T* inputX, const T* inputY, const bool* mask, long long N, TKERNEL kernel, THIST bandwidthX=1.0, THIST bandwidthY=1.0, bool maskUseValue=false) {
    if (!inputX || !inputY || N<=0) return 0.0;

    register THIST res=0;
    register THIST cnt=0;
    for (long long i=0; i<N; i++)  {
        if ((!mask || mask[i]==maskUseValue) && statisticsFloatIsOK(inputX[i]) && statisticsFloatIsOK(inputY[i])) {
            const THIST vx=(tx-(THIST)inputX[i])/bandwidthX;
            const THIST vy=(ty-(THIST)inputY[i])/bandwidthY;
            res+=kernel(vx, vy);
            cnt++;
        }
    }
    return res/cnt/sqrt(bandwidthX)/sqrt(bandwidthY);
}


/*! \brief calculate a Kernel Density Estimation (KDE) histogram from the given data array
    \ingroup tools_math_stat

    This function performs <a href="https://en.wikipedia.org/wiki/Kernel_density_estimation">Kernel Density Estimation</a> for a given data array.
    Then the resulting density is evaluated on a regular grid spannung [mmin...mmax] with bins datapoints in between. This might give smoother results,
    than simple histograms, if few datapoints are available.

    \note this functions is getting very slow for large dataset, as for each point in the resulting histogram N kernel functions have to be evaluated.

    \param inputX first input data array
    \param inputY second input array
    \param mask if \c !=NULL use only the datapoints, where \c mask[i]==maskUseValue
    \param N number of entries in mask and inputX and inputY
    \param kernel kernel function or functor to use, e.g. statisticsKernel2DGaussian()
    \param bandwidthX a smoothing parameter in x direction
    \param bandwidthY a smoothing parameter in y direction
    \param mminX start of values range for inputX
    \param mmaxX end of values range for inputX
    \param mminY start of values range for inputY
    \param mmaxY end of values range for inputY
    \param histAxisX the bin values (center value!) for inputX are written into this array, the size has to be binsX items
    \param histAxisY the bin values (center value!) for inputY are written into this array, the size has to be binsY items
    \param histogram the histogram is written into this output array, the size has to be binsX*binsY items
    \param binsX number of bins in the output histogram on the x-axis
    \param binsY number of bins in the output histogram on the y-axis
    \param normalized indicates whether the histogram has to be normalized
    \param maskUseValue see mask


*/
template <class T, class THIST, typename TKERNEL>
inline void statistics2DKDEHistogramMaskedRanged(const T* inputX, const T* inputY, const bool* mask, long long N, TKERNEL kernel, THIST bandwidthX, THIST bandwidthY, T mminX, T mmaxX, T mminY, T mmaxY, THIST* histX, THIST* histY, THIST* histogram, long long binsX=101, long long binsY=101, bool maskUseValue=false) {
    if ((!inputX)||(!inputY)) return ;
    for (long long i=0; i<binsX*binsY; i++)  {
        histogram[i]=0;
    }
    if (N<=0) return;


    THIST rangeX=mmaxX-mminX;
    THIST binwX=rangeX/(THIST)binsX;
    for (long long i=0; i<binsX; i++)  {
        histX[i]=mminX+(THIST)i*binwX+binwX/2.0;
    }

    THIST rangeY=mmaxY-mminY;
    THIST binwY=rangeY/(THIST)binsY;
    for (long long i=0; i<binsY; i++)  {
        histY[i]=mminY+(THIST)i*binwY+binwY/2.0;
    }

    for (long long y=0; y<binsY; y++)  {
        for (long long x=0; x<binsX; x++)  {
            histogram[y*binsX+x]=statisticsEvaluate2DKernelSum(histX[x], histY[y], inputX, inputY, mask, N, kernel, bandwidthX, bandwidthY, maskUseValue);
        }
    }

}

/*! \brief calculate a Kernel Density Estimation (KDE) histogram from the given data array
    \ingroup tools_math_stat

    This function performs <a href="https://en.wikipedia.org/wiki/Kernel_density_estimation">Kernel Density Estimation</a> for a given data array.
    Then the resulting density is evaluated on a regular grid spannung [mmin...mmax] with bins datapoints in between. This might give smoother results,
    than simple histograms, if few datapoints are available.

    \note this functions is getting very slow for large dataset, as for each point in the resulting histogram N kernel functions have to be evaluated.

    \param inputX first input data array
    \param inputY second input array
    \param mask if \c !=NULL use only the datapoints, where \c mask[i]==maskUseValue
    \param N number of entries in mask and inputX and inputY
    \param kernel kernel function or functor to use, e.g. statisticsKernel2DGaussian()
    \param bandwidthX a smoothing parameter in x direction
    \param bandwidthY a smoothing parameter in y direction
    \param mminX start of values range for inputX
    \param mmaxX end of values range for inputX
    \param mminY start of values range for inputY
    \param mmaxY end of values range for inputY
    \param histAxisX the bin values (center value!) for inputX are written into this array, the size has to be binsX items
    \param histAxisY the bin values (center value!) for inputY are written into this array, the size has to be binsY items
    \param histogram the histogram is written into this output array, the size has to be binsX*binsY items
    \param binsX number of bins in the output histogram on the x-axis
    \param binsY number of bins in the output histogram on the y-axis
    \param normalized indicates whether the histogram has to be normalized
    \param maskUseValue see mask


*/
template <class T, class THIST, typename TKERNEL>
inline void statistics2DKDEHistogramMasked(const T* inputX, const T* inputY, const bool* mask, long long N, TKERNEL kernel, THIST bandwidthX, THIST bandwidthY, THIST* histAxisX, THIST* histAxisY, THIST* histogram, long long binsX=101, long long binsY=101, bool maskUseValue=false) {
    if ((!inputX) || (!inputY)) return ;
    for (long long i=0; i<binsX*binsY; i++)  {
        histogram[i]=0;
    }
    if (N<=0) return;

    T mmaxX, mminX;
    statisticsMaskedMinMax(inputX, mask, N, mminX, mmaxX, maskUseValue);
    T mmaxY, mminY;
    statisticsMaskedMinMax(inputY, mask, N, mminY, mmaxY, maskUseValue);

    statistics2DKDEHistogramMaskedRanged(inputX, inputY, mask, N, kernel, bandwidthX, bandwidthY, mminX, mmaxX, mminY, mmaxY, histAxisX, histAxisY, histogram, binsX, binsY, maskUseValue);
}


/*! \brief calculate a Kernel Density Estimation (KDE) histogram from the given data array
    \ingroup tools_math_stat

    This function performs <a href="https://en.wikipedia.org/wiki/Kernel_density_estimation">Kernel Density Estimation</a> for a given data array.
    Then the resulting density is evaluated on a regular grid spannung [mmin...mmax] with bins datapoints in between. This might give smoother results,
    than simple histograms, if few datapoints are available.

    \note this functions is getting very slow for large dataset, as for each point in the resulting histogram N kernel functions have to be evaluated.

    \param inputX first input data array
    \param inputY second input array
    \param mask if \c !=NULL use only the datapoints, where \c mask[i]==maskUseValue
    \param N number of entries in mask and inputX and inputY
    \param kernel kernel function or functor to use, e.g. statisticsKernel2DGaussian()
    \param bandwidthX a smoothing parameter in x direction
    \param bandwidthY a smoothing parameter in y direction
    \param mminX start of values range for inputX
    \param mmaxX end of values range for inputX
    \param mminY start of values range for inputY
    \param mmaxY end of values range for inputY
    \param histAxisX the bin values (center value!) for inputX are written into this array, the size has to be binsX items
    \param histAxisY the bin values (center value!) for inputY are written into this array, the size has to be binsY items
    \param histogram the histogram is written into this output array, the size has to be binsX*binsY items
    \param binsX number of bins in the output histogram on the x-axis
    \param binsY number of bins in the output histogram on the y-axis
    \param normalized indicates whether the histogram has to be normalized
    \param maskUseValue see mask


*/
template <class T, class THIST, typename TKERNEL>
inline void statistics2DKDEHistogram(const T* inputX, const T* inputY, long long N, TKERNEL kernel, THIST bandwidthX, THIST bandwidthY, THIST* histAxisX, THIST* histAxisY, THIST* histogram, long long binsX=101, long long binsY=101) {
    statistics2DKDEHistogramMasked(inputX, inputY, NULL, N, kernel, bandwidthX, bandwidthY, histAxisX, histAxisY, histogram, binsX, binsY);
}

/*! \brief calculate a Kernel Density Estimation (KDE) histogram from the given data array
    \ingroup tools_math_stat

    This function performs <a href="https://en.wikipedia.org/wiki/Kernel_density_estimation">Kernel Density Estimation</a> for a given data array.
    Then the resulting density is evaluated on a regular grid spannung [mmin...mmax] with bins datapoints in between. This might give smoother results,
    than simple histograms, if few datapoints are available.

    \note this functions is getting very slow for large dataset, as for each point in the resulting histogram N kernel functions have to be evaluated.

    \param inputX first input data array
    \param inputY second input array
    \param mask if \c !=NULL use only the datapoints, where \c mask[i]==maskUseValue
    \param N number of entries in mask and inputX and inputY
    \param kernel kernel function or functor to use, e.g. statisticsKernel2DGaussian()
    \param bandwidthX a smoothing parameter in x direction
    \param bandwidthY a smoothing parameter in y direction
    \param mminX start of values range for inputX
    \param mmaxX end of values range for inputX
    \param mminY start of values range for inputY
    \param mmaxY end of values range for inputY
    \param histAxisX the bin values (center value!) for inputX are written into this array, the size has to be binsX items
    \param histAxisY the bin values (center value!) for inputY are written into this array, the size has to be binsY items
    \param histogram the histogram is written into this output array, the size has to be binsX*binsY items
    \param binsX number of bins in the output histogram on the x-axis
    \param binsY number of bins in the output histogram on the y-axis
    \param normalized indicates whether the histogram has to be normalized
    \param maskUseValue see mask


*/
template <class T, class THIST, typename TKERNEL>
inline void statistics2DKDEHistogramRanged(const T* inputX, const T* inputY, long long N, TKERNEL kernel, THIST bandwidthX, THIST bandwidthY, T mminX, T mmaxX, T mminY, T mmaxY, THIST* histAxisX, THIST* histAxisY, THIST* histogram, long long binsX=101, long long binsY=101) {
    statistics2DKDEHistogramMaskedRanged(inputX, inputY, NULL, N, kernel, bandwidthX, bandwidthY, mminX, mmaxX, mminY, mmaxY, histAxisX, histAxisY, histogram, binsX, binsY);
}












/*! \brief calculate a 2D histogram from the given data array and given ranges
    \ingroup tools_math_stat

    The histogram is normalized by the overall number of items \a N

    \param inputX first input data array
    \param inputY second input array
    \param mask if \c !=NULL use only the datapoints, where \c mask[i]==maskUseValue
    \param N number of entries in mask and inputX and inputY
    \param mminX start of values range for inputX
    \param mmaxX end of values range for inputX
    \param mminY start of values range for inputY
    \param mmaxY end of values range for inputY
    \param histAxisX the bin values (center value!) for inputX are written into this array, the size has to be binsX items
    \param histAxisY the bin values (center value!) for inputY are written into this array, the size has to be binsY items
    \param histogram the histogram is written into this output array, the size has to be binsX*binsY items
    \param binsX number of bins in the output histogram on the x-axis
    \param binsY number of bins in the output histogram on the y-axis
    \param normalized indicates whether the histogram has to be normalized
    \param maskUseValue see mask

*/
template <class T, class THIST>
inline void statisticsHistogram2DMaskedRanged(const T* inputX, const T* inputY, const bool* mask, long long N, T mminX, T mmaxX, T mminY, T mmaxY,  THIST* histAxisX, THIST* histAxisY, THIST* histogram, long long binsX=11, long long binsY=11, bool normalized=false, bool maskUseValue=false) {
    if ((!inputX) || (!inputY)) return ;
    for (long long i=0; i<binsX*binsY; i++)  {
        histogram[i]=0;
    }
    if (N<=0) return;

    THIST rangeX=mmaxX-mminX;
    THIST binwX=rangeX/(THIST)binsX;
    if (histAxisX) {
        for (long long i=0; i<binsX; i++)  {
            histAxisX[i]=mminX+(THIST)i*binwX+binwX/2.0;
        }
    }

    THIST rangeY=mmaxY-mminY;
    THIST binwY=rangeY/(THIST)binsY;
    if (histAxisY) {
        for (long long i=0; i<binsY; i++)  {
            histAxisY[i]=mminY+(THIST)i*binwY+binwY/2.0;
        }
    }

    for (long long i=0; i<N; i++)  {
        if (!mask || mask[i]==maskUseValue) {
            const T vx=inputX[i];
            const T vy=inputY[i];
            if (statisticsFloatIsOK(vx) && statisticsFloatIsOK(vy)) {
                long long bx=floor((vx-mminX)/binwX);
                if (bx<0) bx=0;
                if (bx>=binsX) bx=binsX-1;
                long long by=floor((vy-mminY)/binwY);
                if (by<0) by=0;
                if (by>=binsY) by=binsY-1;

                histogram[by*binsX+bx]=histogram[by*binsX+bx]+1;
            }
        }
    }
    if (normalized) {
        for (long long i=0; i<binsX*binsY; i++)  {
            histogram[i]=histogram[i]/(THIST)N;
        }
    }
}

/*! \brief calculate a 2D histogram from the given data array, the ranges are determined automatically
    \ingroup tools_math_stat

    The histogram is normalized by the overall number of items \a N

    \param inputX first input data array
    \param inputY second input array
    \param mask if \c !=NULL use only the datapoints, where \c mask[i]==maskUseValue
    \param N number of entries in mask and inputX and inputY
    \param histAxisX the bin values (center value!) for inputX are written into this array, the size has to be binsX items
    \param histAxisY the bin values (center value!) for inputY are written into this array, the size has to be binsY items
    \param histogram the histogram is written into this output array, the size has to be binsX*binsY items
    \param binsX number of bins in the output histogram on the x-axis
    \param binsY number of bins in the output histogram on the y-axis
    \param normalized indicates whether the histogram has to be normalized
    \param maskUseValue see mask

*/
template <class T, class THIST>
inline void statisticsHistogram2DMasked(const T* inputX, const T* inputY, const bool* mask, long long N,  THIST* histAxisX, THIST* histAxisY, THIST* histogram, long long binsX=11, long long binsY=11, bool normalized=false, bool maskUseValue=false) {
    if ((!inputX) || (!inputY)) return ;
    for (long long i=0; i<binsX*binsY; i++)  {
        histogram[i]=0;
    }
    if (N<=0) return;

    T mmaxX, mminX;
    statisticsMaskedMinMax(inputX, mask, N, mminX, mmaxX, maskUseValue);
    T mmaxY, mminY;
    statisticsMaskedMinMax(inputY, mask, N, mminY, mmaxY, maskUseValue);

    statisticsHistogram2DMaskedRanged(inputX, inputY, mask, N, mminX, mmaxX, mminY, mmaxY, histAxisX, histAxisY, histogram, binsX, binsY, normalized, maskUseValue);
}


/*! \brief calculate a 2D histogram from the given data array, the ranges are determined automatically
    \ingroup tools_math_stat

    The histogram is normalized by the overall number of items \a N

    \param inputX first input data array
    \param inputY second input array
    \param N number of entries in mask and inputX and inputY
    \param histAxisX the bin values (center value!) for inputX are written into this array, the size has to be binsX items
    \param histAxisY the bin values (center value!) for inputY are written into this array, the size has to be binsY items
    \param histogram the histogram is written into this output array, the size has to be binsX*binsY items
    \param binsX number of bins in the output histogram on the x-axis
    \param binsY number of bins in the output histogram on the y-axis
    \param normalized indicates whether the histogram has to be normalized

*/
template <class T, class THIST>
inline void statisticsHistogram2D(const T* inputX, const T* inputY, long long N, THIST* histAxisX, THIST* histAxisY, THIST* histogram, long long binsX=11, long long binsY=11, bool normalized=false) {
    statisticsHistogram2DMasked(inputX, inputY, NULL, N, histAxisX, histAxisY, histogram, binsX, binsY, normalized);
}

/*! \brief calculate a 2D histogram from the given data array for all values within given ranges
    \ingroup tools_math_stat

    The histogram is normalized by the overall number of items \a N


    \param inputX first input data array
    \param inputY second input array
    \param N number of entries in mask and inputX and inputY
    \param mminX start of values range for inputX
    \param mmaxX end of values range for inputX
    \param mminY start of values range for inputY
    \param mmaxY end of values range for inputY
    \param histAxisX the bin values (center value!) for inputX are written into this array, the size has to be binsX items
    \param histAxisY the bin values (center value!) for inputY are written into this array, the size has to be binsY items
    \param histogram the histogram is written into this output array, the size has to be binsX*binsY items
    \param binsX number of bins in the output histogram on the x-axis
    \param binsY number of bins in the output histogram on the y-axis
    \param normalized indicates whether the histogram has to be normalized
*/
template <class T, class THIST>
inline void statisticsHistogram2DRanged(const T* inputX, const T* inputY, long long N, T mminX, T mmaxX, T mminY, T mmaxY,  THIST* histAxisX, THIST* histAxisY, THIST* histogram, long long binsX=11, long long binsY=11, bool normalized=false) {
    statisticsHistogram2DMaskedRanged(inputX, inputY, NULL, N, mminX, mmaxX, mminY, mmaxY, histAxisX, histAxisY, histogram, binsX, binsY, normalized);
}


















/*! \brief calculate the linear regression coefficients for a given dataset where the model is \f$ f(x)=a+b\cdot x \f$
    \ingroup tools_math_stat

    This function computes:
    \f[ a=\overline{y}-b\cdot\overline{x} \f]
    \f[ b=\frac{\sum x_iy_i-N\cdot\overline{x}\cdot\overline{y}}{\sum x_i^2-N\cdot(\overline{x})^2} \f]
*/
template <class T>
inline bool statisticsLinearRegression(const T* dataX, const T* dataY, long long N, double& coeffA, double& coeffB, bool fixA=false, bool fixB=false) {
    if ((!dataX)||(!dataY)) return false;

    if (N<2) return false;
    T sumx=0, sumy=0, sumxy=0, sumx2=0;
    double NN=0;
    for (int jj=0; jj<N; jj++) {
        const T fit_x=dataX[jj];
        const T fit_y=dataY[jj];
        if (statisticsFloatIsOK(fit_x) && statisticsFloatIsOK(fit_y)) {
            sumx=sumx+fit_x;
            sumy=sumy+fit_y;
            sumxy=sumxy+fit_x*fit_y;
            sumx2=sumx2+fit_x*fit_x;
            NN++;
        }
    }
    if (NN<2) return false;
    if (!fixA && !fixB) {
        coeffB=(double(sumxy)-double(sumx)*double(sumy)/NN)/(double(sumx2)-double(sumx)*double(sumx)/NN);;
        coeffA=double(sumy)/NN-coeffB*double(sumx)/NN;
    } else if (fixA && !fixB) {
        coeffB=(double(sumy)/NN-coeffA)/(double(sumx)/NN);
    } else if (!fixA && fixB) {
        coeffA=double(sumy)/NN-coeffB*double(sumx)/NN;
    } else {
        return false;
    }
    return true;
}


/*! \brief calculate the weighted linear regression coefficients for a given dataset where the model is \f$ f(x)=a+b\cdot x \f$
    \ingroup tools_math_stat

    This function computes:
    \f[ a=\frac{\overline{y}-b\cdot\overline{x}}{\overline{w^2}} \f]
    \f[ b=\frac{\overline{w^2}\cdot\overline{x\cdot y}-\overline{x}\cdot\overline{y}}{\overline{x^2}\cdot\overline{w^2}-\overline{x}^2} \f]
    Here the averages are defined in terms of a weight vector \f$ w_i\f$:
    \f[ \overline{x}=\sum\limits_iw_i^2\cdot x_i \f]
    \f[ \overline{y}=\sum\limits_iw_i^2\cdot y_i \f]
    \f[ \overline{x\cdot y}=\sum\limits_iw_i^2\cdot x_i\cdot y_i \f]
    \f[ \overline{x^2}=\sum\limits_iw_i^2\cdot x_i^2 \f]
    \f[ \overline{w^2}=\sum\limits_iw_i^2 \f]

    So this function solves the optimization problem:
    \f[ (a^\ast, b^\ast)=\argmin\limits_{a,b}\sum\limits_iw_i^2\cdot\left(y_i-(a+b\cdot x_i)\right)^2 \f]
*/
template <class T>
inline bool statisticsLinearWeightedRegression(const T* dataX, const T* dataY, const T* weights, long long N, double& coeffA, double& coeffB, bool fixA=false, bool fixB=false) {
    if ((!dataX)||(!dataY)||(!weights)) return false;

    if (N<2) return false;
    T sumx=0, sumy=0, sumxy=0, sumx2=0, sumw2=0;
    for (int jj=0; jj<N; jj++) {
        const T fit_x=dataX[jj];
        const T fit_y=dataY[jj];
        const T fit_w2=weights[jj]*weights[jj];
        if (statisticsFloatIsOK(fit_x)&&statisticsFloatIsOK(fit_y)&&statisticsFloatIsOK(fit_w2)) {
            sumx=sumx+fit_w2*fit_x;
            sumy=sumy+fit_w2*fit_y;
            sumxy=sumxy+fit_w2*fit_x*fit_y;
            sumx2=sumx2+fit_w2*fit_x*fit_x;
            sumw2=sumw2+fit_w2;
        }
    }
    if (!fixA && !fixB) {
        coeffB=(double(sumxy)*double(sumw2)-double(sumx)*double(sumy))/(double(sumx2)*double(sumw2)-double(sumx)*double(sumx));
        coeffA=(double(sumy)-coeffB*double(sumx))/double(sumw2);
    } else if (fixA && !fixB) {
        coeffB=(double(sumy)-coeffA*double(sumw2))/double(sumx);
    } else if (!fixA && fixB) {
        coeffA=(double(sumy)-coeffB*double(sumx))/double(sumw2);
    } else {
        return false;
    }
    return true;
}

/*! \brief calculate the iteratively reweighted least-squares (IRLS) estimate for the parameters of the model \f$ f(x)=a+b\cdot x \f$
    \ingroup tools_math_stat

    \see C. Sidney Burrus: "Iterative Reweighted Least Squares", <a href="http://cnx.org/content/m45285/latest/">http://cnx.org/content/m45285/latest/</a>

    This is a simple form of the IRLS algorithm to estimate the parameters a and b in a linear model \f$ f(x)=a+b\cdot x \f$.
    This algorithm solves the optimization problem for a \f$ L_p\f$-norm:
      \f[ (a^\ast,b^\ast)=\argmin\limits_{a,b}\sum\limits_i|a+b\cdot x_i-y_i|^p \f]
    by iteratively optimization weights \f$ \vec{w} \f$ and solving a weighted least squares problem in each iteration:
      \f[ (a_n,b_n)=\argmin\limits_{a,b}\sum\limits_i|a+b\cdot x_i-y_i|^{(p-2)}\cdot|a+b\cdot x_i-y_i|^2 \f]


    The algoruithms works as follows:
      - calculate initial \f$ a_0\f$ and \f$ b_0\f$ with unweighted regression from x and y
      - perform a number of iterations (parameter \a iterations ). In each iteration \f$ n\f$:
          - calculate the error vector \f$\vec{e}\f$: \f[ e_i = a+b\cdot x_i -y_i \f]
          - estimate new weights \f$\vec{w}\f$: \[ w_i=|e_i|^{(p-2)/2} \]
          - calculate new estimates \f$ a_n\f$ and \f$ b_n\f$ with weighted regression from \f$ \vec{x}\f$ and \f$ \vec{y}\f$ and \f$ \vec{w}\f$
        .
      - return the last estimates \f$ a_n\f$ and \f$ b_n\f$
    .

    \image html irls.png
    \image latex irls.png
*/
template <class T>
inline bool statisticsIterativelyReweightedLeastSquaresRegression(const T* dataX, const T* dataY, long long N, double& coeffA, double& coeffB, double p=1.1, int iterations=100, bool fixA=false, bool fixB=false) {
    if ((!dataX)||(!dataY)) return false;

    if (N<2) return false;
    T* weights=(T*)statisticsMalloc(N*sizeof(T));
    for (int i=0; i<N; i++) weights[i]=1;
    double alast=coeffA, blast=coeffB;
    statisticsLinearWeightedRegression(dataX, dataY, weights, N, alast, blast, fixA, fixB);
    for (int it=0; it<iterations; it++) {
        // calculate weights
        for (int jj=0; jj<N; jj++) {
            const T fit_x=dataX[jj];
            const T fit_y=dataY[jj];
            const T e=alast+blast*fit_x-fit_y;
            weights[jj]=pow(fabs(e), (p-2.0)/2.0);
        }
        // solve weighted linear least squares
        statisticsLinearWeightedRegression(dataX, dataY, weights, N, alast, blast, fixA, fixB);
    }
    statisticsFree(weights);
    coeffA=alast;
    coeffB=blast;
    return true;
}


class statisticsFunctorLog {
    public:
        inline double operator()(const double& x) const { return log(x); }
};

/*! \brief calculate the power-law regression coefficients for a given dataset where the model is \f$ f(x)=a\cdot x^b \f$
    \ingroup tools_math_stat

    This function computes a linear regression of the log-log pot of the data
*/
template <class T>
inline bool statisticsPowerLawRegression(const T* dataX, const T* dataY, long long N, double& coeffA, double& coeffB, bool fixA=false, bool fixB=false) {
    statisticsFunctorLog logF;
    T* dataLX=statisticsDuplicateAndApply(dataX, N, &logF);
    T* dataLY=statisticsDuplicateAndApply(dataY, N, &logF);

    double a=log(coeffA), b=coeffB;
    bool res=statisticsLinearRegression(dataLX, dataLY, N, a, b, fixA, fixB);

    coeffA=exp(a);
    coeffB=b;

    statisticsFree(dataLX);
    statisticsFree(dataLY);
    return res;
}

/*! \brief calculate the power-law regression coefficients for a given dataset where the model is \f$ f(x)=a\cdot x^b \f$
    \ingroup tools_math_stat

    This function computes a linear regression of the log-log pot of the data
*/
template <class T>
inline bool statisticsPowerLawWeightedRegression(const T* dataX, const T* dataY, const T* weights, long long N, double& coeffA, double& coeffB, bool fixA=false, bool fixB=false) {
    statisticsFunctorLog logF;
    T* dataLX=statisticsDuplicateAndApply(dataX, N, &logF);
    T* dataLY=statisticsDuplicateAndApply(dataY, N, &logF);

    double a=log(coeffA), b=coeffB;
    bool res=statisticsLinearWeightedRegression(dataLX, dataLY, weights, N, a, b, fixA, fixB);

    coeffA=exp(a);
    coeffB=b;

    statisticsFree(dataLX);
    statisticsFree(dataLY);
    return res;
}


/*! \brief calculate the power-law regression coefficients for a given dataset where the model is \f$ f(x)=a\cdot x^b \f$ using IRLS
    \ingroup tools_math_stat

    This function computes a linear regression of the log-log pot of the data
*/
template <class T>
inline bool statisticsPowerLawIRLS(const T* dataX, const T* dataY, long long N, double& coeffA, double& coeffB, double p=1.1, int iterations=100, bool fixA=false, bool fixB=false) {
    statisticsFunctorLog logF;
    T* dataLX=statisticsDuplicateAndApply(dataX, N, &logF);
    T* dataLY=statisticsDuplicateAndApply(dataY, N, &logF);

    double a=log(coeffA), b=coeffB;
    bool res=statisticsIterativelyReweightedLeastSquaresRegression(dataLX, dataLY, N, a, b, p, iterations, fixA, fixB);

    coeffA=exp(a);
    coeffB=b;

    statisticsFree(dataLX);
    statisticsFree(dataLY);
    return res;
}









/*! \brief calculate the power-law regression coefficients for a given dataset where the model is \f$ f(x)=a\cdot \exp(b\cdot x) \f$
    \ingroup tools_math_stat

    This function computes a linear regression of the log-log pot of the data
*/
template <class T>
inline bool statisticsExponentialRegression(const T* dataX, const T* dataY, long long N, double& coeffA, double& coeffB, bool fixA=false, bool fixB=false) {
    statisticsFunctorLog logF;
    T* dataLY=statisticsDuplicateAndApply(dataY, N, &logF);

    double a=log(coeffA), b=coeffB;
    bool res=statisticsLinearRegression(dataX, dataLY, N, a, b, fixA, fixB);

    coeffA=exp(a);
    coeffB=b;

    statisticsFree(dataLY);
    return res;
}

/*! \brief calculate the power-law regression coefficients for a given dataset where the model is \f$ f(x)=a\cdot \exp(b\cdot x) \f$
    \ingroup tools_math_stat

    This function computes a linear regression of the log-log pot of the data
*/
template <class T>
inline bool statisticsExponentialWeightedRegression(const T* dataX, const T* dataY, const T* weights, long long N, double& coeffA, double& coeffB, bool fixA=false, bool fixB=false) {
    statisticsFunctorLog logF;
    T* dataLY=statisticsDuplicateAndApply(dataY, N, &logF);

    double a=log(coeffA), b=coeffB;
    bool res=statisticsLinearWeightedRegression(dataX, dataLY, weights, N, a, b, fixA, fixB);

    coeffA=exp(a);
    coeffB=b;

    statisticsFree(dataLY);
    return res;
}


/*! \brief calculate the power-law regression coefficients for a given dataset where the model is \f$ f(x)=a\cdot \exp(b\cdot x) \f$ using IRLS
    \ingroup tools_math_stat

    This function computes a linear regression of the log-log pot of the data
*/
template <class T>
inline bool statisticsExponentialIRLS(const T* dataX, const T* dataY, long long N, double& coeffA, double& coeffB, double p=1.1, int iterations=100, bool fixA=false, bool fixB=false) {
    statisticsFunctorLog logF;
    T* dataLY=statisticsDuplicateAndApply(dataY, N, &logF);

    double a=log(coeffA), b=coeffB;
    bool res=statisticsIterativelyReweightedLeastSquaresRegression(dataX, dataLY, N, a, b, p, iterations, fixA, fixB);

    coeffA=exp(a);
    coeffB=b;

    statisticsFree(dataLY);
    return res;
}





/*! \brief searches \a x in the \b sorted array \a data between items \a l and \a r
    \ingroup tools_math_stat

    if \a exactFind = \c false , the function returns the nearest element to \a x, if \a x is not exactly found!

    \returns \c -1 if the element was not found!
*/
template <class T>
inline long long statisticsBinarySearchHelp(T x, const T* data, long long l, long long r, bool exactFind=true) {
    //printf("statisticsBinarySearchHelp(%lf, %d...%d: %lf...%lf)\n", (double)x, (int)l, (int)r, (double) data[l], (double) data[r]);
    if (l==r) return l; // 1 element
    if (l+1==r) { // 2 elements
        if (data[r]==x) return r;
        if (data[l]==x) return l;
        if (exactFind) return -1;
        if (data[l]<=x && x<=data[r]) return l;
        return -1;
    }
    if ((x<data[l] || x>data[r])) {
        return -1;
    }
    if (x==data[l]) return l;
    if (x==data[r]) return r;
    int mid=(r+l)/2;
    // search sub-arrays
    long long lr=-1;
    if (x<=data[mid]) lr=statisticsBinarySearchHelp(x, data, l, mid, exactFind);
    else lr=statisticsBinarySearchHelp(x, data, mid, r, exactFind);
    return lr;
}

/*! \brief searches \a x in the \b sorted array \a data of length \a N
    \ingroup tools_math_stat

    if \a exactFind = \c false , the function returns the nearest element to \a x, if \a x is not exactly found!

    \returns \c -1 if the element was not found!
*/
template <class T>
inline long long statisticsBinarySearch(T x, const T* data, long long N, bool exactFind=true) {
    if (N<=0) return -1;
    return statisticsBinarySearchHelp(x, data, 0, N-1, exactFind);
}

/*! \brief performs a linear interpolation on the \b sorted data arrays \a dataX and \a dataY (size \a N) at the position \a x
    \ingroup tools_math_stat

    if \a extendCurve = \c true , the function returns the first/last value in dataY if \a x is outside the boundaries of \a dataX

    \returns \c NAN on error

    \note Thsi function assumes that \a dataX is sorted ascendingly! Use statisticsLinearInterpolate() if you're not sure about this, as the latter will sort the array internally!
*/
template <class T>
inline T statisticsLinearInterpolateSorted(T x, const T* dataX, const T* dataY, long long N, bool extendCurve=false) {
    if ((!dataX)||(!dataY)||N<=0) return NAN;


    if (N==1) {
        if (extendCurve) {
            return dataY[0];
        } else {
            if (x==dataX[0]) return dataY[0];
            return NAN;
        }
    } else {
        if (x<dataX[0]) {
            return (extendCurve)?dataY[0]:NAN;
        } else if (x>dataX[N-1]) {
            return (extendCurve)?dataY[N-1]:NAN;
        } else if (x==dataX[0]) {
            return dataY[0];
        } else if (x==dataX[N-1]) {
            return dataY[N-1];
        } else {
            long long nearestIdx=statisticsBinarySearch(x, dataX, N, false);
            if (nearestIdx<0) return NAN;
            if (x==dataX[nearestIdx]) return  dataY[nearestIdx];
            if (nearestIdx==N-1) {
                if (extendCurve) return dataY[N-1];
                return NAN;
            }
            T x0=dataX[nearestIdx];
            T x1=dataX[nearestIdx+1];
            T y0=dataY[nearestIdx];
            T y1=dataY[nearestIdx+1];
            return y0+(y1-y0)*(x-x0)/(x1-x0);
        }
    }
    return NAN;
}

/*! \brief linear interpolation between the points (x0,y0) and (x1,y1)
    \ingroup tools_math_stat

    if \a extendCurve = \c true , the function returns the first/last value in dataY if \a x is outside the boundaries of \a dataX

*/
template <class T>
inline T statisticsLinearInterpolate(T x, T x0, T y0, T x1, T y1, bool extendCurve=false) {
    if (x<x0) {
        return (extendCurve)?y0:NAN;
    } else if (x>x1) {
        return (extendCurve)?y1:NAN;
    } else if (x==x0) {
        return y0;
    } else if (x==x1) {
        return y1;
    } else {
        return y0+(y1-y0)*(x-x0)/(x1-x0);
    }
    return NAN;
}

/*! \brief performs a linear interpolation on the \b sorted data arrays \a dataX and \a dataY (size \a N) at the position \a x
    \ingroup tools_math_stat

    if \a extendCurve = \c true , the function returns the first/last value in dataY if \a x is outside the boundaries of \a dataX

    \returns \c NAN on error

    \note This function sorts the arrays \a dataX and \a dataY internally! Use statisticsLinearInterpolateSorted() if they are already sorted, as this is faster!
*/
template <class T>
inline T statisticsLinearInterpolate(T x, const T* dataX, const T* dataY, long long N, bool extendCurve=false) {
    if ((!dataX)||(!dataY)||N<=0) return NAN;

    T* dX=statisticsDuplicateArray(dataX, N);
    T* dY=statisticsDuplicateArray(dataY, N);

    statisticsSort2(dX, dY, N);

    T res= statisticsLinearInterpolateSorted(x, dX, dY, N, extendCurve);
    if (dX) statisticsFree(dX);
    if (dY) statisticsFree(dY);
    return res;
}

/*! \brief find root of linearly interpolated data dataX, dataY where dataY==y
    \ingroup tools_math_stat

*/
template <class T>
inline T statisticsFindRootSorted(T y, const T* dataX, const T* dataY, long long N/*, long MaxIterations=1000*/) {
    if ((!dataX)||(!dataY)||N<=0) return NAN;

    if (N==1) {
        if (dataY[0]==y) return dataX[0];
        return NAN;
    }

    long long i=0;
    T x0=dataX[i];
    T y0=dataY[i];
    i++;
    T x1=dataX[i];
    T y1=dataY[i];
    while (i<N) {
        //printf("it=%d:   y=%lf in %lf..%lf\n", (int)i,y, y0, y1);
        if (y==y0) return x0;
        else if (y==y1) return x1;
        else if ((y0<y && y<y1)) {
            return statisticsLinearInterpolate(y, y0,x0,y1,x1);
        } else if ((y1<y && y<y0)) {
            return statisticsLinearInterpolate(y, y1,x1,y0,x0);
        }
        i++;
        y0=y1;
        x0=x1;
        if (i<N) {
            y1=dataY[i];
            x1=dataX[i];
        }
    }

    return NAN;
}

/*! \brief find root of linearly interpolated data dataX, dataY where dataY==y
    \ingroup tools_math_stat

*/
template <class T>
inline T statisticsFindRoot(T y, const T* dataX, const T* dataY, long long N/*, long MaxIterations=1000*/) {
    //This function uses the bisection method: http://en.wikipedia.org/wiki/Bisection_method
    if ((!dataX)||(!dataY)||N<=0) return NAN;

    T* dX=statisticsDuplicateArray(dataX, N);
    T* dY=statisticsDuplicateArray(dataY, N);

    statisticsSort2(dX, dY, N);

    T res=statisticsFindRootSorted(y, dataX, dataY, N/*, MaxIterations*/);

    if (dX) statisticsFree(dX);
    if (dY) statisticsFree(dY);
    return res;
}

/*! \brief (linearly) interpolates the first value of x where y==(ymax+ymin)/2
    \ingroup tools_math_stat

*/
template <class T>
inline T statisticsXatY50Sorted(const T* dataX, const T* dataY, long long N) {
    if ((!dataX)||(!dataY)||N<=0) return NAN;
    T mi, ma;
    statisticsMinMax(dataY,N, mi, ma);

    T mid=(mi+ma)/2.0;

    T res= statisticsFindRootSorted(mid, dataX, dataY, N);
    return res;
}

/*! \brief (linearly) interpolates the first value of x where y==(ymax+ymin)/2
    \ingroup tools_math_stat

*/
template <class T>
inline T statisticsXatY50(const T* dataX, const T* dataY, long long N) {
    if ((!dataX)||(!dataY)||N<=0) return NAN;

    T* dX=statisticsDuplicateArray(dataX, N);
    T* dY=statisticsDuplicateArray(dataY, N);

    statisticsSort2(dX, dY, N);



    T res= statisticsXatY50Sorted(dX,dY,N);
    if (dX) statisticsFree(dX);
    if (dY) statisticsFree(dY);
    return res;
}

/*! \brief calculates a running average over the given data, the returned array is smaller (N-avgSize) than the input. The new size is returned, or the old size on error
    \ingroup tools_math_stat

*/
template <class T>
inline int statisticsRunningAverage(T* data, long long N, int avgSize) {
    if (N>=avgSize && data) {
        int it=0;
        while (it<N-avgSize) {
            T s=0;
            for (int i=it; i<it+avgSize; i++) {
                s+=data[i];
            }
            data[it]=s/T(avgSize);
            it++;
        }
        return N-avgSize;
    }
    return N;
}

/*! \brief calculates a running average over the given data, the returned array is smaller (N-avgSize) than the input. The new size is returned, or the old size on error
    \ingroup tools_math_stat

    if dataX is given, it is also reduced to the new x values.
*/
template <class T>
inline int statisticsRunningAverage(T* dataX, T* data, long long N, int avgSize) {
    if (N>=avgSize && data) {
        int it=0;
        while (it<N-avgSize) {
            T s=0;
            T sX=0;
            for (int i=it; i<it+avgSize; i++) {
                s+=data[i];
                sX+=dataX[i];
            }
            data[it]=s/T(avgSize);
            dataX[it]=sX/T(avgSize);
            it++;
        }
        return N-avgSize;
    }
    return N;
}


/*! \brief find a peak in the \b sorted data
    \ingroup tools_math_stat

*/
template <class T>
inline T statisticsPeakFindSorted(T& peakWidth, const T* dataX, const T* dataY, long long N, T peakInitialWidth=0, T peakInitBaseLine=NAN, T* peakBaseLineOut=NULL, T* peakHeightOut=NULL) {
    if ((!dataX)||(!dataY)||N<=0) return NAN;
    if (N==1) {
        peakWidth=std::numeric_limits<T>::epsilon()*100.0;
        return dataX[0];
    }
    if (N==2) {
        peakWidth=(dataX[1]-dataX[0])/2.0;
        if (dataY[0]>=dataY[1]) {
            return dataX[0];
        } else {
            return dataX[1];
        }
    }
    T pP=NAN;
    T pW=peakInitialWidth;
    if (pW<=0) pW=(dataX[N-1]-dataX[0])/10.0;
    long long peakInitialWidthI=statisticsMin<long long>(2, (dataX[N-1]-dataX[0])/pW);
    T pB=peakInitBaseLine;
    if (!statisticsFloatIsOK(pB)) {
        pB=statisticsMin(dataY, N);//(dataY[0]+dataY[N-1])/2.0;
    }
    T pH=statisticsMax(dataY, N)-pB;;

    //printf("pB=%lf, pW=%lg\n", pB, pW);

    T* dY=statisticsDuplicateArray(dataY, N);
    T* dX=statisticsDuplicateArray(dataX, N);
    long long N1=N;

    statisticsRunningAverage(dX, dY, N, peakInitialWidthI);

    const T* ddY=dY;
    const T* ddX=dX;
    if (N1<3) {
        ddY=dataY;
        ddX=dataX;
        N1=N;
    }

    T* ddXm=NULL;
    T* ddYm=NULL;
    if (N1>15) {
        ddXm=statisticsDuplicateArray(ddX, N1);
        ddYm=statisticsDuplicateArray(ddY, N1);

        long long j=0;
        for (long long i=0; i+2<N1; i+=3) {
            ddXm[j]=(ddX[i]+ddX[i+1]+ddX[i+2])/3.0;
            ddYm[j]=(ddY[i]+ddY[i+1]+ddY[i+2])/3.0;
            j++;
        }
        N1=j;
        ddX=ddXm;
        ddY=ddYm;
    }

    std::vector<std::pair<long long, T> > peaks;
    if (N1>=3){
        for (long long i=0; i<N1-2; i++) {
            if (ddY[i+1]>ddY[i] && ddY[i+1]>ddY[i+2]) {
                peaks.push_back(std::make_pair(i+1, ddY[i+1]));
                //printf("found peak at %d, val: %lf\n", (int)i+1, ddY[i+1]);
            }
        }
    }

    if (peaks.size()==0) {
        pP=(dataX[N-1]+dataX[0])/2.0;
        T sum=0;
        T sum2=0;
        T dsum=0;
        for (long long i=0; i<N; i++) {
            dsum=dsum+dataY[i];
            sum=sum+dataX[i]*dataY[i];
            sum2=sum2+dataY[i]*dataX[i]*dataX[i];
        }
        pP=sum/dsum;
        peakWidth=sqrt(sum2/dsum-sum*sum/dsum/dsum)*2.3548;

    } else {
        size_t imax=peaks[0].first;
        T maxval=peaks[0].second;
        for (size_t i=1; i<peaks.size(); i++) {
            if (peaks[i].second>maxval) {
                maxval=peaks[i].second;
                imax=peaks[i].first;
            }
        }
        pP=ddX[imax];
        pH=statisticsLinearInterpolate(pP, dataX, dataY, N, true);

        //printf("found max. peak %d at %lf (%lf)\n", (int)imax, pP, maxval);

        T mid=(maxval+pB)/2.0;
        peakWidth=fabs(ddX[0]-pP)*2.0;
        for (long long i=imax; i>=0; i--) {
            //printf("i=%d: %lf:  %lf < %lf\n", (int)i, ddX[i], ddY[i], mid);
            if (ddY[i]<=mid) {
                peakWidth=fabs(ddX[i]-pP)*2.0;
                //printf("  => peakWidth=%lg\n", peakWidth);
                break;
            }
        }
    }

    if (dX) statisticsFree(dX);
    if (dY) statisticsFree(dY);
    if (ddXm) statisticsFree(ddXm);
    if (ddYm) statisticsFree(ddYm);
    if (peakBaseLineOut) *peakBaseLineOut=pB;
    if (peakBaseLineOut) *peakHeightOut=pH;
    return pP;
}


/*! \brief find a peak in the data
    \ingroup tools_math_stat

*/
template <class T>
inline T statisticsPeakFind(T& peakWidth, const T* dataX, const T* dataY, long long N, T peakInitialWidth=0, T peakInitBaseLine=NAN, T* peakBaseLineOut=NULL, T* peakHeightOut=NULL) {
    if ((!dataX)||(!dataY)||N<=0) return NAN;

    T* dX=statisticsDuplicateArray(dataX, N);
    T* dY=statisticsDuplicateArray(dataY, N);

    statisticsSort2(dX, dY, N);

    T res= statisticsPeakFindSorted(peakWidth,dX,dY,N,peakInitialWidth,peakInitBaseLine, peakBaseLineOut, peakHeightOut);

    if (dX) statisticsFree(dX);
    if (dY) statisticsFree(dY);
    return res;
}

/*! \brief find the minimum distance between any two datapoints in dataX
    \ingroup tools_math_stat

*/
template <class T>
inline T statisticsMinDistance(const T* dataX, long long N, T* minDistNonZero=NULL) {
    if ((!dataX)||N<=1) return 0;

    T res=fabs(dataX[1]-dataX[0]);
    T resg0=res;
    for (int i=1; i<N; i++) {
        T d=fabs(dataX[i]-dataX[i-1]);
        if (d<res) res=d;
        if (resg0<=0 || (d>T(0) && d<resg0)) resg0=d;
    }
    if (minDistNonZero) *minDistNonZero=resg0;
    return res;
}

/*! \brief find the maximum distance between any two datapoints in dataX
    \ingroup tools_math_stat

*/
template <class T>
inline T statisticsMaxDistance(const T* dataX, long long N, T* maxDistNonZero=NULL) {
    if ((!dataX)||N<=1) return 0;

    T res=0;
    T resg0=0;
    for (int i=1; i<N; i++) {
        T d=fabs(dataX[i]-dataX[i-1]);
        if (d>res) res=d;
        if (resg0<=0 || (d>T(0) && d>resg0)) resg0=d;
    }
    if (maxDistNonZero) *maxDistNonZero=resg0;
    return res;
}



/*! \brief find two peaks in the \b sorted data
    \ingroup tools_math_stat

*/
template <class T>
inline bool statistics2PeakFindSorted(T& peakPos, T& peakWidth, T& peakPos2, T& peakWidth2, const T* dataX, const T* dataY, long long N, T peakInitialWidth=0, T peakInitBaseLine=NAN, T* peakBaseLineOut=NULL, T* peakHeightOut=NULL, T* peakHeightOut2=NULL) {
    if ((!dataX)||(!dataY)||N<=0) return NAN;

    T* dX=statisticsDuplicateArray(dataX, N);
    T* dY=statisticsDuplicateArray(dataY, N);

    T pH=0;
    //printArray(dX, N, "    in x = ");
    //printArray(dY, N, "    in y = ");
    peakPos=statisticsPeakFindSorted(peakWidth, dX, dY, N, peakInitialWidth, peakInitBaseLine, peakBaseLineOut, &pH);
    if (peakHeightOut) *peakHeightOut=pH;
    if (statisticsFloatIsOK(peakPos)) {
        T sigma=peakWidth/2.3548;
        for (long long i=0; i<N; i++) {
            dY[i]=dY[i]-pH*exp(-0.5*(dX[i]-peakPos)*(dX[i]-peakPos)/sigma/sigma);
        }
        //printArray(dY, N, "   cor y = ");

        //printf("found single-peak @ %lf,   width=%lf,   height=%lf\n", peakPos, peakWidth, pH);

        peakPos2=statisticsPeakFindSorted(peakWidth2, dX, dY, N, peakInitialWidth, peakInitBaseLine, peakBaseLineOut, peakHeightOut2);
    }

    if (dX) statisticsFree(dX);
    if (dY) statisticsFree(dY);

    return statisticsFloatIsOK(peakPos);
}



/*! \brief find three peaks in the \b sorted data
    \ingroup tools_math_stat

*/
template <class T>
inline bool statistics3PeakFindSorted(T& peakPos, T& peakWidth, T& peakPos2, T& peakWidth2, T& peakPos3, T& peakWidth3, const T* dataX, const T* dataY, long long N, T peakInitialWidth=0, T peakInitBaseLine=NAN, T* peakBaseLineOut=NULL, T* peakHeightOut=NULL, T* peakHeightOut2=NULL, T* peakHeightOut3=NULL) {
    if ((!dataX)||(!dataY)||N<=0) return NAN;

    T* dX=statisticsDuplicateArray(dataX, N);
    T* dY=statisticsDuplicateArray(dataY, N);

    T pH=0;
    T pH2=0;
    //printArray(dX, N, "    in x = ");
    //printArray(dY, N, "    in y = ");
    peakPos=statisticsPeakFindSorted(peakWidth, dX, dY, N, peakInitialWidth, peakInitBaseLine, peakBaseLineOut, &pH);
    if (peakHeightOut) *peakHeightOut=pH;
    if (statisticsFloatIsOK(peakPos)) {
        T sigma=peakWidth/2.3548;
        for (long long i=0; i<N; i++) {
            dY[i]=dY[i]-pH*exp(-0.5*(dX[i]-peakPos)*(dX[i]-peakPos)/sigma/sigma);
        }
        //printArray(dY, N, "   cor y = ");

        //printf("found single-peak @ %lf,   width=%lf,   height=%lf\n", peakPos, peakWidth, pH);

        peakPos2=statisticsPeakFindSorted(peakWidth2, dX, dY, N, peakInitialWidth, peakInitBaseLine, peakBaseLineOut, &pH);
        if (peakHeightOut2) *peakHeightOut2=pH2;
        sigma=peakWidth2/2.3548;
        for (long long i=0; i<N; i++) {
            dY[i]=dY[i]-pH2*exp(-0.5*(dX[i]-peakPos2)*(dX[i]-peakPos2)/sigma/sigma);
        }
        peakPos3=statisticsPeakFindSorted(peakWidth3, dX, dY, N, peakInitialWidth, peakInitBaseLine, peakBaseLineOut, peakHeightOut3);
    }

    if (dX) statisticsFree(dX);
    if (dY) statisticsFree(dY);

    return statisticsFloatIsOK(peakPos);
}




/*! \brief find two peaks in the data
    \ingroup tools_math_stat

*/
template <class T>
inline bool statistics2PeakFind(T& peakPos, T& peakWidth, T& peakPos2, T& peakWidth2, const T* dataX, const T* dataY, long long N, T peakInitialWidth=0, T peakInitBaseLine=NAN, T* peakBaseLineOut=NULL, T* peakHeightOut=NULL, T* peakHeightOut2=NULL) {
    if ((!dataX)||(!dataY)||N<=0) return false;

    T* dX=statisticsDuplicateArray(dataX, N);
    T* dY=statisticsDuplicateArray(dataY, N);

    statisticsSort2(dX, dY, N);

    bool res= statistics2PeakFindSorted(peakPos, peakWidth, peakPos2, peakWidth2, dataX, dataY, N,  peakInitialWidth,  peakInitBaseLine, peakBaseLineOut,  peakHeightOut,  peakHeightOut2);

    if (dX) statisticsFree(dX);
    if (dY) statisticsFree(dY);
    return res;
}


/*! \brief find three peaks in the data
    \ingroup tools_math_stat

*/
template <class T>
inline bool statistics3PeakFind(T& peakPos, T& peakWidth, T& peakPos2, T& peakWidth2, T& peakPos3, T& peakWidth3, const T* dataX, const T* dataY, long long N, T peakInitialWidth=0, T peakInitBaseLine=NAN, T* peakBaseLineOut=NULL, T* peakHeightOut=NULL, T* peakHeightOut2=NULL, T* peakHeightOut3=NULL) {
    if ((!dataX)||(!dataY)||N<=0) return false;

    T* dX=statisticsDuplicateArray(dataX, N);
    T* dY=statisticsDuplicateArray(dataY, N);

    statisticsSort2(dX, dY, N);

    bool res= statistics3PeakFindSorted(peakPos, peakWidth, peakPos2, peakWidth2, peakPos3, peakWidth3, dataX, dataY, N,  peakInitialWidth,  peakInitBaseLine, peakBaseLineOut,  peakHeightOut,  peakHeightOut2,  peakHeightOut3);

    if (dX) statisticsFree(dX);
    if (dY) statisticsFree(dY);
    return res;
}




/*! \brief apply a given function/functor to every element of the data arrays \a data
    \ingroup tools_math_stat

*/
template <class T, typename TFunctor>
inline void statisticsApply(T* data, long long N, TFunctor* f) {
    if (data && N>0) {
        for (long long i=0; i<N; i++) {
            data[i]=(*f)(data[i]);
        }
    }
}


/*! \brief copy the given array and apply a given function/functor to every element of the copy of the data arrays \a data
    \ingroup tools_math_stat

*/
template <class T, typename TFunctor>
inline T* statisticsDuplicateAndApply(const T* data, long long N, TFunctor* f) {
    if (data && N>0) {
        T* c=statisticsDuplicateArray(data, N);
        statisticsApply(c, N, f);
        return c;
    }
    return NULL;
}


/*! \brief this class ensures that the given pointer is statisticsFreed when the class is destroyed.
    \ingroup tools_math_stat

*/
template<typename T>
class StatisticsScopedPointer {
    public:
        inline explicit StatisticsScopedPointer(T* pnt) {
            pntr=pnt;
        }
        inline explicit StatisticsScopedPointer() {
            pntr=NULL;
        }
        inline explicit StatisticsScopedPointer(const StatisticsScopedPointer& other) {
            pntr=other.pntr;
        }


        ~StatisticsScopedPointer() {
            if (pntr) statisticsFree(pntr);
        }


        inline  T& operator=(T* p) {
             pntr=p;
             return *this;
         }

        inline  T& operator=(const StatisticsScopedPointer<T>& p) {
             pntr=p.pntr;
             return *this;
         }

        inline T& operator*() const { return *pntr; }
        inline T* operator->() const { return pntr; }
        inline T* data() const { return pntr; }
        inline T& operator[](long long i) {
            return pntr[i];
        }
        inline const T& operator[](long long i) const {
            return pntr[i];
        }



    private:
        T* pntr;
};


/*! \brief lookup the given value \a data in the given \a lut, which images the range \a lmin ... \lmax to \a lut_size values of type TLUT.
    \ingroup tools_math_stat

    If the given data is out of range, then the nearest lut bound is returned.
*/
template <class T, class TLUT>
inline TLUT statisticsLookupLUT(const T& data, const T& lmin, const T& lmax, const TLUT* lut, int lut_size) {
    if (!lut || lut_size<=0) return 0;
    int idx=(data-lmin)*T(lut_size-1)/(lmax-lmin);
    if (idx<0) return lut[0];
    else if (idx>=lut_size) return lut[lut_size-1];
    return lut[idx];
}

#define linalgMatIndex(l,c,C) ((l)*(C)+(c))


/*! \brief print the given LxC matrix to std::cout
    \ingroup tools_math_linalg

*/
template <class T>
inline void linalgPrintMatrix(T* matrix, long L, long C, int width=9, int precision=3, char mode='f') {
    for (long l=0; l<L; l++) {
        for (long c=0; c<C; c++) {
            if (c>0) std::cout<<", ";
            std::cout.precision(precision);
            std::cout.width(width);
            if (mode=='f') std::cout<<std::fixed<<std::right<<matrix[linalgMatIndex(l,c,C)];
            else std::cout<<std::scientific<<std::right<<matrix[linalgMatIndex(l,c,C)];
        }
        std::cout<<std::endl;
    }
}



/*! \brief convert the given LxC matrix to std::string
    \ingroup tools_math_linalg

*/
template <class T>
inline std::string linalgMatrixToString(T* matrix, long L, long C, int width=9, int precision=3, const std::string& mode=std::string("g")) {
    std::string format="%"+linalgIntToStr(width)+std::string(".")+linalgIntToStr(precision)+std::string("l")+mode;
    std::ostringstream ost;
    for (long l=0; l<L; l++) {
        for (long c=0; c<C; c++) {
            if (c>0) ost<<", ";
            char buf[500];
            sprintf(buf, format.c_str(), double(matrix[linalgMatIndex(l,c,C)]));
            ost<<buf;
            /*ost.precision(precision);
            ost.width(width);
            if (mode=='f') ost<<std::fixed<<std::right<<matrix[linalgMatIndex(l,c,C)];
            else ost<<std::scientific<<std::right<<matrix[linalgMatIndex(l,c,C)];*/

        }
        ost<<std::endl;
    }
    return ost.str();
}

/*! \brief maps the number range -1 ... +1 to a color-scale lightblue - white - lightred (used for coloring matrices!)
    \ingroup tools_math_linalg

*/
inline void linalgPM1ToRWBColors(double val, uint8_t& r, uint8_t& g, uint8_t& b){
    r=255;
    g=255;
    b=255;
    const double fval=fabs(val);
    if (val<0 && val>=-1) {
        r=round(255.0-fval*127.0);
        g=round(255.0-fval*127.0);
    } else if (val>0 && val<=1) {
        b=round(255.0-fval*127.0);
        g=round(255.0-fval*127.0);
    } else if (val<-1) {
        r=127;
        g=127;
        b=255;
    } else if (val>1) {
        r=255;
        g=127;
        b=127;
    }
}

/*! \brief convert the given LxC matrix to std::string, encoded as HTML table
    \ingroup tools_math_linalg

*/
template <class T>
inline std::string linalgMatrixToHTMLString(T* matrix, long L, long C, int width=9, int precision=3, const std::string& mode=std::string("g"), const std::string& tableformat=std::string(), const std::string& prenumber=std::string(), const std::string& postnumber=std::string(), bool colorcoding=false, bool zeroIsWhite=true, std::string* colorlabel=NULL) {
    std::ostringstream ost;
    ost<<"<table "<<tableformat<<">\n";
    std::string format="%"+linalgIntToStr(width)+std::string(".")+linalgIntToStr(precision)+std::string("l")+mode;
    double minv=0, maxv=0;
    if (colorcoding) {
        statisticsMinMax(matrix, L*C, minv, maxv);
    }
    for (long l=0; l<L; l++) {
        ost<<"  <tr>";
        for (long c=0; c<C; c++) {
            const double val=matrix[linalgMatIndex(l,c,C)];
            std::string cols="";
            if (colorcoding) {
                double valrel=0;
                uint8_t r=255,g=255,b=255;
                if (zeroIsWhite){
                    if (val<0) valrel=-1.0*fabs(val/minv);
                    if (val>0) valrel=fabs(val/maxv);
                } else {
                    valrel=((val-minv)/(maxv-minv)-0.5)*2.0;
                }
                linalgPM1ToRWBColors(valrel, r,g,b);
                char buf[500];
                sprintf(buf, " bgcolor=\"#%02X%02X%02X\"", int(r), int(g), int(b));
                cols=buf;
            }
            ost<<"<td align=\"center\" valign=\"middle\" width=\""<<(100.0/double(C))<<"%\" "<<cols<<"><nobr>";
            ost.precision(precision);
            ost.width(width);
            char buf[500];
            sprintf(buf, format.c_str(), val);
            ost<<prenumber<<buf<<postnumber;
            ost<<"</nobr></td>";
        }
        ost<<"</tr>\n";
    }
    ost<<"</table>";

    if (colorcoding && colorlabel) {
        char buf[8192];
        uint8_t rm=255,gm=255,bm=255;
        uint8_t rmc=255,gmc=255,bmc=255;
        uint8_t rc=255,gc=255,bc=255;
        uint8_t rcp=255,gcp=255,bcp=255;
        uint8_t rp=255,gp=255,bp=255;
        double vm=minv;
        double vc=0;
        double vp=maxv;
        if (!zeroIsWhite) {
            vc=(maxv+minv)/2.0;
        }
        linalgPM1ToRWBColors(-1, rm, gm, bm);
        linalgPM1ToRWBColors(-0.5, rmc, gmc, bmc);
        linalgPM1ToRWBColors(0, rc, gc, bc);
        linalgPM1ToRWBColors(0.5, rcp, gcp, bcp);
        linalgPM1ToRWBColors(1, rp, gp, bp);
        sprintf(buf, "<table cellpadding=\"2\" cellspacing=\"0\" border=\"1\"><tr><td><table cellpadding=\"3\" cellspacing=\"0\" border=\"0\"><tr>"
                     "<td bgcolor=\"#%02X%02X%02X\" width=\"20%%\"><nobr>%9.3lg</nobr></td>"
                     "<td bgcolor=\"#%02X%02X%02X\" width=\"20%%\"><nobr>&nbsp;&nbsp;&nbsp;</nobr></td>"
                     "<td bgcolor=\"#%02X%02X%02X\" width=\"20%%\"><nobr>%9.3lg</nobr></td>"
                     "<td bgcolor=\"#%02X%02X%02X\" width=\"20%%\"><nobr>&nbsp;&nbsp;&nbsp;</nobr></td>"
                     "<td bgcolor=\"#%02X%02X%02X\" width=\"20%%\"><nobr>%9.3lg</nobr></td>"
                     "</tr></table></td></tr></table>", int(rm), int(gm), int(bm), vm, int(rmc), int(gmc), int(bmc), int(rc), int(gc), int(bc), vc, int(rcp), int(gcp), int(bcp), int(rp), int(gp), int(bp), vp);
        (*colorlabel)=std::string(buf);
    }
    return ost.str();
}

/*! \brief dot-product
    \ingroup tools_math_linalg

*/
template <class T>
inline T linalgDotProduct(const T* vec1, const T* vec2, long N) {
    register T res=0;
    for (long l=0; l<N; l++) {
        res=res+vec1[l]*vec2[l];
    }
    return res;
}


/*! \brief transpose the given NxN matrix
    \ingroup tools_math_linalg

*/
template <class T>
inline void linalgTransposeMatrix(T* matrix, long N) {
    for (long l=0; l<N; l++) {
        for (long c=l+1; c<N; c++) {
            statisticsSwap(matrix, linalgMatIndex(l,c,N), linalgMatIndex(c,l,N));
        }
    }
}


/*! \brief transpose the given LxC matrix
    \ingroup tools_math_linalg

    The output is interpreted as CxL matrix!!!
*/
template <class T>
inline void linalgTransposeMatrix(T* matrix, long L, long C) {
    T* t=statisticsDuplicateArray<T>(matrix, L*C);
    for (long l=0; l<L; l++) {
        for (long c=0; c<C; c++) {
            matrix[linalgMatIndex(c,l,L)]=t[linalgMatIndex(l,c,C)];
        }
    }
    statisticsFree(t);
}

/*! \brief swap two lines in a matrix
    \ingroup tools_math_linalg

*/
template <class T>
inline void linalgMatrixSwapLines(T* m, long l1, long l2, long C) {
    for (long c=0; c<C; c++) {
        statisticsSwap(m, linalgMatIndex(l1, c, C), linalgMatIndex(l2, c, C));
    }
}
/*! \brief swap two lines in a matrix
    \ingroup tools_math_linalg

*/
template <class T>
inline void linalgMatrixSwapLinesV(std::vector<T>& m, long l1, long l2, long C) {
    for (long c=0; c<C; c++) {
        statisticsSwapV(m, linalgMatIndex(l1, c, C), linalgMatIndex(l2, c, C));
    }
}

/*! \brief matrix-matrix product
    \ingroup tools_math_linalg

    \param M1 matrix 1, size: L1xC1
    \param M2 matrix 1, size: L2xC2
    \param[out] M output matrix M=M1*M2, size: L1xC2
*/
template <class T>
inline void linalgMatrixProduct(const T* M1, long L1, long C1, const T* M2, long L2, long C2, T* M) {
    if (M1!=M &&M2!=M) {
//#ifdef STATISTICS_TOOLS_MAY_USE_EIGEN3
//        if (L1==2 && C1==2 && C2==2 && L2==2) {
//            Eigen::Map<const Eigen::Matrix<T,2,2,Eigen::RowMajor> > eM1(M1);
//            Eigen::Map<const Eigen::Matrix<T,2,2,Eigen::RowMajor> > eM2(M2);
//            Eigen::Map<Eigen::Matrix<T,2,2,Eigen::RowMajor> > eM(M);
//            eM=eM1*eM2;
//        } else if (L1==3 && C1==3 && C2==3 && L2==3) {
//            Eigen::Map<const Eigen::Matrix<T,3,3,Eigen::RowMajor> > eM1(M1);
//            Eigen::Map<const Eigen::Matrix<T,3,3,Eigen::RowMajor> > eM2(M2);
//            Eigen::Map<Eigen::Matrix<T,3,3,Eigen::RowMajor> > eM(M);
//            eM=eM1*eM2;
//        } else {
//            Eigen::Map<const Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> > eM1(M1,L1,C1);
//            Eigen::Map<const Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> > eM2(M2,L2,C2);
//            Eigen::Map<Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> > eM(M,L1,C2);
//            eM=eM1*eM2;
//        }
//#else
        for (long l=0; l<L1; l++) {
            for (long c=0; c<C2; c++) {
                register double s=T(0);
                for (long i=0; i<C1; i++) {
                    s = s + M1[linalgMatIndex(l, i, C1)]*M2[linalgMatIndex(i,c, C2)];
                }
                M[linalgMatIndex(l, c, C2)]=s;
            }
        }
//#endif
    } else if (M1==M && M2!=M) {
        T* MM=statisticsDuplicateArray(M1, L1*C1);
        linalgMatrixProduct(MM,L1,C1,M2,L2,C2,M);
        statisticsFree(MM);
    } else if (M1!=M && M2==M) {
        T* MM=statisticsDuplicateArray(M2, L1*C1);
        linalgMatrixProduct(M1,L1,C1,MM,L2,C2,M);
        statisticsFree(MM);
    } else if (M1==M && M2==M) {
        T* MM=statisticsDuplicateArray(M1, L1*C1);
        linalgMatrixProduct(MM,L1,C1,MM,L2,C2,M);
        statisticsFree(MM);
    }
}


/*! \brief matrix-matrix product of two NxN matrices
    \ingroup tools_math_linalg

    \param M1 matrix 1, size: NxN
    \param M2 matrix 1, size: NxN
    \param[out] M output matrix M=M1*M2, size: NxN
*/
template <class T>
inline void linalgMatrixProduct(const T* M1, const T* M2, long N, T* M) {
    linalgMatrixProduct(M1,N,N,M2,N,N,M);
}



///*! \brief LU decomposition of an NxN matrix
//    \ingroup tools_math_linalg

//    \param M matrix, size: NxN
//    \param N size of the matrix (number of rows/columns)
//    \param[out] L L matrix, size: NxN
//    \param[out] U U matrix, size: NxN
//    \param[out] Pinv P permuntation matrix (optional!)
//    \param[out] Qinv Q permutation matrix (optional!)

//    The full decomposition is \f[ \mat{M}=\mat{P}^{-1}\mat{L}\mat{U}\mat{Q}^{-1} \f]

//    \note if not using EIGEN library, this function does not work for some matrices!!!
//*/
//template <class T>
//inline void linalgMatrixLUDecomposition(const T* M, long N, T* L, T* U, T* Pinv=NULL, T* Qinv=NULL) {
//#ifdef STATISTICS_TOOLS_MAY_USE_EIGEN3
//    Eigen::Map<const Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> >  eM(M, N,N);
//    Eigen::Map<Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> >  eL(L, N,N);
//    Eigen::Map<Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> >  eU(U, N,N);


//    Eigen::FullPivLU<Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> > LU(eM);

//    eL=eL.setIdentity(N,N);
//    eL.template triangularView<Eigen::StrictlyLower>() = LU.matrixLU();
//    eU=LU.matrixLU().template triangularView<Eigen::Upper>();




//    if (Pinv) {
//        Eigen::Map<Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> > Pi(Pinv, N,N);
//        Pi=LU.permutationP().inverse();
//    }
//    if (Qinv) {
//        Eigen::Map<Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> > Qi(Qinv, N,N);
//        Qi=LU.permutationQ().inverse();
//    }
//#else
//    memset(L, 0, N*N*sizeof(T));
//    memset(U, 0, N*N*sizeof(T));
//    if (Pinv) {
//        memset(Pinv, 0, N*N*sizeof(T));
//        for (register long i=0; i<N; i++) {
//            L[linalgMatIndex(i, i, N)]=1;
//        }
//    }
//    if (Qinv) {
//        memset(Qinv, 0, N*N*sizeof(T));
//        for (register long i=0; i<N; i++) {
//            L[linalgMatIndex(i, i, N)]=1;
//        }
//    }
//    for (register long i=0; i<N; i++) {
//        L[linalgMatIndex(i, i, N)]=1;
//    }
//    for (register long j=0; j<N; j++) {
//        for (register long i=0; i<=j; i++) {
//            register T s=0;
//            for (register long k=0; k<=i-1; k++) {
//                s+=(L[linalgMatIndex(i, k, N)]*U[linalgMatIndex(k,j, N)]);
//            }
//            U[linalgMatIndex(i, j, N)]=M[linalgMatIndex(i, j, N)]-s;
//        }
//        for (register long i=j+1; i<N; i++) {
//            register T s=0;
//            for (register long k=0; k<=j-1; k++) {
//                s+=(L[linalgMatIndex(i, k, N)]*U[linalgMatIndex(k,j, N)]);
//            }
//            L[linalgMatIndex(i, j, N)]=(M[linalgMatIndex(i, j, N)]-s)/U[linalgMatIndex(j, j, N)];
//        }
//    }
//#endif
//}



/*! \brief performs a Gauss-Jordan eliminaion on a LxC matrix m
    \ingroup tools_math_linalg

    For a matrix equation \f[ A\cdot\vec{x}=\vec{b} \f] the input matrix is \f[ \left[A | \vec{b}\right] \f] for matrix inversion it is
    \f[ \left[A | I_L\right] \f] where \f$ I_L \f$ is the unit matrix with LxL entries.

    \see http://www.virtual-maxim.de/matrix-invertieren-in-c-plus-plus/
*/
template <class T>
inline bool linalgGaussJordan(T* m, long L, long C) {

    const long N=L;

    //std::cout<<"\nstep 0:\n";
    //statisticsPrintMatrix(m, N, C);

    // first we perform a Gauss-elimination, which transforms the  matrix in the left half of m into upper triangular form
    for (long k=0; k<N-1; k++) {
        //std::cout<<"\nstep G"<<k<<": pivot="<<m[statisticsMatIndex(k,k,C)]<<"\n";
        if (m[linalgMatIndex(k,k,C)]==0.0) {
            // if pivot(m[k,k])==0, then swap this line with a line, which does not have a 0 in the k-th column
            for (long ks=k+1; ks<N; ks++) {
                if (m[linalgMatIndex(ks,k,C)]!=0.0) {
                    linalgMatrixSwapLines(m, ks, k, C);
                    break;
                } else if (ks==N-1) {
                    // if no such element is found, the matrix may not be inverted!
                    return false;
                }
            }
        }

        // now we can eliminate all entries i below the pivot line p, by subtracting
        // the pivot line, scaled by s, from every line, where
        // s=m[i,p]/m[p,p]
        for (long i=k+1; i<N; i++) {
            const T s=m[linalgMatIndex(i,k,C)]/m[linalgMatIndex(k,k,C)];
            for (long c=k; c<C; c++) {
                m[linalgMatIndex(i,c,C)] -= m[linalgMatIndex(k,c,C)]*s;
            }
        }

        //statisticsPrintMatrix(m, N, C);
    }

    // now we can caluate the determinant of the left half-matrix, which can be used to determine, whether matrix
    // is invertible at all: if det(T)==0.0 -> matrix is not invertible
    // the determinant of an upper triangular marix equals the product of the diagonal elements
    T det=1.0;
    for (long k=0; k<N; k++) {
        det = det * m[linalgMatIndex(k,k,C)];
    }
    //statisticsPrintMatrix(m, N, C);
    //std::cout<<"\nstep 2:  det(M)="<<det<<"\n";
    if (fabs(det)<DBL_MIN*10.0) return false;


    // if the matrix may be inverted, we can go on with the JOrdan part of the algorithm:
    // we work the Nx(2N) matrix from bottom to top and transform the left side into a unit matrix
    //   - the last row is left unchanged
    //   - the last row is subtracted from every row i above, scaled by m[i,N]/m[N,N]
    // then we repeat this for the (N-1)*(N-1) left upper matrix, which has again full triangular form
    for (long k=N-1; k>0; k--) {
        //std::cout<<"\nstep J"<<k<<":\n";
        for (long i=k-1; i>=0; i--) {
            const T s=m[linalgMatIndex(i,k,C)]/m[linalgMatIndex(k,k,C)];
            for (long c=k; c<C; c++) {
                m[linalgMatIndex(i,c,C)] -= m[linalgMatIndex(k,c,C)]*s;
            }
        }
        //statisticsPrintMatrix(m, N, C);
    }
    // finally each line is normalized to 1 by dividing by the diagonal entry in the left NxN matrix
    // and copy the result to matrix_out
    for (long k=0; k<N; k++) {

        const T s=m[linalgMatIndex(k,k,C)];
        for (long c=N; c<C; c++) {
            m[linalgMatIndex(k,c,C)] = m[linalgMatIndex(k,c,C)] / s;
        }
        m[linalgMatIndex(k,k,C)]=T(1);
    }


    return true;
}




/*! \brief performs a Gauss-Jordan eliminaion on a LxC matrix m
    \ingroup tools_math_linalg

    For a matrix equation \f[ A\cdot\vec{x}=\vec{b} \f] the input matrix is \f[ \left[A | \vec{b}\right] \f] for matrix inversion it is
    \f[ \left[A | I_L\right] \f] where \f$ I_L \f$ is the unit matrix with LxL entries.

    \see http://www.virtual-maxim.de/matrix-invertieren-in-c-plus-plus/
*/
template <class T>
inline bool linalgGaussJordanV(std::vector<T>& m, long L, long C) {

    const long N=L;

    //std::cout<<"\nstep 0:\n";
    //statisticsPrintMatrix(m, N, C);

    // first we perform a Gauss-elimination, which transforms the  matrix in the left half of m into upper triangular form
    for (long k=0; k<N-1; k++) {
        //std::cout<<"\nstep G"<<k<<": pivot="<<m[statisticsMatIndex(k,k,C)]<<"\n";
        if (m[linalgMatIndex(k,k,C)]==0.0) {
            // if pivot(m[k,k])==0, then swap this line with a line, which does not have a 0 in the k-th column
            for (long ks=k+1; ks<N; ks++) {
                if (m[linalgMatIndex(ks,k,C)]!=0.0) {
                    linalgMatrixSwapLinesV(m, ks, k, C);
                    break;
                } else if (ks==N-1) {
                    // if no such element is found, the matrix may not be inverted!
                    return false;
                }
            }
        }

        // now we can eliminate all entries i below the pivot line p, by subtracting
        // the pivot line, scaled by s, from every line, where
        // s=m[i,p]/m[p,p]
        for (long i=k+1; i<N; i++) {
            const T s=m[linalgMatIndex(i,k,C)]/m[linalgMatIndex(k,k,C)];
            for (long c=k; c<C; c++) {
                m[linalgMatIndex(i,c,C)] -= m[linalgMatIndex(k,c,C)]*s;
            }
        }

        //statisticsPrintMatrix(m, N, C);
    }

    // now we can caluate the determinant of the left half-matrix, which can be used to determine, whether matrix
    // is invertible at all: if det(T)==0.0 -> matrix is not invertible
    // the determinant of an upper triangular marix equals the product of the diagonal elements
    T det=1.0;
    for (long k=0; k<N; k++) {
        det = det * m[linalgMatIndex(k,k,C)];
    }
    //statisticsPrintMatrix(m, N, C);
    //std::cout<<"\nstep 2:  det(M)="<<det<<"\n";
    if (fabs(det)<DBL_MIN*10.0) return false;


    // if the matrix may be inverted, we can go on with the JOrdan part of the algorithm:
    // we work the Nx(2N) matrix from bottom to top and transform the left side into a unit matrix
    //   - the last row is left unchanged
    //   - the last row is subtracted from every row i above, scaled by m[i,N]/m[N,N]
    // then we repeat this for the (N-1)*(N-1) left upper matrix, which has again full triangular form
    for (long k=N-1; k>0; k--) {
        //std::cout<<"\nstep J"<<k<<":\n";
        for (long i=k-1; i>=0; i--) {
            const T s=m[linalgMatIndex(i,k,C)]/m[linalgMatIndex(k,k,C)];
            for (long c=k; c<C; c++) {
                m[linalgMatIndex(i,c,C)] -= m[linalgMatIndex(k,c,C)]*s;
            }
        }
        //statisticsPrintMatrix(m, N, C);
    }
    // finally each line is normalized to 1 by dividing by the diagonal entry in the left NxN matrix
    // and copy the result to matrix_out
    for (long k=0; k<N; k++) {

        const T s=m[linalgMatIndex(k,k,C)];
        for (long c=N; c<C; c++) {
            m[linalgMatIndex(k,c,C)] = m[linalgMatIndex(k,c,C)] / s;
        }
        m[linalgMatIndex(k,k,C)]=T(1);
    }

    return true;
}




/*! \brief invert the given NxN matrix using Gauss-Jordan elimination
    \ingroup tools_math_linalg

    \return \c true on success and the inverted matrix in matrix_out.

    \note It is save to call \c statisticsMatrixInversion(A,A,N) with the same argument for in and out matrix. Then the input will be overwritten with the new matrix!
    \note matrix and matrix_out have to be of size N*N. Matrices are interpreted as row-major!

    \see http://www.virtual-maxim.de/matrix-invertieren-in-c-plus-plus/
*/
template <class T>
inline bool linalgMatrixInversion(const T* matrix, T* matrix_out, long N) {
#ifdef STATISTICS_TOOLS_MAY_USE_EIGEN3
    if (N==2) {
        Eigen::Map<const Eigen::Matrix<T,2,2,Eigen::RowMajor> > eA(matrix);
        Eigen::Map<Eigen::Matrix<T,2,2,Eigen::RowMajor> > eO(matrix_out);
        eO=eA.inverse();
        //std::cout<<"\n--------------------------------------\n2x2 input matrix\n"<<eA<<"\n--------------------------------------\n";
        return eO.allFinite();
    } else if (N==3) {
        Eigen::Map<const Eigen::Matrix<T,3,3,Eigen::RowMajor> > eA(matrix);
        Eigen::Map<Eigen::Matrix<T,3,3,Eigen::RowMajor> > eO(matrix_out);
        //std::cout<<"\n--------------------------------------\n3x3 input matrix\n"<<eA<<"\n--------------------------------------\n";
        eO=eA.inverse();
        return eO.allFinite();
    } else if (N==4) {
        Eigen::Map<const Eigen::Matrix<T,4,4,Eigen::RowMajor> > eA(matrix);
        Eigen::Map<Eigen::Matrix<T,4,4,Eigen::RowMajor> > eO(matrix_out);
        //std::cout<<"\n--------------------------------------\n4x4 input matrix\n"<<eA<<"\n--------------------------------------\n";
        eO=eA.inverse();
        return eO.allFinite();
    } else {
        Eigen::Map<const Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> > eA(matrix,N,N);
        Eigen::Map<Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> > eO(matrix_out,N,N);
        eO=eA.inverse();
        //std::cout<<"\n--------------------------------------\nNxN input matrix\n"<<eA<<"\n--------------------------------------\n";
        return eO.allFinite();
    }
    return true;
#else
    // first build a N*(2N) matrix of the form
    //
    //    m11 m12 ...  | 1 0 0
    //    m21 m22 ...  | 0 1 0
    //    ... ... ...  | .....
    //
    const long msize=N*N*2;
    std::vector<T> m;
    m.resize(msize);
    //StatisticsScopedPointer<T> m((T*)statisticsMalloc(msize*sizeof(T))); // use scoped pointer to ensure, that m is free'd, when the function is ending
    for (long i=0; i<msize; i++) m[i]=T(0); // init with 0
    for (long l=0; l<N; l++) {
        for (long c=0; c<N; c++) { // init left half with matrix
            m[linalgMatIndex(l,c,2*N)]=matrix[linalgMatIndex(l,c,N)];
        }
        // init right half with unit matrix
        m[linalgMatIndex(l,N+l,2*N)]=T(1);
    }


    bool ok=linalgGaussJordanV(m, N, 2*N);

    if (ok) {
        // finally we copy the result to matrix_out
        for (long k=0; k<N; k++) {
            for (long c=N; c<2*N; c++) {
                matrix_out[linalgMatIndex(k,c-N,N)] = m[linalgMatIndex(k,c,2*N)];
            }
        }
    }

    return ok;
#endif
}



/*! \brief invert the given NxN matrix using Gauss-Jordan elimination
    \ingroup tools_math_linalg

    \return \c true on success and the inverted matrix in matrix_out.

    \note It is save to call \c statisticsMatrixInversion(A,A,N) with the same argument for in and out matrix. Then the input will be overwritten with the new matrix!
    \note matrix and matrix_out have to be of size N*N. Matrices are interpreted as row-major!

    \see http://www.virtual-maxim.de/matrix-invertieren-in-c-plus-plus/
*/
template <class T>
inline bool linalgMatrixInversionV(const std::vector<T>& matrix, std::vector<T>& matrix_out, long N) {
    // first build a N*(2N) matrix of the form
    //
    //    m11 m12 ...  | 1 0 0
    //    m21 m22 ...  | 0 1 0
    //    ... ... ...  | .....
    //
    const long msize=N*N*2;
    std::vector<T> m;
    m.resize(msize);
    //StatisticsScopedPointer<T> m((T*)malloc(msize*sizeof(T))); // use scoped pointer to ensure, that m is free'd, when the function is ending
    for (long i=0; i<msize; i++) m[i]=T(0); // init with 0
    for (long l=0; l<N; l++) {
        for (long c=0; c<N; c++) { // init left half with matrix
            m[linalgMatIndex(l,c,2*N)]=matrix[linalgMatIndex(l,c,N)];
        }
        // init right half with unit matrix
        m[linalgMatIndex(l,N+l,2*N)]=T(1);
    }


    bool ok=linalgGaussJordanV(m, N, 2*N);

    if (ok) {
        // finally we copy the result to matrix_out
        for (long k=0; k<N; k++) {
            for (long c=N; c<2*N; c++) {
                matrix_out[linalgMatIndex(k,c-N,N)] = m[linalgMatIndex(k,c,2*N)];
            }
        }
    }

    return ok;
}


/*! \brief invert the given NxN matrix using Gauss-Jordan elimination
    \ingroup tools_math_linalg

*/
template <class T>
inline bool linalgMatrixInversion(T* matrix, long N) {
    return linalgMatrixInversion(matrix, matrix, N);
}

/*! \brief invert the given NxN matrix using Gauss-Jordan elimination
    \ingroup tools_math_linalg

*/
template <class T>
inline bool linalgMatrixInversionV(T matrix, long N) {
    return linalgMatrixInversionV(matrix, matrix, N);
}


/*! \brief solve a system of N linear equations \f$ A\cdot\vec{x}=B \f$ simultaneously for C columns in B
    \ingroup tools_math_linalg

    \param A an NxN matrix of coefficients
    \param B an NxC marix
    \param N number of equations
    \param C number of columns in B
    \param result_out a NxC matrix with the results after the inversion of the system of equations
    \return \c true on success

    \note This function uses the Gauss-Jordan algorithm
    \note It is save to call \c statisticsLinSolve(A,B,N,C,B) with the same argument for B and result_out. Then the input will be overwritten with the new matrix!
*/
template <class T>
inline bool linalgLinSolve(const T* A, const T* B, long N, long C, T* result_out) {
#ifdef STATISTICS_TOOLS_MAY_USE_EIGEN3
    if (N==2 && C==1) {
        Eigen::Map<const Eigen::Matrix<T,2,2,Eigen::RowMajor> > eA(A);
        Eigen::Matrix<T,2,1>  eB;
        Eigen::Map<Eigen::Matrix<T,2,1> > x(result_out);

        eB=Eigen::Map<const Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> >(B,2,1);

        x=eA.colPivHouseholderQr().solve(eB);
    } else if (N==3 && C==1) {
        Eigen::Map<const Eigen::Matrix<T,3,3,Eigen::RowMajor> > eA(A);
        Eigen::Matrix<T,3,1>  eB;
        Eigen::Map<Eigen::Matrix<T,3,1> > x(result_out);

        eB=Eigen::Map<const Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> >(B,3,1);

        x=eA.colPivHouseholderQr().solve(eB);
    } else {
        Eigen::Map<const Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> > eA(A,N,N);
        Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>  eB(N,C);
        Eigen::Map<Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> > x(result_out,N,C);

        eB=Eigen::Map<const Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> >(B,N,C);



        x=eA.colPivHouseholderQr().solve(eB);
    }
    return true;
#else
    // first build a N*(N+C) matrix of the form
    //
    //    <---- N ----> <---- C ---->
    //  ^ A11 A12 ...  | B11 B12 ...
    //  | A21 A22 ...  | B21 B22 ...
    //  N ... ... ...  | .....
    //  | ... ... ...  | .....
    //  v ... ... ...  | .....
    //
    const long msize=N*(N+C);
    StatisticsScopedPointer<T> m((T*)statisticsMalloc(msize*sizeof(T))); // use scoped pointer to ensure, that m is free'd, when the function is ending
    for (long l=0; l<N; l++) {
        for (long c=0; c<N; c++) { // init left half with matrix A
            m[linalgMatIndex(l,c,N+C)]=A[linalgMatIndex(l,c,N)];
        }
        // init right half with B
        for (long c=0; c<C; c++) { // init left half with matrix B
            m[linalgMatIndex(l,N+c,N+C)]=B[linalgMatIndex(l,c,C)];
        }
    }


    bool ok=linalgGaussJordan(m.data(), N, N+C);

    if (ok) {
        for (long k=0; k<N; k++) {
            for (long c=N; c<(N+C); c++) {
                if (!statisticsFloatIsOK(m[linalgMatIndex(k,c,N+C)])) {
                    ok=false;
                    break;
                }
            }
            if (!ok) break;
        }
        if (ok) {
            // finally we copy the result to matrix_out
            for (long k=0; k<N; k++) {
                for (long c=N; c<(N+C); c++) {
                    result_out[linalgMatIndex(k,c-N,C)] = m[linalgMatIndex(k,c,N+C)];
                }
            }
        }
    }

    return ok;
#endif
}

template <class T>
inline bool linalgLinSolve(const T* A, T* B, long N, long C) {
    return linalgLinSolve(A,B,N,C,B);
}


/*! \brief solve a system of N linear equations \f$ A\cdot\vec{x}=\vec{b} \f$
    \ingroup tools_math_linalg

    \param A an NxN matrix of coefficients
    \param b an N-entry vector
    \param N number of equations
    \param C number of columns in B
    \param result_out a N-entry vector with the result
    \return \c true on success

    \note This function uses the Gauss-Jordan algorithm
    \note It is save to call \c statisticsLinSolve(A,B,N,C,B) with the same argument for B and result_out. Then the input will be overwritten with the new matrix!
*/
template <class T>
inline bool linalgLinSolve(const T* A, const T* b, long N, T* result_out) {
    return linalgLinSolve(A, b, N, 1, result_out);
}

template <class T>
inline bool linalgLinSolve(const T* A, T* b, long N) {
    return linalgLinSolve(A,b,N,1,b);
}

/*! \brief returns the unit vector \f$ \vec{e}_r=(\sin(\theta)\cdot\cos(\phi), \sin(\theta)\cdot\sin(\phi), \cos(\theta))^T \f$ of spherical coordinates
    \ingroup tools_math_linalg

    \note angles are given in radians!!!
*/
template <class T>
inline void linalgSphericalCoordinatesUnitVecR(T* er, double theta, double phi) {
    er[0]=sin(theta)*cos(phi);
    er[1]=sin(theta)*sin(phi);
    er[2]=cos(theta);
}
/*! \brief returns the unit vector \f$ \vec{e}_\theta=( \cos(\theta)\cdot\cos(\phi), \cos(\theta)\cdot\sin(\phi), -\sin(\theta) )^T \f$ of spherical coordinates
    \ingroup tools_math_linalg

    \note angles are given in radians!!!
*/
template <class T>
inline void linalgSphericalCoordinatesUnitVecTheta(T* er, double theta, double phi) {
    er[0]=cos(theta)*cos(phi);
    er[1]=cos(theta)*sin(phi);
    er[2]=-sin(theta);
}
/*! \brief returns the unit vector \f$ \vec{e}_\phi=( -\sin(\phi), \cos(\phi), 0 )^T \f$ of spherical coordinates
    \ingroup tools_math_linalg

    \note angles are given in radians!!!
*/
template <class T>
inline void linalgSphericalCoordinatesUnitVecPhi(T* er, double /*theta*/, double phi) {
    er[0]=-sin(phi);
    er[1]=cos(phi);
    er[2]=0;
}

/*! \brief returns the rotation matrix \f$ \mathrm{\mathbf{M}}(\alpha) \f$ for a rotation by an angle (in radians) \f$ \lpaha \f$ around a direction vector \f$ \vec{n}=(n_x,n_y,n_z)^T \f$
    \ingroup tools_math_linalg

    \f[
  \mathrm{\mathbf{M}}(\alpha) = \left(\begin{matrix}\cos(\alpha)+n_x^2\cdot (1-\cos(\alpha))  &  n_x\cdot n_y\cdot (1-\cos(\alpha))-n_z\cdot \sin(\alpha) &  n_x\cdot n_z\cdot (1-\cos(\alpha))+n_y\cdot \sin(\alpha)\\
   n_x\cdot n_y\cdot (1-\cos(\alpha))+n_z\cdot \sin(\alpha) & \cos(\alpha)+n_y^2\cdot (1-\cos(\alpha))  &   n_y\cdot n_z\cdot (1-\cos(\alpha))-n_x\cdot \sin(\alpha)\\
   n_z\cdot n_x\cdot (1-\cos(\alpha))-n_y\cdot \sin(\alpha) & n_z\cdot n_y\cdot (1-\cos(\alpha))+n_x\cdot \sin(\alpha)  & \cos(\alpha)+n_z^2\cdot (1-\cos(\alpha))\end{matrix}\right)
    \f]

    \note angles are given in radians!!!
*/
template <class T>
inline void linalgRotationMatrix3D(T* M, double alpha, const T* direction) {
    const double norm=sqrt(statisticsSqr(direction[0])+statisticsSqr(direction[1])+statisticsSqr(direction[2]));
    const double nx=direction[0]/norm;
    const double ny=direction[1]/norm;
    const double nz=direction[2]/norm;
    const double ct=cos(alpha);
    const double st=sin(alpha);

    M[0]=ct+statisticsSqr(nx)*(1.0-ct);
    M[1]=nx*ny*(1.0-ct)-nz*st;
    M[2]=nx*nz*(1.0-ct)+ny*st;

    M[3]=nx*ny*(1.0-ct)+nz*st;
    M[4]=ct+statisticsSqr(ny)*(1.0-ct);
    M[5]=ny*nz*(1.0-ct)-nx*st;

    M[6]=nz*nx*(1.0-ct)-ny*st;
    M[7]=nz*ny*(1.0-ct)+nx*st;
    M[8]=ct+statisticsSqr(nz)*(1.0-ct);
}

/*! \brief rotate a vector around a direction vector \f$ \vec{n}=(n_x,n_y,n_z)^T \f$ by an angle \$ \alpha \f$
    \ingroup tools_math_linalg

    This function applies the rotation matrix:
    \f[
  \mathrm{\mathbf{M}}(\alpha) = \left(\begin{matrix}\cos(\alpha)+n_x^2\cdot (1-\cos(\alpha))  &  n_x\cdot n_y\cdot (1-\cos(\alpha))-n_z\cdot \sin(\alpha) &  n_x\cdot n_z\cdot (1-\cos(\alpha))+n_y\cdot \sin(\alpha)\\
   n_x\cdot n_y\cdot (1-\cos(\alpha))+n_z\cdot \sin(\alpha) & \cos(\alpha)+n_y^2\cdot (1-\cos(\alpha))  &   n_y\cdot n_z\cdot (1-\cos(\alpha))-n_x\cdot \sin(\alpha)\\
   n_z\cdot n_x\cdot (1-\cos(\alpha))-n_y\cdot \sin(\alpha) & n_z\cdot n_y\cdot (1-\cos(\alpha))+n_x\cdot \sin(\alpha)  & \cos(\alpha)+n_z^2\cdot (1-\cos(\alpha))\end{matrix}\right)
    \f]

    \note angles are given in radians!!!
*/
template <class T>
inline void linalgRotateVector3D(T* vector, double alpha, const T* direction) {
    double M[9];
    linalgRotationMatrix3D(M, alpha, direction);
    double vec[3]={vector[0], vector[1], vector[2]};
    linalgMatrixProduct(M, 3, 3, vec, 3, 1, vector);
}


/*! \brief fits (in a least-squares sense) a polynomial \f$ f(x)=\sum\limits_{i=0}^Pp_ix^i \f$ of order P to a set of N data pairs \f$ (x_i,y_i) \f$
    \ingroup tools_math_stat

    \param X an N-entry vector of x-values
    \param Y an N-entry vector of y-values
    \param N number of input data points
    \param P degree of the polynomial (P>=N !!!)
    \param result_out a (P+1)-entry vector with the polynomial coefficients \f$ p_i \f$
    \return \c true on success

    This function uses statisticsLinSolve() to solve the system of equations
      \f[ \begin{bmatrix} y_1\\ y_2\\ y_3 \\ \vdots \\ y_n \end{bmatrix}= \begin{bmatrix} 1 & x_1 & x_1^2 & \dots & x_1^P \\ 1 & x_2 & x_2^2 & \dots & x_2^P\\ 1 & x_3 & x_3^2 & \dots & x_3^P \\ \vdots & \vdots & \vdots & & \vdots \\ 1 & x_n & x_n^2 & \dots & x_n^P \end{bmatrix} \begin{bmatrix} p_0\\ p_1\\ p_2\\ \vdots \\ p_P \end{bmatrix}  \f]
      \f[ \vec{y}=V\vec{p}\ \ \ \ \ \Rightarrow\ \ \ \ \ \vec{p}=(V^TV)^{-1}V^T\vec{y} \]

    \see https://en.wikipedia.org/wiki/Polynomial_regression
*/
template <class T>
inline bool statisticsPolyFit(const T* X, const T* Y, long N, long P, T* result_out) {
    // build Vandermonde matrix V
    StatisticsScopedPointer<T> V((T*)statisticsMalloc(N*(P+1)*sizeof(T))); // use scoped pointer to ensure, that V is free'd, when the function is ending
    for (long l=0; l<N; l++) {
        V[linalgMatIndex(l,0,P+1)]=T(1);
        register T x=X[l];
        register T xx=x;
        for (long c=1; c<P+1; c++) {
            V[linalgMatIndex(l,c,P+1)]=x;
            x=x*xx;
        }
    }

//    std::cout<<"V = \n";
//    statisticsPrintMatrix(V.data(),N,P+1);
//    std::cout<<"\n";

    // calculate V^T
    StatisticsScopedPointer<T> VT(statisticsDuplicateArray(V.data(), N*(P+1)));
    linalgTransposeMatrix(VT.data(), N, P+1);

//    std::cout<<"V^T = \n";
//    statisticsPrintMatrix(VT.data(),P+1,N);
//    std::cout<<"\n";

    // calculate V^T*V
    StatisticsScopedPointer<T> VTV((T*)statisticsMalloc((P+1)*(P+1)*sizeof(T))); // use scoped pointer to ensure, that VTV is free'd, when the function is ending
    linalgMatrixProduct(VT.data(), P+1, N, V.data(), N, P+1, VTV.data());

//    std::cout<<"V^T*V = \n";
//    statisticsPrintMatrix(VTV.data(),P+1,P+1);
//    std::cout<<"\n";

    // calculate V^T*y
    StatisticsScopedPointer<T> VTY((T*)statisticsMalloc((P+1)*sizeof(T))); // use scoped pointer to ensure, that VTY is free'd, when the function is ending
    linalgMatrixProduct(VT.data(), P+1, N, Y, N, 1, VTY.data());

//    std::cout<<"V^T*y = \n";
//    statisticsPrintMatrix(VTY.data(),P+1,1);
//    std::cout<<"\n";

    // solve V^T*y = V^T*V*p
    bool ok=linalgLinSolve(VTV.data(), VTY.data(), P+1);

    if (ok && result_out) {
        for (long p=0; p<P+1; p++) {
            result_out[p]=VTY[p];
        }
    }

    return ok;
}

/*! \brief evaluate a polynomial \f$ f(x)=\sum\limits_{i=0}^Pp_ix^i \f$ of order P
    \ingroup tools_math_stat

    \param x where to evaluate
    \param p polynomial coefficients, size: P+1
    \param P degree of the polynomial
    \return f(x)

*/
template <class T>
inline T statisticsPolyEval(const T& x, const T* p, long P) {
    register T v=0.0;
    register T xx=1.0;
    for (long i=0; i<=P; i++) {
        v=v+p[i]*xx;
        xx=xx*x;
    }
    return v;
}


/*! \brief calculate the weighted average and variance of a given array

    \f[ \overline{v}=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot v_i}{\sum\limits_{i=0}^{N-1}w_i} \f]
    \f[ \text{Var}(v)=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot (v_i-\overline{v})^2}{\sum\limits_{i=0}^{N-1}w_i} \f]

    \note if (N>4) This function will exclude the lower q_low*100 and upper q_high*100 percent of the masked data before calculating the average/std

*/
template <class T>
inline double statisticsAverageVarianceMaskedExcludeHighLow(double& var, const bool* mask, const T* value, long long N, bool maskUseValue=true, double q_low=0.05, double q_high=0.05) {
    if ((!value)||(!mask)) return 0;
    if (N>4) {
        T* data=(T*)statisticsCalloc(N, sizeof(T));
        double res=0;
        long long NN=0;
        for (int i=0; i<N; i++) {
            if (statisticsFloatIsOK(value[i]) && mask[i]==maskUseValue) {
                data[NN]=value[i];
                NN++;
            }
        }
        statisticsSort(data, NN);
        long cutL=ceil(q_low*double(NN));
        long cutH=ceil(q_high*double(NN));
        if (cutL+cutH<NN) {
            long long NNN=0;
            for (long long i=cutL; i<NN-cutH; i++) {
                data[NNN]=data[i];
                NNN++;
            }
            res=statisticsAverageVariance(var, data, NNN);
        } else {
            res=statisticsAverageVariance(var, data, NN);
        }
        statisticsFree(data);
        return res;
    } else {
        return statisticsAverageVarianceMasked(var, mask, value, N, maskUseValue)    ;
    }

}

/*! \brief calculate the weighted average and variance of a given array

    \f[ \overline{v}=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot v_i}{\sum\limits_{i=0}^{N-1}w_i} \f]
    \f[ \text{Var}(v)=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot (v_i-\overline{v})^2}{\sum\limits_{i=0}^{N-1}w_i} \f]

    \note if (N>4) This function will exclude the masked data above the lower and below the upper given quantiles before calculating the average/std

*/
template <class T>
inline double statisticsAverageVarianceMaskedExcludeQuantiles(double& var, const bool* mask, const T* value, long long N, bool maskUseValue=true, double q_low=0.05, double q_high=0.95) {
    if ((!value)||(!mask)) return 0;
    if (N>4) {
        T* data=(T*)statisticsCalloc(N, sizeof(T));
        double res=0;
        long long NN=0;
        for (int i=0; i<N; i++) {
            if (statisticsFloatIsOK(value[i]) && mask[i]==maskUseValue) {
                data[NN]=value[i];
                NN++;
            }
        }
        statisticsSort(data, NN);
        T qL, qH;
        qL=statisticsSortedQuantile(data, NN, q_low);
        qH=statisticsSortedQuantile(data, NN, q_high);
        long long NNN=0;
        for (long long i=0; i<NN; i++) {
            if (data[i]>qL && data[i]<qH) {
                data[NNN]=data[i];
                NNN++;
            }
        }
        res=statisticsAverageVariance(var, data, NNN);

        statisticsFree(data);
        return res;
    } else {
        return statisticsAverageVarianceMasked(var, mask, value, N, maskUseValue)    ;
    }

}






/*! \brief determinant the given NxN matrix
    \ingroup tools_math_linalg

    \note for large matrices this algorithm is very slow, think about defining the macro STATISTICS_TOOLS_MAY_USE_EIGEN3 to use faster methods from the EIGEN library!
*/
template <class T>
inline T linalgMatrixDeterminant(const T* a, long N) {
#ifdef STATISTICS_TOOLS_MAY_USE_EIGEN3
    if (N < 1) { /* Error */
        return NAN;
    } else if (N == 1) { /* Shouldn't get used */
        return a[linalgMatIndex(0,0,N)];
    } else if (N == 2) {
        return a[linalgMatIndex(0,0,N)] * a[linalgMatIndex(1,1,N)] - a[linalgMatIndex(1,0,N)] * a[linalgMatIndex(0,1,N)];
    } else if (N==3){
        Eigen::Map<const Eigen::Matrix<T,3,3,Eigen::RowMajor> > eA(a);
        return eA.determinant();
    } else if (N==4){
        Eigen::Map<const Eigen::Matrix<T,4,4,Eigen::RowMajor> > eA(a);
        return eA.determinant();
    } else {
        Eigen::Map<const Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> > eA(a,N,N);
        return eA.determinant();
    }
#else
    register long i,j,j1,j2;
    register T det = 0;
    T* m = NULL;

    if (N < 1) { /* Error */
        det=NAN;
    } else if (N == 1) { /* Shouldn't get used */
        det = a[linalgMatIndex(0,0,N)];
    } else if (N == 2) {
        det = a[linalgMatIndex(0,0,N)] * a[linalgMatIndex(1,1,N)] - a[linalgMatIndex(1,0,N)] * a[linalgMatIndex(0,1,N)];
    } else {
        det = 0;
        for (j1=0;j1<N;j1++) {
            m = (T*)statisticsCalloc((N-1)*(N-1),sizeof(T *));

            for (i=1;i<N;i++) {
                j2 = 0;
                for (j=0;j<N;j++) {
                    if (j != j1){
                        m[linalgMatIndex(i-1,j2,N-1)] = a[linalgMatIndex(i,j,N)];
                        j2++;
                    }
                }
            }
            //printf("%d: %lf (%lf  %lf)\n",j1,pow(-1.0,1.0+double(j1)+1.0),a[linalgMatIndex(0,j1,N)], linalgMatrixDeterminant(m,N-1));
            det = det + double(((1+j1+1)%2==0)?1.0:-1.0)/* pow(-1.0,1.0+double(j1)+1.0)*/ * a[linalgMatIndex(0,j1,N)] * linalgMatrixDeterminant(m,N-1);

            statisticsFree(m);
        }
    }

    //printf("\n\n  linalgMatrixDeterminant(%d):\n",N);
    //linalgPrintMatrix(a,N,N);
    //printf("  linalgMatrixDeterminant(%d) = %lf\n", N, det);
    return(det);
#endif
}
















typedef double(*statisticsGetFitFunctionP)(double,const double*);
typedef double(*statisticsGetFitFunctionUserDataP)(double,const double*,void*);

/*! \brief compute the jacobi matrix for a least-squares data fit problem for function \f$ f(x,\vec{p}) \f$ with input data \f$ (x_i,y_i,\sigma_i) \f$: \f$ J_{i,j}=\frac{1}{\sigma_i}\cdot\left.\frac{\partial f(x,\vec{p})}{\partial p_j}\right|_{x=x_i} \f$
    \ingroup tools_math_stat

    this function estimates the first derivatives by the 5-point stencil rule (https://en.wikipedia.org/wiki/Five-point_stencil)

    The data vector \a dataX has to have \a N entries. The fit parameter results will be passed as \a params (with \a Nparams entries) and the given
    function \a F will have to know how many parameters to expect. The user parameters \A userData can be passed optionally (they will simply be
    forwarded to \a F. Finally the result will be stored in \a jacobi, which has to have \c N*Nparams entries, i.e. \a N rows with \a Nparams columns each.


*/
inline void statisticsGetFitProblemJacobiMatrix(double* jacobi, const double* dataX, long long N, statisticsGetFitFunctionUserDataP F, const double* params, int Nparams, const double* sigmas=NULL, void* userData=NULL, double h=10.0*sqrt(DBL_EPSILON)) {
    double* p=statisticsDuplicateArray(params, Nparams);
    for (register long long i=0; i<N; i++) {
        const double xi=dataX[i];
        const double s=(sigmas)?sigmas[i]:double(1.0);
        for (register int j=0; j<Nparams; j++) {
            register double deriv=0;
            p[j]=params[j]+2.0*h;
            deriv -= F(xi, p, userData);
            p[j]=params[j]+h;
            deriv += 8.0*F(xi, p, userData);
            p[j]=params[j]-h;
            deriv -= 8.0*F(xi, p, userData);
            p[j]=params[j]-2.0*h;
            deriv += F(xi, p, userData);

            deriv /= (12.0*h);
            deriv /= s;

            jacobi[i*Nparams+j]=deriv;

            p[j]=params[j];
        }
    }
}
/*! \brief compute the jacobi matrix for a least-squares data fit problem for function \f$ f(x,\vec{p}) \f$ with input data \f$ (x_i,y_i,\sigma_i) \f$: \f$ J_{i,j}=\frac{1}{\sigma_i}\cdot\left.\frac{\partial f(x,\vec{p})}{\partial p_j}\right|_{x=x_i} \f$
    \ingroup tools_math_stat

    this function estimates the first derivatives by the 5-point stencil rule (https://en.wikipedia.org/wiki/Five-point_stencil)

    The data vector \a dataX has to have \a N entries. The fit parameter results will be passed as \a params (with \a Nparams entries) and the given
    function \a F will have to know how many parameters to expect. The user parameters \A userData can be passed optionally (they will simply be
    forwarded to \a F. Finally the result will be stored in \a jacobi, which has to have \c N*Nparams entries, i.e. \a N rows with \a Nparams columns each.


*/
inline void statisticsGetFitProblemJacobiMatrix(double* jacobi, const double* dataX, long long N, statisticsGetFitFunctionP F, const double* params, int Nparams, const double* sigmas=NULL, double h=10.0*sqrt(DBL_EPSILON)) {
    double* p=statisticsDuplicateArray(params, Nparams);
    for (register long long i=0; i<N; i++) {
        const double xi=dataX[i];
        const double s=(sigmas)?sigmas[i]:double(1.0);
        for (register int j=0; j<Nparams; j++) {
            register double deriv=0;
            p[j]=params[j]+2.0*h;
            deriv -= F(xi, p);
            p[j]=params[j]+h;
            deriv += 8.0*F(xi, p);
            p[j]=params[j]-h;
            deriv -= 8.0*F(xi, p);
            p[j]=params[j]-2.0*h;
            deriv += F(xi, p);

            deriv /= (12.0*h);
            deriv /= s;

            jacobi[i*Nparams+j]=deriv;

            p[j]=params[j];
        }
    }
    statisticsFree(p);
}

/*! \brief compute the chi2 fit statistics for a fit problem:
    \ingroup tools_math_stat

     \f[ \chi^2(\vec{p})=\sum\limits_{i=1}^N\left(\frac{y_i-f(x_i,\vec{p})}{\sigma_i}\right)^2} \f]

*/
inline double statisticsGetFitProblemChi2(const double* dataX, const double* dataY, long long N, statisticsGetFitFunctionUserDataP F, const double* params, const double* sigmas=NULL, void* userData=NULL) {
    register double chi2=0;
    if (sigmas){
        for (register long long i=0; i<N; i++) {
            chi2=chi2+statisticsSqr((dataY[i]-F(dataX[i], params, userData))/sigmas[i]);
        }
    } else {
        for (register long long i=0; i<N; i++) {
            chi2=chi2+statisticsSqr((dataY[i]-F(dataX[i], params, userData)));
        }
    }
    return chi2;
}


/*! \brief compute the chi2 fit statistics for a fit problem:
    \ingroup tools_math_stat

     \f[ \chi^2(\vec{p})=\sum\limits_{i=1}^N\left(\frac{y_i-f(x_i,\vec{p})}{\sigma_i}\right)^2} \f]

*/
inline double statisticsGetFitProblemChi2(const double* dataX, const double* dataY, long long N, statisticsGetFitFunctionP F, const double* params, const double* sigmas=NULL) {
    register double chi2=0;
    if (sigmas){
        for (register long long i=0; i<N; i++) {
            chi2=chi2+statisticsSqr((dataY[i]-F(dataX[i], params))/sigmas[i]);
        }
    } else {
        for (register long long i=0; i<N; i++) {
            chi2=chi2+statisticsSqr((dataY[i]-F(dataX[i], params)));
        }
    }
    return chi2;
}


/*! \brief compute the variance/covariance matrix for a least-squares data fit problem from a given jacobi matrix (e.g. from statisticsGetFitProblemJacobiMatrix() )
    \ingroup tools_math_stat

    The jacobi matrix is a \c N*Nparams matrix, where \a N is the number of datapoints in the fit problem and \a Nparams is the number of fit
    parameters. The resulting \a covariance (\f$ C \f$) is then an \c Nparams*Nparams matrix.

    Theis function computes:
    \f[ \mbox{covar}=(J^T J)^{-1} \f]

*/
template <typename T>
inline void statisticsGetFitProblemCovMatrix(T* covariance, const T* jacobi, long long N, long long Nparams) {
//    std::cout<<"statisticsGetFitProblemCovMatrix: 1\n";
//    std::cout.flush();
    T* JT=statisticsDuplicateArray(jacobi, Nparams*N);
//    std::cout<<"statisticsGetFitProblemCovMatrix: 2\n";
//    std::cout.flush();
    linalgTransposeMatrix(JT, N, Nparams);
//    std::cout<<"statisticsGetFitProblemCovMatrix: 3\n";
//    std::cout.flush();
    linalgMatrixProduct(JT,Nparams,N,jacobi,N,Nparams,covariance);
//    std::cout<<"statisticsGetFitProblemCovMatrix: 4\n";
//    std::cout.flush();
    linalgMatrixInversion(covariance,Nparams);
//    std::cout<<"statisticsGetFitProblemCovMatrix: 5\n";
//    std::cout.flush();
    statisticsFree(JT);
//    std::cout<<"statisticsGetFitProblemCovMatrix: 6\n";
//    std::cout.flush();
}


/*! \brief compute the variance/covariance matrix (using statisticsGetFitProblemCovMatrix() ) for a least-squares data fit problem from a given jacobi matrix (e.g. from statisticsGetFitProblemJacobiMatrix() ) and multiplies it by \f$\sqrt{\chi^2/(N-p)}\f$ where \f$ N\f$ is the number of datapoints and \f$p\f$ is the number of parameters. This is advised to be used for un-weighted fit problems.
    \ingroup tools_math_stat

    The jacobi matrix is a \c N*Nparams matrix, where \a N is the number of datapoints in the fit problem and \a Nparams is the number of fit
    parameters. The resulting \a covariance (\f$ C \f$) is then an \c Nparams*Nparams matrix.

    Theis function computes:
    \f[ \mbox{covar}=\frac{\chi^2}{N-p}(J^T J)^{-1} \f]

*/
template <typename T>
inline void statisticsGetFitProblemVarCovMatrix(T* covariance, const T* jacobi, long long N, long long Nparams, double chi2) {
    statisticsGetFitProblemCovMatrix(covariance, jacobi, N, Nparams);
    if (chi2>0 && N>Nparams) {
        const double f=chi2/double(N-Nparams);
        for (int i=0; i<Nparams*Nparams; i++) {
            covariance[i]=covariance[i]*f;
        }
    }
}
/*! \brief compute the parameter errors from a covariance matrix computed with statisticsGetFitProblemCovMatrix()
    \ingroup tools_math_stat

    if chi2 and Ndata are provided, this function returns:
      \f[ \mbox{err}_i=\frac{\chi^2}{N-p}\cdot\sqrt{C_{p,p}} \f]
    in any other case, or when this may not be calculated, as Ndata<=Nparams, it returns simply:
      \f[ \mbox{err}_i=\sqrt{C_{p,p}} \f]

    \note You can also use this together with the varCov-matrix from   statisticsGetFitProblemVarCovMatrix(), but then you <u>have</u> to set \c chi2=Ndata=0 !

*/
template <typename T>
inline double statisticsGetFitProblemParamErrors(int param, const T* covariance, int Nparams, double chi2, int Ndata) {
    if (param<0 || param>=Nparams) return 0;
    if (chi2>0 && Ndata>0 && Ndata>Nparams) return sqrt(covariance[linalgMatIndex(param,param,Nparams)])*sqrt(chi2/double(Ndata-Nparams));
    else return sqrt(covariance[linalgMatIndex(param,param,Nparams)]);
}

/*! \brief compute the parameter errors from a covariance matrix computed with statisticsGetFitProblemVarCovMatrix()
    \ingroup tools_math_stat

      \f[ \mbox{err}_i=\sqrt{C_{p,p}} \f]


*/
template <typename T>
inline double statisticsGetFitProblemParamErrors(int param, const T* covariance, int Nparams) {
    if (param<0 || param>=Nparams) return 0;
    return sqrt(covariance[linalgMatIndex(param,param,Nparams)]);
}


#endif // STATISTICS_TOOLS_H


