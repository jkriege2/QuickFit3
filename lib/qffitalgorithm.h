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
        later on you can read it back using getParameter(). Note that is is possible that the main application
        overwrites these parameters by calling setParameter() with externally defined values (e.g. from an INI
        file). So if you want really private parameters, used private/protected variables for them.
      - in the protected function intFit() you will have to implement your fitting algorithm in a form which
        minimizes a vector values function \f$ \vec{f}(\vec{p})\in\mathbb{R}^M \f$ of a set of parameters
        \f$ \vec{p}\in\mathbb{R}^N \f$ with an arbitrary norm (usually \f$ L^2 \f$ ):
          \f[ \vec{p}^\ast=\min\limits_{\vec{p}}\|\vec{f}(\vec{p})\| \f]
        The function \f$ \vec{f}(\vec{p}) \f$ is implemented as a QFFitAlgorithm::Functor object.
      - optionally you may overwrite displayConfig() which displays a modal configuration dialog for your fit algorithm
      - use get_supportsBoxConstraints() to tell the user/program whether the fitting algorithm supports box constraints or not.
    .


    This class provides interfaces to data fitting and optimization:
      - fit() is used to solve a data fitting problem with an objective function provided by a QFFitFunction and a set of measurements
        \f$ (x_i,y_i)_{i=1..M} \f$ :
            \f[ \vec{p}^\ast=\min\limits_{\vec{p}}\sum\limits_{m=1}^M\left\|\frac{y_m-f_m(x_m, \vec{p})}{\sigma_m}\right\| \f]
    .
    All interfaces are internally mapped to the protected function intFit().

*/
class QFFitAlgorithm {
    public:
        /*! \brief functor base class that may be used to optimize arbitrary functions

            This functor allows to evaluate arbitrary vector-valued functions
              \f[ \vec{y}=\vec{f}(\vec{p})\in\mathbf{R}^M, \vec{p}\in\mathbb{R}^N. \f]
            For some data fitting/optimization algorithms we also have to know the jacobian of the model function, i.e.
              \f[ J_{m,i}=\frac{\partial y_m}{\partial p_i}\ \ \ \ \ \ \text{with}\ \ \ \ \ \ \vec{y}=\vec{f}(x, \vec{p}) \f]
            This function is implemented in evaluateJacobian(). As it is impossible to calculate the jacobian analytically for
            some functions, or because the implementor didn't do it, there is the function get_implementsJacobian() which tells
            whether the jacobian was implemented or not.

         */
        class Functor {
            public:
                Functor(int Mevalout) { m_evalout=Mevalout; }
                /*! \brief function that evaluates the arbitrary function

                    \param[out] evalout with size get_evalout()
                    \param params parameter vector with size get_paramcount()
                 */
                virtual void evaluate(double* evalout, double* params)=0;

                /*! \brief function that evaluates the arbitrary function

                    \param[out] evalout with size get_evalout()*get_paramcount() in the order
                                \f$ \left[ \frac{\partial f_1}{\partial p_1}, \frac{\partial f_1}{\partial p_2}, ..., \frac{\partial f_1}{\partial p_N}, \frac{\partial f_2}{\partial p_1}, \frac{\partial f_2}{\partial p_2}, ..., \frac{\partial f_2}{\partial p_N}, ..., \frac{\partial f_M}{\partial p_N} \right] \f$
                    \param params parameter vector with size get_paramcount()

                    \note This is only implemented if get_implementsJacobian() returns true
                 */
                virtual void evaluateJacobian(double* evalout, double* params);

                /** \brief return the number of parameters \f$ N \f$ */
                virtual int get_paramcount() const=0;

                /** \brief return \c true if the jacobian is implemented */
                virtual bool get_implementsJacobian() const { return false; };

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
                   \f$ \vec{p}^\ast=\min\limits_{\vec{p}}\sum\limits_{i=1}^M\left\|\frac{y_i-f(x_i; \vec{p})}{\sigma_i}\right\| \f$

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


            This method uses the nested Functor class to solve a problem of the form
                \f[ \vec{p}^\ast=\min\limits_{\vec{p}}\|\vec{f}(\vec{p})\| \f]
            The norm \f$ \|\cdot\| \f$ is usually the \f$ L^2 \f$ norm, but may be any other norm defined
            by the fitting algorithm. Note that this function is called for data fitting problems with an internal
            implementation of Functor which evaluates to \f$ f_m=\frac{y_m-f_m(x_m, \vec{p})}{\sigma_m} \f$ where
            \f$ (x_m, y_m, \sigma_m) \f$ is a measurement vector with weights \f$ \sigma_m \f$ .
        */
        virtual FitResult intFit(double* paramsOut, double* paramErrorsOut, double* initialParams, Functor* model, double* paramsMin, double* paramsMax)=0;
    public:
        /** \brief return a name for the algorithm */
        virtual QString name() const=0;
        /** \brief return a short unique algorithm ID string */
        virtual QString id() const=0;
        /** \brief return a HTML file to be displayed as algorithm help. This file has to be positioned in \c plugins/fitalgorithms/help/<plugin_id> */
        virtual QString helpFile() const { return QString(""); };
        /** \brief \c true if the algorithm supports bounded optimization with box constraints and \c false else */
        virtual bool get_supportsBoxConstraints() const =0;
};

#endif // QFFITALGORITHM_H
