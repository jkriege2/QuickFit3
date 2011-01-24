#ifndef QFFITALGORITHM_H
#define QFFITALGORITHM_H

#include "qffitfunction.h"
#include <QString>
#include <QMap>
#include <QVariant>

/*! \brief describes a virtual base class fitting algorithms that are applied to QFFitFunction objects.
    \ingroup qf3lib_fitting

    When implementing a fitting algorithm you will have to implement this:
      - implement name(), id() and helpFile()
      - in the cosntructor you will have to set the parameters of your fitting algorithm using setParameter()
        later on you can read it back using getParameter().
      - in the protected function intMinimize() you will have to implement your fitting algorithm as an optimization
        algorithm which minimizes a function \f$ f(\vec{p}) \f$ of a set of parameters \f$ \vec{p} \f$ :
          \f[ \vec{p}^\ast=\min\limits_{\vec{p}}f(\vec{p}) \f]
        The function \f$ f(\vec{p}) \f$ is implemented as a QFFitAlgorithm::Functor object.
      - optionally you may overwrite displayConfig() which displays a modal configuration dialog for your fit algorithm
    .
*/
class QFFitAlgorithm {
    public:
        /*! \brief functor base class that may be used to optimize arbitrary functions

            This functor allows to evaluate arbitrary vector-valued functions \f[ \vec{y}=\vec{f}(\vec{p})\in\mathbf{R}^M, \vec{p}\in\mathbb{R}^N. \f]

            This can be used to either solve a simple minimization problem problem
              \f[ \vec{p}^\ast=\min\limits_{\vec{p}}g(\vec{p}) \f]
            by setting \c Mevalout=1. Then the function evaluate() evaluates to \f[ f_1=g(\vec{p}) \f]

            It is also possible to solve data fitting problems of the form
              \f[ \vec{p}^\ast=\min\limits_{\vec{p}}\sum\limits_{m=1}^M\left(\frac{y_m-f(x_m, \vec{p})}{\sigma_m}\right)^2 \f]
            by setting \c Mevalout= \f$ M \f$ where \f$ M \f$ is the number of measurements \f$ (x_m, y_m) \f$ . The method
            evaluate() then returns a vector of size \f$ M \f$ where the m-th entry evluates to
              \f[ f_m=\frac{y_m-f(x_m, \vec{p})}{\sigma_m} \f]
            The summing and squaring has to be done by the optimization algorithm afterwards.


         */
        class Functor {
            public:
                Functor(int Mevalout) { m_evalout=Mevalout; }
                /*! \brief function that evaluates the arbitrary function

                    \param[out] evalout
                    \param params parameter vector
                    \param Nparams number of parameters \f$ N \f$
                 */
                virtual void evaluate(double* evalout, double* params, int Nparams)=0;

                /** \brief return the number of parameters \f$ N \f$ */
                virtual int get_paramcount() const=0;

                /** \brief return dimension of function output vector \f$ M \f$ */
                inline int get_evalout() const { return m_evalout; };

            protected:
                /** \brief size of function output \f$ M \f$ */
                int m_evalout;

                Functor() { m_evalout=1; };
        };

        /** \brief this struct is used to return the fit result */
        struct FitResult {
            /** \brief \c true on success */
            bool fitOK;
            /** \brief additional result parameters of the fit (maxIterations, ...).
             *         These parameters are ment superseed parameters with the same name retrieved with QFFitAlgorithm::getParameter() */
            QMap<QString, QVariant> params;
            /** \brief a report of the fit result in human-readable form, you may use HTML markup to enrich your results */
            QString message;
            /** \brief a report of the fit result in human-readable form, you may NOT use HTML markup to enrich your results */
            QString messageSimple;
        };

        /** \brief class destructor */
        virtual ~QFFitAlgorithm() {}


        /** \brief return a list with the names of all fitting algorithm parameters */
        inline QStringList getParameterIDs() const { return m_parameters.keys(); }

        /** \brief return the given parameter */
        inline QVariant getParameter(QString id, QVariant defaultValue=QVariant()) const {
            if (m_parameters.contains(id)) return m_parameters[id];
            return defaultValue;
        }

        /** \brief set the given parameter */
        inline void setParameter(QString id, QVariant value) {
            m_parameters[id]=value;
        }


        /*! \brief this wrapper routine allows to use the fitting algorithm for 1D data fitting with a given model function \f$ f(x; \vec{p}) \f$ encoded in \a model
                   and a set of measurements \f$ (x_i, y_i, \sigma_i) \f$ where \f$ \sigma_i \f$ are the weights for the measurements. Then this routine solves the problem:
                   \f$ \vec{p}^\ast=\min\limits_{\vec{p}}\sum\limits_{i=1}^M\left(\frac{y_i-f(x_i; \vec{p})}{\sigma_i}\right)^2 \f$

            \param[out] paramsOut The optimal parameter vector is written into this array
            \param[out] paramErrorsOut The optimal parameter error vector is written into this array
            \param dataX \f$ x_i\f$ -value of the data
            \param dataY \f$ y_i\f$ -values of the data
            \param dataWeight weights \f$ w_i\f$
            \param N number of datapoints (i.e. size of the arrays dataX, dataY and dataWeights
            \param model the model function to use
            \param initialParams initial values for the parameters
            \param fixParams which parameters to fix (if \c NULL, no parameters are fixed)
            \param paramsMin lower parameter bound
            \param paramsMax upper parameter bound
            \return a FitResult object describing the fit result
        */
        FitResult fit(double* paramsOut, double* paramErrorsOut, double* dataX, double* dataY, double* dataWeight, uint64_t N, QFFitFunction* model, double* initialParams, bool* fixParams=NULL, double* paramsMin=NULL, double* paramsMax=NULL);

        /*! \brief this wrapper routine allows to use the fitting algorithm for general optimization of a vector valued function \f$ \vec{y}=\vec{f}(\vec{p})\in\mathbf{R}^M, \vec{p}\in\mathbb{R}^N \f$
                   encoded in the parameter \a model. So this solves \f$ \vec{p}^\ast=\min\limits_{\vec{p}}\|\vec{f}(\vec{p})\|^2_2 \f$

            \param[out] paramsOut The optimal parameter vector is written into this array
            \param[out] paramErrorsOut The optimal parameter error vector is written into this array
            \param model the model function to use
            \param initialParams initial values for the parameters
            \param paramsMin lower parameter bound
            \param paramsMax upper parameter bound
            \return a FitResult object describing the fit result
        */
        FitResult fit(double* paramsOut, double* paramErrorsOut, Functor* model, double* initialParams, double* paramsMin=NULL, double* paramsMax=NULL);


        /*! \brief display a modal configuration dialog for you fit algorithm
            \return \c true if the user clicked "OK"

            \note you will also have to implement the writeback of the parameters, using setParameter()
        */
        virtual bool displayConfig();
    protected:
        /** \brief fit algorithm parameters */
        QMap<QString, QVariant> m_parameters;

    protected:
        /*! \brief this routine implements the fitting itself

            \param[out] paramsOut The optimal parameter vector is written into this array
            \param[out] paramErrorsOut The optimal parameter error vector is written into this array
            \param initialParams initial values for the parameters
            \param model the model function to minimize
            \param paramsMin lower parameter bound
            \param paramsMax upper parameter bound
            \return a FitResult object describing the fit result
        */
        virtual FitResult intMinimize(double* paramsOut, double* paramErrorsOut, double* initialParams, Functor* model, double* paramsMin, double* paramsMax)=0;
    public:
        /** \brief return a name for the algorithm */
        virtual QString name() const=0;
        /** \brief return a short unique algorithm ID string */
        virtual QString id() const=0;
        /** \brief return a HTML file to be displayed as algorithm help. This file has to be positioned in \c plugins/fitalgorithms/help/<plugin_id> */
        virtual QString helpFile() const { return QString(""); };
};

#endif // QFFITALGORITHM_H
