/*
    Copyright (c) 2008-2014 Jan W. Krieger, German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

/*! \brief Avogadro's number \f$ N_A=6.02214179\cdot10^{23} \f$
    \ingroup qf3lib_mathtools
 */
#define QF_NAVOGADRO (6.02214179e23)

/*! \brief Boltzman's constant number \f$ k_B=1.380648813\cdot10^{-23}J/K \f$
    \ingroup qf3lib_mathtools
 */
#define QF_K_BOLTZ 1.380648813e-23

/*! \brief square root of pi \f$ \sqrt{\pi}=1.77245385091 \f$
    \ingroup qf3lib_mathtools
 */
#define QF_SQRTPI (1.77245385091)

/*! \brief square root of pi \f$ \sqrt{2\pi}=2.50662827463 \f$
    \ingroup qf3lib_mathtools
 */
#define QF_SQRT2PI (2.50662827463)

/*! \brief check whether the dlotaing point number is OK (i.e. non-inf, non-NAN)
    \ingroup qf3lib_mathtools
 */
#define QFFloatIsOK(v) (std::isfinite(v) && (!std::isinf(v)) && (!std::isnan(v)))

/*! \brief squares the argument
    \ingroup qf3lib_mathtools
 */
inline double qfSqr(double x) { return x*x; }
inline float qfSqr(float x) { return x*x; }

/*! \brief takes the argument to the third power
    \ingroup qf3lib_mathtools
 */
inline double qfCube(double x) { return x*x*x; }
inline float qfCube(float x) { return x*x*x; }

/*! \brief takes the argument to the fourth power
    \ingroup qf3lib_mathtools
 */
inline double qfPow4(double x) { double x2=x*x; return x2*x2; }
inline float qfPow4(float x) { float x2=x*x; return x2*x2; }

/*! \brief takes the argument to the fifth power
    \ingroup qf3lib_mathtools
 */
inline double qfPow5(double x) { double x2=x*x; return x2*x2*x; }
inline float qfPow5(float x) { float x2=x*x; return x2*x2*x; }

/** \brief sinc function \f$ \mbox{sinc}(x)=\frac{\sin(x)}{x} \f$ */
QFLIB_EXPORT double qfSinc(double x);
/** \brief sinc function \f$ \mbox{tanc}(x)=\frac{\tan(x)}{x} \f$ */
QFLIB_EXPORT double qfTanc(double x);

/** \brief 1/sqrt(e)-width gaussian function \f$ \mbox{g}(x, \sigma)=\exp\left(-\frac{1}{2}\cdot\frac{x^2}{\sigma^2}\right) \f$ with \f$ g(\sigma,\sigma)=1/\sqrt{e} \f$
    \ingroup qf3lib_mathtools
 */
inline double qfGaussSqrtE(double x, double sigma) {
    return exp(-0.5*x*x/(sigma*sigma));
}
/** \brief 1/sqrt(e)-width gaussian function \f$ \mbox{g}(x, \sigma=1)=\exp\left(-\frac{1}{2}\cdot\frac{x^2}{\sigma^2}\right) \f$ with \f$ g(\sigma,\sigma)=1/\sqrt{e} \f$
    \ingroup qf3lib_mathtools
 */
inline double qfGaussSqrtE(double x) {
    return qfGaussSqrtE(x, 1);
}
/** \brief normalized 1/sqrt(e)-width gaussian function \f$ \mbox{g}(x, \sigma)=\frac{1}{w\cdot\sqrt{2\pi}}\cdot\exp\left(-\frac{1}{2}\cdot\frac{x^2}{\sigma^2}\right) \f$ with \f$ g(\sigma,\sigma)=1/\sqrt{e} \f$
    \ingroup qf3lib_mathtools
 */
inline double qfGaussNormSqrtE(double x, double sigma){
    return exp(-0.5*x*x/(sigma*sigma))/(sigma*sqrt(2.0*M_PI));
}
/** \brief normalized 1/sqrt(e)-width gaussian function \f$ \mbox{g}(x, \sigma=1)=\frac{1}{w\cdot\sqrt{2\pi}}\cdot\exp\left(-\frac{1}{2}\cdot\frac{x^2}{\sigma^2}\right) \f$ with \f$ g(\sigma,\sigma)=1/\sqrt{e} \f$
    \ingroup qf3lib_mathtools
 */
inline double qfGaussNormSqrtE(double x){
    return qfGaussNormSqrtE(x,1);
}
/** \brief 1/e²-width gaussian function \f$ \mbox{g}(x, w)=\exp\left(-2\cdot\frac{x^2}{w^2}\right) \f$ with \f$ g(w,w)=1/e^2 \f$
    \ingroup qf3lib_mathtools
 */
inline double qfGaussE2(double x, double w) { return qfGaussSqrtE(x, w/2.0); }
/** \brief 1/e²-width gaussian function \f$ \mbox{g}(x, w)=\exp\left(-2\cdot\frac{x^2}{w^2}\right) \f$ with \f$ g(w,w)=1/e^2 \f$
    \ingroup qf3lib_mathtools
 */
inline double qfGaussE2(double x) { return qfGaussSqrtE(x, 1.0/2.0); }
/** \brief normalized 1/e²-width gaussian function \f$ \mbox{g}(x, w)=\frac{\sqrt{2}}{w\cdot\sqrt{\pi}}\cdot\exp\left(-2\cdot\frac{x^2}{w^2}\right) \f$
    \ingroup qf3lib_mathtools
 */
inline double qfGaussNormE2(double x, double w) { return qfGaussNormSqrtE(x, w/2.0); }
/** \brief normalized 1/e²-width gaussian function \f$ \mbox{g}(x, w)=\frac{\sqrt{2}}{w\cdot\sqrt{\pi}}\cdot\exp\left(-2\cdot\frac{x^2}{w^2}\right) \f$
    \ingroup qf3lib_mathtools
 */
inline double qfGaussNormE2(double x) { return qfGaussNormSqrtE(x, 1.0/2.0); }
/** \brief Theta step function \f$ \Theta(x)=\begin{cases}0&x<0\\1&\text{else}\end{cases} \f$
    \ingroup qf3lib_mathtools
 */
inline double qfTheta(double x) { return (x>=0)?1.0:0.0; }
/** \brief sigmoidal function \f$ \mbox{sigmoid}(x)=\frac{1}{1+\exp(-x)} \f$
    \ingroup qf3lib_mathtools
 */
inline double qfSigmoid(double x) { return 1.0/(1+exp(-1.0*x)); }
/** \brief sigmoidal function \f$ \mbox{sigmoid}(x)=\frac{1}{1+\exp(-x)} \f$
    \ingroup qf3lib_mathtools
 */
inline double qfSigmoid(double x, double width, double x0=0) { return 1.0/(1+exp(-1.0*(x-x0)/width)); }
/** \brief sign function \f$ \mbox{signa}(x)=\begin{cases}-1&x<0\\0&x=0\\1&x>0\end{cases} \f$
    \ingroup qf3lib_mathtools
 */
inline double qfSign(double x) {
    if (x<0) return -1;
    if (x>0) return 1;
    return 0;
}

/** \brief range/slit function \f$ \mbox{slit}(x)=\begin{cases}1&-a/2\leq x\leq a/2\\0&\text{else}\end{cases} \f$
    \ingroup qf3lib_mathtools
 */
inline double qfSlit(double x, double a) {
    if (x>=-a/2.0 && x<=a/2.0) return 1;
    return 0;
}

/** \brief evaluate the polynomial defined by coefficients at the position x
    \ingroup qf3lib_mathtools
 */
inline double qfEvalPolynomial(double x, const double* coefficients, int NC) {
    register double r=0;
    register double xf=1;
    for (register int i=0; i<NC; i++) {
        r+=coefficients[i]*xf;
        xf=xf*x;
    }
    return r;
}

/** \brief evaluate the polynomial defined by coefficients at the position x
    \ingroup qf3lib_mathtools
 */
inline double qfEvalPolynomial(double x, const QVector<double>& coefficients) {
    return qfEvalPolynomial(x, coefficients.data(), coefficients.size());
}


/** \brief derive the polynomial defined by coefficients at the position x
    \ingroup qf3lib_mathtools
 */
inline QVector<double> qfDerivePolynomial(const QVector<double>& coefficients) {
    QVector<double> r;
    if (coefficients.size()<=1) {
        r<<0.0;
        return r;
    }
    for (int i=1; i<coefficients.size(); i++) {
        r<<coefficients[i]*double(i);
    }
    return r;
}


/** \brief convert degrees (0..360) ro radians (0..2pi)
    \ingroup qf3lib_mathtools
 */
inline double qfDegToRad(double x) {
    return x/180.0*M_PI;
}

/** \brief convert degrees (0..360) ro radians (0..2pi)
    \ingroup qf3lib_mathtools
 */
inline double qfRadToDeg(double x) {
    return x/M_PI*180.0;
}

/** \brief normalized cauchy distribution
    \ingroup qf3lib_mathtools
 */
inline double qfCauchy(double x, double fwhm, double center=0.0) {
    return 1.0/(qfSqr((x-center)/fwhm)+1.0)/M_PI/fwhm;
}

/** \brief cauchy distribution with given amplitude
    \ingroup qf3lib_mathtools
 */
inline double qfCauchy(double x, double A, double fwhm, double center) {
    return A/(qfSqr((x-center)/fwhm)+1.0);
}

/** \brief calculate the error propagation for <code>factorA*a+factorB*b</code> or <code>factorA*a-factorB*b</code> with errors \a ea and \a eb
    \ingroup qf3lib_mathtools

    \f[ f=\alpha a\pm\beta b \f]
    \f[ \Delta f=\sqrt{\left(\Delta a\cdot\alpha\right)^2+\left(\Delta b\cdot\beta\right)^2} \f]
 */
inline double qfErrorSumMinus(double a, double ea, double b, double eb, double factorA=1.0, double factorB=1.0) {
    return sqrt(qfSqr(factorA*ea)+qfSqr(factorB*eb));
}

/** \brief calculate the error propagation for <code>a*b</code> with errors \a ea and \a eb
    \ingroup qf3lib_mathtools

    \f[ f=a\cdot b \f]
    \f[ \Delta f=\sqrt{\left(\Delta a\cdot b\right)^2+\left(\Delta b\cdot a\right)^2} \f]
 */
inline double qfErrorMul(double a, double ea, double b, double eb) {
    return sqrt(qfSqr(ea*b)+qfSqr(eb*a));
}

/** \brief calculate the error propagation for <code>a/b</code> with errors \a ea and \a eb
    \ingroup qf3lib_mathtools
    \f[ f=\frac{a}{b} \f]
    \f[ \Delta f=\sqrt{\left(\Delta a\cdot b\right)^2+\left(\Delta b\cdot\frac{1}{b^2}\right)^2} \f]
 */
inline double qfErrorDiv(double a, double ea, double b, double eb) {
    return sqrt(qfSqr(ea*b)+qfSqr(eb*a/b/b));
}

/** \brief calculate the error propagation for <code>a^(factorB*b)</code> with errors \a ea and \a eb
    \ingroup qf3lib_mathtools
    \f[ f=a^{\beta b} \f]
    \f[ \Delta f=\sqrt{\left(\Delta a\cdot a^{\beta b-1}\right)^2+\left(\Delta b\cdot a^{\beta b}\beta\cdot\log(a)\right)^2} \f]
 */
inline double qfErrorPow(double a, double ea, double b, double eb, double factorB=1.0) {
    return sqrt(qfSqr(ea*pow(a, factorB*b-1.0))+qfSqr(eb*pow(a, factorB*b)*factorB*log(a)));
}

/** \brief calculate the error propagation for <code>sqrt(factorA*a+offsetA)</code> with errors \a ea
    \ingroup qf3lib_mathtools
    \f[ f=\sqrt{\alpha a+o} \f]
    \f[ \Delta f=\left|\left(\frac{\Delta a\cdot\alpha}{2\sqrt{\alpha a+o}}\right)\right| \f]
 */
inline double qfErrorSqrt(double a, double ea, double factorA=1.0, double offsetA=0.0) {
    return fabs(ea/sqrt(factorA*a+offsetA)*factorA/2.0);
}

/** \brief calculate the error propagation for <code>sqrt(factorA*a+factorB*b+offset)</code> with errors \a ea
    \ingroup qf3lib_mathtools
    \f[ f=\sqrt{\alpha a+\beta b+o} \f]
    \f[ \Delta f=\sqrt{\left(\frac{\Delta a\cdot\alpha}{2\sqrt{\alpha a+\beta b+o}}\right)^2+\left(\frac{\Delta b\cdot\beta}{2\sqrt{\alpha a+\beta b+o}}\right)^2} \f]
 */
inline double qfErrorSqrt(double a, double ea, double b, double eb, double factorA=1.0, double factorB=1.0, double offset=0.0) {
    const double f=2.0*sqrt(factorA*a+factorB*b+offset);
    return sqrt(qfSqr(ea/f*factorA)+qfSqr(eb/f*factorB));
}

/** \brief calculate the error propagation for <code>sin(factorA*a+factorB*b+offset)</code> with errors \a ea
    \ingroup qf3lib_mathtools
    \f[ f=\sin(\alpha a+\beta b+o) \f]
    \f[ \Delta f=\sqrt{\left(\Delta a\cdot\alpha\cdot\cos(\alpha a+\beta b+o)\right)^2+\left(\Delta b\cdot\beta\cdot\cos(\alpha a+\beta b+o)\right)^2} \f]
 */
inline double qfErrorSin(double a, double ea, double b, double eb, double factorA=1.0, double factorB=1.0, double offset=0.0) {
    const double f=cos(factorA*a+factorB*b+offset);
    return sqrt(qfSqr(ea*f*factorA)+qfSqr(eb*f*factorB));
}

/** \brief calculate the error propagation for <code>cos(factorA*a+factorB*b+offset)</code> with errors \a ea
    \ingroup qf3lib_mathtools
    \f[ f=\cos(\alpha a+\beta b+o) \f]
    \f[ \Delta f=\sqrt{\left(\Delta a\cdot\alpha\cdot\sin(\alpha a+\beta b+o)\right)^2+\left(\Delta b\cdot\beta\cdot\sin(\alpha a+\beta b+o)\right)^2} \f]
 */
inline double qfErrorCos(double a, double ea, double b, double eb, double factorA=1.0, double factorB=1.0, double offset=0.0) {
    const double f=sin(factorA*a+factorB*b+offset);
    return sqrt(qfSqr(ea*f*factorA)+qfSqr(eb*f*factorB));
}

/** \brief calculate the error propagation for <code>tan(factorA*a+factorB*b+offset)</code> with errors \a ea
    \ingroup qf3lib_mathtools
    \f[ f=\tan(\alpha a+\beta b+o) \f]
    \f[ \Delta f=\sqrt{\left(\Delta a\cdot\alpha\cdot\left[\cos(\alpha a+\beta b+o)\right]^{-2}\right)^2+\left(\Delta b\cdot\beta\cdot\left[\sin(\alpha a+\beta b+o)\right]^{-2}\right)^2} \f]
 */
inline double qfErrorTan(double a, double ea, double b, double eb, double factorA=1.0, double factorB=1.0, double offset=0.0) {
    const double f=1.0/qfSqr(cos(factorA*a+factorB*b+offset));
    return sqrt(qfSqr(ea*f*factorA)+qfSqr(eb*f*factorB));
}


/*! \brief calculate the dot product of two vectors
    \ingroup qf3lib_mathtools

*/
template <typename T>
typename T::value_type qfDotProduct(const T& v1, const T& v2) {
    if (v1.size()!=v2.size()) return 0;
    typename T::value_type r=0;
    for (int i=0; i<v1.size(); i++) {
        r=r+v1[i]*v2[i];
    }
    return r;
}




/*! \brief calculate the average and variance of a given array
    \ingroup qf3lib_mathtools

    \f[ \overline{v}=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot v_i}{\sum\limits_{i=0}^{N-1}w_i} \f]
    \f[ \text{Var}(v)=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot (v_i-\overline{v})^2}{\sum\limits_{i=0}^{N-1}w_i} \f]

*/
template <class T>
double qfstatisticsAverageVariance(double& var, const T& value) {
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
    var= fabs( sum2 - sum*sum/(double)NN ) / (double)(NN-1);
    if (!QFFloatIsOK(var) || var<0.0) {
        var=0.0;
    }
    return sum/(double)NN;
}




/*! \brief calculate the average and variance of a given array, limited to where mask==maskUseValue
    \ingroup qf3lib_mathtools

    \f[ \overline{v}=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot v_i}{\sum\limits_{i=0}^{N-1}w_i} \f]
    \f[ \text{Var}(v)=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot (v_i-\overline{v})^2}{\sum\limits_{i=0}^{N-1}w_i} \f]

*/
template <class T>
double qfstatisticsMaskedAverageVariance(double& var, const QVector<bool>& mask, const T& value, bool maskUseValue=true) {
    if (mask.size()<value.size()) return qfstatisticsAverageVariance(var, value);
    long long N=value.size();
    if (N<=1) return 0;
    register double sum=0;
    register double sum2=0;
    long long NN=0;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i]) && mask[i]==maskUseValue) {
            sum2=sum2+(double)(value[i]*value[i]);
            sum=sum+(double)(value[i]);
            NN++;
        }
    }
    var= fabs( sum2 - sum*sum/(double)NN ) / (double)(NN-1);
    if (!QFFloatIsOK(var) || var<0.0) {
        var=0.0;
    }

    return sum/(double)NN;
}




/*! \brief calculate the average and variance of a given array
    \ingroup qf3lib_mathtools

    \f[ \overline{v}=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot v_i}{\sum\limits_{i=0}^{N-1}w_i} \f]
    \f[ \text{Var}(v)=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot (v_i-\overline{v})^2}{\sum\limits_{i=0}^{N-1}w_i} \f]

*/
template <class T>
double qfstatisticsAverageStd(double& std, const T& value) {
    double var=0;
    double avg=qfstatisticsAverageVariance(var, value);
    var=sqrt(var);
    return avg;
}


/*! \brief calculate the average and variance of a given array
    \ingroup qf3lib_mathtools

    \f[ \overline{v}=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot v_i}{\sum\limits_{i=0}^{N-1}w_i} \f]
    \f[ \text{Var}(v)=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot (v_i-\overline{v})^2}{\sum\limits_{i=0}^{N-1}w_i} \f]

*/
template <class T>
double qfstatisticsMaskedverageStd(double& std, const QVector<bool>& mask, const T& value, bool maskUseValue=true) {
    if (mask.size()<value.size()) return qfstatisticsAverageStd(std, value);
    double var=0;
    double avg=qfstatisticsMaskedAverageVariance(var, mask, value, maskUseValue);
    var=sqrt(var);
    return avg;
}

/*! \brief calculate the average and variance of a given array
    \ingroup qf3lib_mathtools

    \f[ \overline{v}=\frac{1}{N}\cdot\sum\limits_{i=0}^{N-1} v_i \f]
*/
template <class T>
double qfstatisticsAverage(const T& value) {
    long long N=value.size();
    if (N==1) return value[0];
    if (N<=0) return 0;
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

/*! \brief calculate the average and variance of a given array, limited to where mask==maskUseValue
    \ingroup qf3lib_mathtools

    \f[ \overline{v}=\frac{1}{N}\cdot\sum\limits_{i=0}^{N-1} v_i \f]
*/
template <class T>
double qfstatisticsMaskedAverage(const QVector<bool>& mask, const T& value, bool maskUseValue=true) {
    if (mask.size()<value.size()) return qfstatisticsAverage(value);
    long long N=value.size();
    if (N==1) return value[0];
    if (N<=0) return 0;
    register double sum=0;
    long long NN=0;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i]) && mask[i]==maskUseValue) {
            sum=sum+(double)(value[i]);
            NN++;
        }
    }
    return sum/(double)NN;
}



/*! \brief calculate the correlation coefficient
    \ingroup qf3lib_mathtools

    \f[ \text{Kor}(x,y)=\frac{\sum\limits_{i=0}^{N-1}(x_i-\overline{x})(y_i-\overline{y})}{\sqrt{\sum\limits_{i=0}^{N-1}(x_i-\overline{x})^2\cdot\sum\limits_{i=0}^{N-1}(y_i-\overline{y})^2}} \f]

*/
template <class T>
double qfstatisticsCorrCoeff(const T& X, const T& Y) {
    long long N=qMin(X.size(), Y.size());
    if (N<=1) return 0;
    register double xbar=0;
    register double ybar=0;
    register double NN=0;
    for (long long i=0; i<N; i++) {
        const double xm=X[i];
        const double ym=Y[i];
        if (QFFloatIsOK(xm) && QFFloatIsOK(ym)) {
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
        const double xm=X[i]-xbar;
        const double ym=Y[i]-ybar;
        if (QFFloatIsOK(xm) && QFFloatIsOK(ym)) {
            sumxy=sumxy+xm*ym;
            sumx=sumx+xm*xm;
            sumy=sumy+ym*ym;
        }
    }
    return sumxy/sqrt(sumx*sumy);
}

/*! \brief calculate the skewness of a dataset
    \ingroup qf3lib_mathtools

    \f[ \gamma_1=\mathbb{E}\left[\left(\frac{X-\mu}{\sigma}\right)^3\right]= \frac{m_3}{m_2^{3/2}}  = \frac{\frac{1}{n} \sum_{i=1}^n (x_i-\overline{x})^3}{\left(\frac{1}{n} \sum_{i=1}^n (x_i-\overline{x})^2\right)^{3/2}} \f]
    where \f$\mu\f$ is the mean and \f$\sigma\f$ the standard deviation of a random variable \f$X\f$ and \f$\overline{x}\f$ is the average (calculated using statisticsAverage() ) of
    the input dataset \f$ x_i\f$.
*/
template <class T>
double qfstatisticsSkewness(const T& value) {
    long long N=value.size();
    if (N<=0) return 0;
    register double avg=qfstatisticsAverage(value);
    register double sum3=0;
    register double sum2=0;
    register double NN=0;
    for (register long long i=0; i<N; i++) {
        const double v=value[i]-avg;
        if (QFFloatIsOK(v)) {
            sum3=sum3+(double)(v*v*v);
            sum2=sum2+(double)(v*v);
            NN++;
        }
    }
    const double down=qfCube(sum2/double(NN));
    return sum3/double(NN)/sqrt(down);
}


/*! \brief calculate the given central  moment
    \ingroup qf3lib_mathtools

    \f[ \langle X^n\rangle= \mathbb{E}\left[\left(X-\mu\right)^n\right] \f]
    where \f$\mu\f$ is the mean of a random variable \f$X\f$ and \f$\overline{x}\f$ is the average (calculated using statisticsAverage() ) of
    the input dataset \f$ x_i\f$.
*/
template <class T>
double qfstatisticsCentralMoment(const T& value, int order) {
    int N=value.size();
    if (N<=0) return 0;
    register double avg=qfstatisticsAverage(value);
    register double sum=0;
    register double NN=0;
    for (register long long i=0; i<N; i++) {
        const double v=value[i]-avg;
        if (QFFloatIsOK(v)) {
            sum=sum+pow(v, order);
            NN++;
        }
    }
    return sum/double(NN);
}


/*! \brief calculate the given non-central  moment
    \ingroup qf3lib_mathtools

    \f[ \langle X^n\rangle= \mathbb{E}\left[X^n\right] \f]
    where \f$\mu\f$ is the mean of a random variable \f$X\f$ and \f$\overline{x}\f$ is the average (calculated using statisticsAverage() ) of
    the input dataset \f$ x_i\f$.
*/
template <class T>
double qfstatisticsMoment(const T& value, int order) {
    int N=value.size();
    if (N<=0) return 0;
    register double sum=0;
    register double NN=0;
    for (register long long i=0; i<N; i++) {
        const double v=value[i];
        if (QFFloatIsOK(v)) {
            sum=sum+pow(v, order);
            NN++;
        }
    }
    return sum/double(NN);
}

/*! \brief calculate the number of elements in \a value that contain a valid float
    \ingroup qf3lib_mathtools
*/
template <class T>
long long qfstatisticsCount(const T& value) {
    long long N=value.size();
    if (N<=0) return 0;
    register long long NN=0;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i])) {
            NN++;
        }
    }
    return NN;
}


/*! \brief calculate the number of elements in \a value that contain a valid float, limited to where mask==maskUseValue
    \ingroup qf3lib_mathtools

*/
template <class T>
double qfstatisticsMaskedCount(const QVector<bool>& mask, const T& value, bool maskUseValue=true) {
    if (mask.size()<value.size()) return qfstatisticsCount(value);
    long long N=value.size();
    if (N<1) return 0;
    register long long NN=0;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i]) && mask[i]==maskUseValue) {
            NN++;
        }
    }
    return NN;
}

/*! \brief calculate the minimum of the data in \a value
    \ingroup qf3lib_mathtools

*/
template <typename T>
typename T::value_type qfstatisticsMin(const T& value) {
    long long N=value.size();
    if (N<=0) return 0;
    if (N==1) return value[0];
    typename T::value_type m=0;
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
template <typename T>
typename T::value_type qfstatisticsMax(const T& value) {
    long long N=value.size();
    if (N<=0) return 0;
    if (N==1) return value[0];
    typename T::value_type m=0;
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


/*! \brief filter the vecor for NAN ....
    \ingroup qf3lib_mathtools

*/
template <class T>
T qfstatisticsFilter(const T& value) {
    long long N=value.size();
    if (N<1) return T();
    T res;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i])) {
            res<<(double)(value[i]);
        }
    }
    return res;
}

/*! \brief calculate the sum of data
    \ingroup qf3lib_mathtools

    \f[ \overline{v}=\sum\limits_{i=0}^{N-1} v_i \f]
*/
template <class T>
double qfstatisticsSum(const T& value) {
    long long N=value.size();
    if (N<1) return 0;
    register double sum=0;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i])) {
            sum=sum+(double)(value[i]);
        }
    }
    return sum;
}

/*! \brief calculate the sum of data, limited to where mask==maskUseValue
    \ingroup qf3lib_mathtools

    \f[ \overline{v}=\sum\limits_{i=0}^{N-1} v_i \f]
*/
template <class T>
double qfstatisticsMaskedSum(const QVector<bool>& mask, const T& value, bool maskUseValue=true) {
    if (mask.size()<value.size()) return qfstatisticsSum(value);
    long long N=value.size();
    if (N<1) return 0;
    register double sum=0;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i]) && mask[i]==maskUseValue) {
            sum=sum+(double)(value[i]);
        }
    }
    return sum;
}

/*! \brief calculate the cumulative sum of data
    \ingroup qf3lib_mathtools

    \f[ X_n=\sum\limits_{i=0}^{n-1} v_i \f]
*/
template <class T>
T qfstatisticsCumSum(const T& value) {
    long long N=value.size();
    if (N<1) return T();
    double sum=0;
    T res;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i])) {
            sum=sum+(double)(value[i]);

        }
        res.append(sum);
    }
    return res;
}


/*! \brief calculate the differences o the vector of numbers
    \ingroup qf3lib_mathtools

    \f[ X_n=v_n-v_{n-1} \f]
    The returned vector has one item less than the input vector
*/
template <class T>
T qfstatisticsDiff(const T& value) {
    long long N=value.size();
    if (N<=1) return T();
    T res;
    if (N>1) {
        for (register long long i=1; i<N; i++) {
            if (QFFloatIsOK(value[i]) && QFFloatIsOK(value[i-1])) {
                res.append(value[i]-value[i-1]);

            } else {
                res.append(NAN);
            }

        }
    }
    return res;
}





/*! \brief calculate the trapezoidal numerical integration
    \ingroup qf3lib_mathtools

    \f[ \int f(x)\mathrm{d}x=\frac{1}{2}\cdot\sum\limits_{n=1}^{N-1}\left(f(x_n)+f(x_n+1)\right) \f]

*/
template <class T>
double qfstatisticsTrapz(const T& value_in) {
    T value=qfstatisticsFilter(value_in);
    long long N=value.size();
    if (N<=1) return 0;
    double res=0;
    if (N>1) {
        for (register long long i=0; i<N-1; i++) {
            res=res+(value[i]+value[i+1]);
        }
    }
    return res/2.0;
}


/*! \brief calculate the trapezoidal numerical integration
    \ingroup qf3lib_mathtools

    \f[ \int f(x)\mathrm{d}x=\frac{1}{2}\cdot\sum\limits_{n=1}^{N-1}(x_{n+1}-x_n)\cdot\left[f(x_n)+f(x_n+1)\right] \f]

*/
template <class T>
double qfstatisticsTrapzXY(const T& valueX, const T& valueY) {
    T X,Y;

    long long N=qMin(valueX.size(), valueY.size());
    if (N<=1) return 0;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(valueX[i]) && QFFloatIsOK(valueY[i])) {
            X<<(double)(valueX[i]);
            Y<<(double)(valueY[i]);
        }
    }

    N=qMin(X.size(), Y.size());
    if (N<=1) return 0;
    double res=0;
    if (N>1) {
        for (register long long i=0; i<N-1; i++) {
            res=res+(Y[i]+Y[i+1])*(X[i+1]-X[i]);
        }
    }
    return res/2.0;
}



/*! \brief calculate the cumulative sum of data
    \ingroup qf3lib_mathtools

    \f[ X_n=\prod\limits_{i=0}^{n-1} v_i \f]
*/
template <class T>
T qfstatisticsCumProd(const T& value) {
    long long N=value.size();
    if (N<1) return T();
    double prod=1;
    T res;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i])) {
            prod=prod*(double)(value[i]);

        }
        res.append(prod);
    }
    return res;
}

/*! \brief calculate the product of data
    \ingroup qf3lib_mathtools

    \f[ \overline{v}=\prod\limits_{i=0}^{N-1} v_i \f]
*/
template <class T>
double qfstatisticsProd(const T& value) {
    long long N=value.size();
    if (N<1) return 0;
    register double prod=1;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i])) {
            prod=prod*(double)(value[i]);
        }
    }
    return prod;
}
/*! \brief calculate the sum of data squares
    \ingroup qf3lib_mathtools

    \f[ \overline{v}=\cdot\sum\limits_{i=0}^{N-1} v_i^2 \f]
*/
template <class T>
double qfstatisticsSum2(const T& value) {
    long long N=value.size();
    if (N<1) return 0;
    register double sum=0;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i])) {
            sum=sum+(double)(value[i])*(double)(value[i]);
        }
    }
    return sum;
}


/*! \brief calculate the sum of squared data, limited to where mask==maskUseValue
    \ingroup qf3lib_mathtools

    \f[ \overline{v}=\sum\limits_{i=0}^{N-1} v_i^2 \f]
*/
template <class T>
double qfstatisticsMaskedSum2(const QVector<bool>& mask, const T& value, bool maskUseValue=true) {
    if (mask.size()<value.size()) return qfstatisticsSum2(value);
    long long N=value.size();
    if (N<1) return 0;
    register double sum=0;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i]) && mask[i]==maskUseValue) {
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
double qfstatisticsVariance(const T& value) {
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
    double var= fabs( sum2 - sum*sum/(double)NN ) / (double)(NN-1);
    if (!QFFloatIsOK(var) || var<0.0) {
        var=0.0;
    }
    return var;
}

template <class T>
double qfstatisticsStd(const T& value) {
    return sqrt(qfstatisticsVariance(value));
}




/*! \brief calculate the average and variance of a given array, limited to where mask==maskUseValue
    \ingroup qf3lib_mathtools

    \f[ \overline{v}=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot v_i}{\sum\limits_{i=0}^{N-1}w_i} \f]
    \f[ \text{Var}(v)=\frac{\sum\limits_{i=0}^{N-1}w_i\cdot (v_i-\overline{v})^2}{\sum\limits_{i=0}^{N-1}w_i} \f]

*/
template <class T>
double qfstatisticsMaskedVariance(const QVector<bool>& mask, const T& value, bool maskUseValue=true) {
    if (mask.size()<value.size()) return qfstatisticsVariance(value);
    long long N=value.size();
    if (N<=1) return 0;
    register double sum=0;
    register double sum2=0;
    long long NN=0;
    for (register long long i=0; i<N; i++) {
        if (QFFloatIsOK(value[i]) && mask[i]==maskUseValue) {
            sum2=sum2+(double)(value[i]*value[i]);
            sum=sum+(double)(value[i]);
            NN++;
        }
    }
    double var= fabs(( sum2 - sum*sum/(double)NN ) / (double)(NN-1));
    if (!QFFloatIsOK(var) || var<0.0) {
        var=0.0;
    }
    return var;
}

template <class T>
double qfstatisticsStd(const QVector<bool>& mask, const T& value, bool maskUseValue=true) {
    return sqrt(qfstatisticsMaskedVariance(mask, value, maskUseValue));
}


/*! \brief return the smallest element from a sorted array
    \ingroup tools_math_stat
*/
template <typename T>
typename T::value_type qfstatisticsSortedMin(const T& input) {
    if (input.size()<=0) return 0;
    return input[0];
}


/*! \brief return the largest element from a sorted array
    \ingroup tools_math_stat
*/
template <typename T>
typename T::value_type qfstatisticsSortedMax(const T& input) {
    if (input.size()<=0) return 0;
    return input[input.size()-1];
}


/*! \brief return the median from a sorted array
    \ingroup tools_math_stat

    if \a N is odd, then the center element is returned, otherwise the function returns the average of the two centering elements
*/
template <typename T>
typename T::value_type qfstatisticsSortedMedian(const T& input) {
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
typename T::value_type qfstatisticsMedian(const T& input_in) {
    T input=qfstatisticsFilter(input_in);
    qSort(input);
    return qfstatisticsSortedMedian(input);
}



/*! \brief return the median from a array, limited to where mask==maskUseValue
    \ingroup tools_math_stat

    if \a N is odd, then the center element is returned, otherwise the function returns the average of the two centering elements
*/
template <typename T>
typename T::value_type qfstatisticsMaskedMedian(const QVector<bool>& mask, const T& input_in, bool maskUseValue=true) {
    if (mask.size()<input_in.size()) return qfstatisticsMedian(input_in);
    T input;
    for (int i=0; i<input_in.size(); i++) {
        if (mask[i]==maskUseValue) input<<input_in[i];
    }
    input=qfstatisticsFilter(input);
    qSort(input);
    return qfstatisticsSortedMedian(input);
}


/*! \brief calculates the median absolute deviation about the median (MAD), a robust measure of sample deviation, the data has to be a sorted array!

    \f[ \mbox{MAD}(\vec{x})=\mbox{Med}\left\{|\vec{x}-\mbox{Med}(\vec{x})|\right\} \f]

    If \a median is \c !=NULL, the median is returned in this parameter

    \see Ricardo A. Maronna, R. Douglas Martin, Victor J. Yohai: "Robust Statistics: Theory and Methods", Wiley, 2006, ISBN: 978-0-470-01092-1

*/
template <typename T>
typename T::value_type qfstatisticsSortedMAD(const T& value, typename T::value_type* median=NULL) {
    long long N=value.size();
    if (N<=1) return 0;
    register typename T::value_type med=qfstatisticsSortedMedian(value);
    T sorted=value;
    for (int i=0; i<N; i++) {
        sorted[i]=fabs(value[i]-med);
    }
    qSort(sorted);
    register typename T::value_type res=qfstatisticsSortedMedian(sorted);
    if (median) *median=med;
    return res;
}

/*! \brief calculates the normalized median absolute deviation about the median (NMAD), a robust measure of sample deviation, the data has to be a sorted array!

    \f[ \mbox{NMAD}(\vec{x})=\frac{\mbox{MAD}(\vec{x})}{0.6745}=\frac{\mbox{Med}\left\{|\vec{x}-\mbox{Med}(\vec{x})|\right\}}{0.6745} \f]

    If \a median is \c !=NULL, the median is returned in this parameter

    \see Ricardo A. Maronna, R. Douglas Martin, Victor J. Yohai: "Robust Statistics: Theory and Methods", Wiley, 2006, ISBN: 978-0-470-01092-1

*/
template <typename T>
typename T::value_type qfstatisticsSortedNMAD(const T& value, typename T::value_type* median=NULL) {
    return qfstatisticsSortedMAD(value, median)/0.6745;
}


/*! \brief calculates the median absolute deviation about the median (MAD), a robust measure of sample deviation, the data has to be a sorted array!

    \f[ \mbox{MAD}(\vec{x})=\mbox{Med}\left\{|\vec{x}-\mbox{Med}(\vec{x})|\right\} \f]

    If \a median is \c !=NULL, the median is returned in this parameter

    \see Ricardo A. Maronna, R. Douglas Martin, Victor J. Yohai: "Robust Statistics: Theory and Methods", Wiley, 2006, ISBN: 978-0-470-01092-1

*/
template <typename T>
typename T::value_type qfstatisticsMAD(const T& value, typename T::value_type* median) {
    long long N=value.size();
    if (N<=1) return 0;
    T sorted=qfstatisticsFilter(value);
    qSort(sorted);
    register typename T::value_type res=qfstatisticsSortedMAD(sorted, median);
    return res;
}

template <typename T>
typename T::value_type qfstatisticsMADS(const T& value) {
    typename T::value_type median;
    return  qfstatisticsMAD(value, &median);
}

/*! \brief calculates the normalized median absolute deviation about the median (NMAD), a robust measure of sample deviation, the data has to be a sorted array!

    \f[ \mbox{NMAD}(\vec{x})=\frac{\mbox{MAD}(\vec{x})}{0.6745}=\frac{\mbox{Med}\left\{|\vec{x}-\mbox{Med}(\vec{x})|\right\}}{0.6745} \f]

    If \a median is \c !=NULL, the median is returned in this parameter

    \see Ricardo A. Maronna, R. Douglas Martin, Victor J. Yohai: "Robust Statistics: Theory and Methods", Wiley, 2006, ISBN: 978-0-470-01092-1

*/
template <typename T>
double qfstatisticsNMAD(const T& value, typename T::value_type* median) {
    return double(qfstatisticsMAD(value, median))/0.6745;
}

template <typename T>
double qfstatisticsNMADS(const T& value) {
    typename T::value_type median;
    return  qfstatisticsNMAD(value, &median);
}


/*! \brief return the given quantile from a sorted array
    \ingroup tools_math_stat

*/
template <typename T>
typename T::value_type qfstatisticsSortedQuantile(const T& input, double quantile) {
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
typename T::value_type qfstatisticsQuantile(const T& input_in, double quantile) {
    T input=qfstatisticsFilter(input_in);
    qSort(input);
    return qfstatisticsSortedQuantile(input, quantile);
}







/*! \brief return the quantile from a array, limited to where mask==maskUseValue
    \ingroup tools_math_stat

    if \a N is odd, then the center element is returned, otherwise the function returns the average of the two centering elements
*/
template <typename T>
typename T::value_type qfstatisticsMaskedQuantile(const QVector<bool>& mask, const T& input_in, double quantile, bool maskUseValue=true) {
    if (mask.size()<input_in.size()) return qfstatisticsQuantile(input_in, quantile);
    T input;
    for (int i=0; i<input_in.size(); i++) {
        if (mask[i]==maskUseValue) input<<input_in[i];
    }
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
QFLIB_EXPORT double roundError(double error, int addSignifcant) ;
inline double roundError(double error) {
    return roundError(error, 1);
}
inline double roundError(double error, double addSignifcant) {
    return roundError(error, int(addSignifcant));
}

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
        QVector<double> fitfunc;   /**<  */
        QVector<double> residuals;   /**<  */
        QVector<double> residuals_weighted;   /**<  */
        int runAvgMaxN;   /**<  */
        int runAvgN;   /**<  */
        QVector<double> tau_runavg;   /**<  */
        QVector<double> residuals_runavg;   /**<  */
        QVector<double> residuals_runavg_weighted;   /**<  */
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
        QVector<double> resHistogram;   /**<  */
        double resHistogramCount;   /**<  */
        QVector<double> resWHistogram;   /**<  */
        double resWHistogramCount;   /**<  */
        QVector<double> resCorrelation;   /**<  */
        QVector<double> resWCorrelation;   /**<  */
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

    \note the arrays in the resulting struct are allocated using \c qfMalloc(), so you will have to free them using \c qfFree() !!!
  */
QFLIB_EXPORT QFFitStatistics calculateFitStatistics(long N, const double* tauvals, const double* model, const double* corrdata, const double* weights, int datacut_min, int datacut_max, int paramCount, int runAvgWidth, int residualHistogramBins);

#endif // QFMATHTOOLS_H
