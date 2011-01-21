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
          \f[ \vec{p}^\ast=\argmin\limits_{\vec{p}}f(\vec{p}) \f]
        The function \f$ f(\vec{p}) \f$ is implemented as a QFFitAlgorithm::Functor object.
      - optionally you may overwrite displayConfig() which displays a modal configuration dialog for your fit algorithm
    .
*/
class QFFitAlgorithm {
    public:
        /** \brief functor base class that may be used to optimize arbitrary functions */
        struct Functor {
            /** \brief function that evaluates the arbitrary function
             *  \param params parameter vector
             *  \param N number of parameters
             */
            virtual double evaluate(double* params, int N)=0;
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


        /*! \brief this wrapper routine allows to use the fitting algorithm for 1D data fitting

            \param[out] paramsOut The optimal parameter vector is written into this array
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
        FitResult fit(double* paramsOut, double* dataX, double* dataY, double* dataWeight, uint64_t N, QFFitFunction* model, double* initialParams, bool* fixParams=NULL, double* paramsMin=NULL, double* paramsMax=NULL);
        /*! \brief this wrapper routine allows to use the fitting algorithm for 1D data fitting

            \param[out] paramsOut The optimal parameter vector is written into this array
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
        FitResult fit(double* paramsOut, double* dataX, double* dataY, double* dataWeight, uint64_t N, Functor* model, double* initialParams, bool* fixParams=NULL, double* paramsMin=NULL, double* paramsMax=NULL);


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
            \param initialParams initial values for the parameters
            \param model the model function to minimize
            \param paramsMin lower parameter bound
            \param paramsMax upper parameter bound
            \param paramCount number of parameters
            \return a FitResult object describing the fit result
        */
        virtual FitResult intMinimize(double* paramsOut, double* initialParams, Functor* model, double* paramsMin, double* paramsMax, int paramCount)=0;
    public:
        /** \brief return a name for the algorithm */
        virtual QString name() const=0;
        /** \brief return a short unique algorithm ID string */
        virtual QString id() const=0;
        /** \brief return a HTML file to be displayed as algorithm help. This file has to be positioned in \c plugins/fitalgorithms/help/<plugin_id> */
        virtual QString helpFile() const { return QString(""); };
};

#endif // QFFITALGORITHM_H
