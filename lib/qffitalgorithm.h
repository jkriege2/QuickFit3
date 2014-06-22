/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#ifndef QFFITALGORITHM_H
#define QFFITALGORITHM_H

#include "lib_imexport.h"
#include "qffitfunction.h"
#include <QString>
#include <QMap>
#include <QVariant>
#include <stdint.h>
#include "qfrawdatarecord.h"

/*! \brief reporter interface for fitting algorithms
    \ingroup qf3lib_fitting

    This interface may be used to report status information on the ongoing fit to the user:
      - you may report status messages using reportStatus()
      - you may report the progress using setProgressMax() and setProgress(), or incProgress()
      - you may also ask whether the user has canceled the fit procedure using isCanceled()
    .

    An Implementation of a QFFitAlgorithmReporter is dlgQFFitAlgorithmProgressDialog.

*/
class QFLIB_EXPORT QFFitAlgorithmReporter {
    public:
        virtual ~QFFitAlgorithmReporter() {};
        /** \brief report a status message */
        virtual void reportStatus(const QString& message)=0;
        /** \brief set the progress maximum to \a max */
        virtual void setProgressMax(int max=100)=0;
        /** \brief set the current progress to the given value */
        virtual void setProgress(int value)=0;
        /** \brief set the current progress to 100% */
        virtual void setProgressFull()=0;
        /** \brief increment the current progress */
        virtual void incProgress(int increment=1)=0;
        /** \brief return \c true, if the user has canceled the fit procedure */
        virtual bool isCanceled() { return false; };
};

/*! \brief describes a virtual base class fitting algorithms that are applied to QFFitFunction objects.
    \ingroup qf3lib_fitting

    When implementing a fitting algorithm you will have to implement this:
      - implement name(), id()
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

    If you want to execute the fit in a separate thread, you may use QFFitAlgoruthmThreadedFit. You may use
    setReporter() to set a QFFitAlgorithmReporter (e.g. dlgQFFitAlgorithmProgressDialog) implementation used
    to output messages to the user. Use these protected methods to output the message. They will return without
    effect if no reporter is set:
        - reportStatus()
        - setProgressMax()
        - setProgress()
        - incProgress()
    .
    Using the function isCanceled() you my check whether the user has canceled the fit. If you do, you should
    finish the fit immediately when isCanceled() is \c true, No valid results are expected, but you should clean
    up any temporary memory allocated. If you do NOT react in isCanceled(), the fit function won't be stopped,
    so you are guaranteed to finish the function.

*/
class QFLIB_EXPORT QFFitAlgorithm {
    public:
        /*! \brief functor base class that may be used to optimize arbitrary functions

            This functor allows to evaluate arbitrary vector-valued functions
              \f[ \vec{y}=\vec{f}(\vec{p})\in\mathbb{R}^M, \vec{p}\in\mathbb{R}^N. \f]
            For some data fitting/optimization algorithms we also have to know the jacobian of the model function, i.e.
              \f[ J_{m,i}=\frac{\partial y_m}{\partial p_i}\ \ \ \ \ \ \text{with}\ \ \ \ \ \ \vec{y}=\vec{f}(x, \vec{p}) \f]
            This function is implemented in evaluateJacobian(). As it is impossible to calculate the jacobian analytically for
            some functions, or because the implementor didn't do it, there is the function get_implementsJacobian() which tells
            whether the jacobian was implemented or not.

         */
        class QFLIB_EXPORT Functor {
            public:
                Functor(int Mevalout) { m_evalout=Mevalout; }

                virtual  ~Functor() {};

                /*! \brief function that evaluates the arbitrary function

                    \param[out] evalout with size get_evalout()
                    \param params parameter vector with size get_paramcount()
                 */
                virtual void evaluate(double* evalout, const double* params)=0;

                /*! \brief function that evaluates the arbitrary function

                    \param[out] evalout with size get_evalout()*get_paramcount() in the order
                                \f$ \left[ \frac{\partial f_1}{\partial p_1}, \frac{\partial f_1}{\partial p_2}, ..., \frac{\partial f_1}{\partial p_N}, \frac{\partial f_2}{\partial p_1}, \frac{\partial f_2}{\partial p_2}, ..., \frac{\partial f_2}{\partial p_N}, ..., \frac{\partial f_M}{\partial p_N} \right] \f$
                    \param params parameter vector with size get_paramcount()

                    \note This is only implemented if get_implementsJacobian() returns true
                 */
                virtual void evaluateJacobian(double* evalout, const double* params);

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


        /*! \brief base class for QFFitAlgorthm::Functor classes that let the user fit a dataset \f$(x_i,y_i,\sigma_i)\f$ to a 1D-function

                \f[ g_m(\vec{p})=\frac{y_m-f(x_m; m(\vec{p}))}{\sigma_m} \f]

            This class does not assume anything about the function \f$f(\cdot)\f$. it's implementatoin
            is left to child classes of this virtual class (see e.g. QFFitAlgorithm::FitQFFitFunctionFunctor ).
        */
        class QFLIB_EXPORT FitFunctionFunctor: public Functor {
            public:
                /*! \brief constructor, initializes the functor
                    \param dataX the x-values data vector \f$ x_m \f$
                    \param dataY the y-values data vector \f$ y_m \f$
                    \param dataWeights the weight vector \f$ \sigma_m \f$
                    \param M number of datapoints
                */
                FitFunctionFunctor(const double* dataX, const double* dataY, const double* dataWeight, uint64_t M) ;

                virtual ~FitFunctionFunctor();

                const double* getDataX() const;
                const double* getDataY() const;
                const double* getDataWeight() const;
                uint64_t getDataPoints() const;

                void setDataX(const double* data);
                void setDataY(const double* data);
                void setDataWeight(const double* data);

            protected:

                /** \brief the x-values data vector \f$ x_m \f$ */
                const double* m_dataX;
                /** \brief the y-values data vector \f$ y_m \f$ */
                const double* m_dataY;
                /** \brief the weight vector \f$ \sigma_m \f$ */
                const double* m_dataWeight;
                /** \brief number of datapoints      */
                uint64_t m_M;
        };




        /*! \brief adaptor class that uses a FitFunctionFunctor to implement iteratively reweighted least squares

            \note The constructor will overwrite the data weights of the functor, given as a parameter. The destructor will
                  reset them to the original values, so make sure that the given functor exists throughout the lifetime of
                  any coupled IRLSFunctorAdaptor object!

            This adaptor creates an internal vector of weightes, that it will use to overwrite any weights given to the
            functor implementing the actual fit problem. Initially all datapoints have the same weight, but they will get
            reweighted according to the IRLS algorithm on every call of the method irlsReweight(). The algorithm works
            like this:


            This is a simple form of the IRLS algorithm to estimate the parameters \f$\vec{p}\f$ in an arbitrary 1D fit model \f$ f(x;\vec{p}) \f$.
            This algorithm solves the optimization problem for a \f$ L_p\f$-norm:
              \f[ \vec{p}^\ast=\argmin\limits_{\vec{p}}\sum\limits_i|f(x_i;\vec{p})-y_i|^p \f]
            by iteratively optimization weights \f$ \vec{w} \f$ and solving a weighted least squares problem in each iteration:
              \f[ \vec{p}=\argmin\limits_{\vec{p}}\sum\limits_i|f(x_i;\vec{p})-y_i|^{(p-2)}\cdot|f(x_i;\vec{p})-y_i|^2 \f]


            The algoruithms works as follows:
              - calculate initial \f$ \vec{p}_0\f$ with an unweighted fit
              - perform a number of iterations (parameter \a iterations ). In each iteration \f$ n\f$:
                  - calculate the error vector \f$\vec{e}\f$: \f[ e_i = f(x_i;\vec{p}) -y_i \f]
                  - estimate new weights \f$\vec{w}\f$: \[ w_i=|e_i|^{(p-2)/2} \]
                  - calculate new estimates \f$ \vec{p}_n \f$ with a weighted least-squares fit, from \f$ \vec{x}\f$ and \f$ \vec{y}\f$ and \f$ \vec{w}\f$
                .
              - return the last estimates \f$ \vec{p}_n\f$
            .

            \see C. Sidney Burrus: "Iterative Reweighted Least Squares", <a href="http://cnx.org/content/m45285/latest/">http://cnx.org/content/m45285/latest/</a>

        */
        class QFLIB_EXPORT IRLSFunctorAdaptor: public Functor {
            public:
                /*! \brief constructor, initializes the functor
                */
                IRLSFunctorAdaptor(FitFunctionFunctor* functor, double irls_parameter=1.1) ;

                virtual ~IRLSFunctorAdaptor() ;


                /*! \brief function that evaluates the arbitrary function

                    \param[out] evalout with size get_evalout()
                    \param params parameter vector with size get_paramcount()
                 */
                virtual void evaluate(double* evalout, const double* params);

                /*! \brief function that evaluates the arbitrary function

                    \param[out] evalout with size get_evalout()*get_paramcount() in the order
                                \f$ \left[ \frac{\partial f_1}{\partial p_1}, \frac{\partial f_1}{\partial p_2}, ..., \frac{\partial f_1}{\partial p_N}, \frac{\partial f_2}{\partial p_1}, \frac{\partial f_2}{\partial p_2}, ..., \frac{\partial f_2}{\partial p_N}, ..., \frac{\partial f_M}{\partial p_N} \right] \f$
                    \param params parameter vector with size get_paramcount()

                    \note This is only implemented if get_implementsJacobian() returns true
                 */
                virtual void evaluateJacobian(double* evalout, const double* params);

                /** \brief return the number of parameters \f$ N \f$ */
                virtual int get_paramcount() const;

                /** \brief return \c true if the jacobian is implemented */
                virtual bool get_implementsJacobian() const;



                void irlsReweight(const double* params);

            protected:

                /** \brief IRLS parameter */
                double irls_parameter;

                /** \brief functor used to evaluate the fit problem */
                FitFunctionFunctor* irls_functor;

                double* irls_weights;

                const double* oldWeights;
        };


        /*! \brief this is a special functor which is used for data fitting to objective functions declared by QFFitFunction

            The QFFitFunction declares a 1D function \f$ f(x; \vec{p}), \vec{p}\in\mathbb{R}^N \f$ . This functor is provided with a set of data \f$ (x_m, y_m, \sigma_m), m=1..M \f$
            where \f$ \sigma_m \f$ are weights for the data. Then This Functor implementation maps the 1D Function \f$ f(x; \vec{p}) \f$ to a \f$ M \f$ dimensional
            function \f$ \vec{g}(\vec{p}) \f$ with:
                \f[ g_m(\vec{p})=\frac{y_m-f(x_m; m(\vec{p}))}{\sigma_m} \f]
            As QFFitFunction may contain parameters \f$ p_n, n=1..N \f$ that are no fitting parameters (either because they are defined as such, or because they were fixed
            by the user, or they are currently hidden) we have to do parameter reordering. So the fitting algorithm is presented with a modified parameter vector
            \f$ \vec{q}\in\mathbb{R}^Q \f$ with \f$ Q<N \f$ . The mapping is done using a function \f$ m(\cdot) \f$ with \f$ \vec{p}=m(\vec{q})\in\mathbb{R}^N \f$ and
            \f$ \vec{q}=m^{-1}(\vec{p})\in\mathbb{R}^Q \f$ which is defined below, so this Functor finally calculates:
                \f[ g_m(\vec{q})=\frac{y_m-f(x_m; m(\vec{q}))}{\sigma_m} \f]
            The data, the weights and the fix-vector is given to this functor in the constructor. Afterwards you may call evaluate() to calculate \f$ \vec{g}(\vec{q}) \f$ .

            The functor is easier explained by this image:
            \image html FitQFFunctionFunctor.png
            \image latex FitQFFunctionFunctor.png
        */
        class QFLIB_EXPORT FitQFFitFunctionFunctor: public FitFunctionFunctor {
            public:
                /*! \brief constructor, initializes the functor
                    \param model QFFitFunction object used to evaluate \f$ f(x; \vec{p}) \f$
                    \param currentParams the initial parameters. You will have to give the values of ALL non-fit parameters in this vector!
                    \param fixParams \c true if a parameter is fixed by the user
                    \param dataX the x-values data vector \f$ x_m \f$
                    \param dataY the y-values data vector \f$ y_m \f$
                    \param dataWeights the weight vector \f$ \sigma_m \f$
                    \param M number of datapoints
                */
                FitQFFitFunctionFunctor(QFFitFunction* model, const double* currentParams, const bool* fixParams, const double* dataX, const double* dataY, const double* dataWeight, uint64_t M) ;

                virtual ~FitQFFitFunctionFunctor();

                /*! \brief Implements the inverse mapping function \f$ \vec{q}=m^{-1}(\vec{p})\in\mathbb{R}^Q \f$ where \f$ \vec{p} \f$ is given by \a modelData.
                           The result is a NEW array created by calling \c calloc()
                */
                double* createMappedArrayForFunctor(const double* modelData);

                /*! \brief Implements the inverse mapping function \f$ \vec{q}=m^{-1}(\vec{p})\in\mathbb{R}^Q \f$ where \f$ \vec{p} \f$ is given by \a modelData.
                           This function only copies those entries that are present in \a functorData.
                */
                void mapArrayFromModelToFunctor(double* functorData, const double* modelData);

                /*! \brief Implements the mapping function \f$ \vec{p}=m(\vec{q})\in\mathbb{R}^N \f$ where \f$ \vec{q} \f$ is given by \a functorData.
                           and \f$ \vec{p} \f$ is returned in \a modelData. This function only overwrites the entries that are present in \a functorData.
                */
                void mapArrayFromFunctorToModel(double* modelData, const double* functorData);

                /*! \brief Implements the mapping function \f$ \vec{p}=m(\vec{q})\in\mathbb{R}^N \f$
                */
                inline int mapFromFunctorToModel(int functorIndex){
                    return modelFromFunctor[functorIndex];
                }

                /** \brief evaluate the function \f$ \vec{g}(\vec{q}) \f$ */
                virtual void evaluate(double* evalout, const double* params);

                /** \brief evaluate the functions jacobian \f$ J_{n,m}(\vec{q})=\frac{\partial g_m(\vec{q})}{\partial q_n}=-\frac{1}{\sigma_m}\cdot\frac{\partial f(x_m, m(\vec{q}))}{\partial m(q_n)} \f$ */
                virtual void evaluateJacobian(double* evalout, const double* params);

                /** \brief returns \c true if the model implements its jacobian analytically and therefore evaluateJacobian() may be used */
                virtual bool get_implementsJacobian() const { return m_model->get_implementsDerivatives(); }

                /** \brief return the number of parameters \f$ Q \f$ in \f$ \vec{q}=m(\vec{p}) \f$ */
                virtual int get_paramcount() const { return m_paramCount; }

                /** \brief return the used QFFitFunction */
                QFFitFunction* getModel() const { return m_model; }

                /** \brief return a pointer to the stored initial fit parameters */
                double* getModelParams() const { return m_modelParams; }
                /** \brief return a pointer to an array indicating which parameters are fixed */
                bool* getModelParamsFix() const { return m_modelparamsFix; }
                /** \brief return the number of to the stored initial fit parameters */
                int getModelParamsCount() const { return m_model->paramCount(); }
            protected:


                /** \brief  QFFitFunction object used to evaluate \f$ f(x; \vec{p}) \f$ */
                QFFitFunction* m_model;
                /** \brief number of parameters in m_model */
                int m_N;
                /** \brief number of real (non-fixed) parameters, \c m_paramCount<=m_N */
                int m_paramCount;
                /** \brief maps from function parameter index to model parameter index (size m_N) */
                int* functorFromModel;
                /** \brief maps from functor parameter index to model parameter index (size m_paramCount) */
                int* modelFromFunctor;
                /** \brief copy of the current model parameter vector (size m_N) */
                double* m_modelParams;
                /** \brief vector containing which parameters are fixed */
                bool* m_modelparamsFix;
        };



        /*! \brief this is a special functor which is used for data fitting to objective functions declared by QFFitFunction

            The Functor declares a nD function \f$ \vec{f}(\vec{p}), \vec{p}\in\mathbb{R}^N \f$ .

            As Functor may contain parameters \f$ p_n, n=1..N \f$ that are no fitting parameters (e.g. because they are fixed) we have to do parameter reordering.
            So the fitting algorithm is presented with a modified parameter vector \f$ \vec{q}\in\mathbb{R}^Q \f$ with \f$ Q<N \f$ . The mapping is done using a
             function \f$ m(\cdot) \f$ with \f$ \vec{p}=m(\vec{q})\in\mathbb{R}^N \f$ and
            \f$ \vec{q}=m^{-1}(\vec{p})\in\mathbb{R}^Q \f$ which is defined below, so this Functor finally calculates:
                \f[ g_m(\vec{q})=\frac{y_m-f(x_m; m(\vec{q}))}{\sigma_m} \f]
            Afterwards you may call evaluate() to calculate \f$ \vec{g}(\vec{q}) \f$ .
        */
        class QFLIB_EXPORT FitQFOptimizeFunctionFunctor: public Functor {
            public:
                /*! \brief constructor, initializes the functor
                    \param model Functor object used to evaluate \f$ f(x; \vec{p}) \f$
                    \param params parameter count for model
                    \param currentParams the initial parameters. You will have to give the values of ALL non-fit parameters in this vector!
                    \param fixParams \c true if a parameter is fixed by the user
                */
                FitQFOptimizeFunctionFunctor(Functor* model, const double* currentParams, const bool* fixParams) ;

                ~FitQFOptimizeFunctionFunctor();

                /*! \brief Implements the inverse mapping function \f$ \vec{q}=m^{-1}(\vec{p})\in\mathbb{R}^Q \f$ where \f$ \vec{p} \f$ is given by \a modelData.
                           The result is a NEW array created by calling \c calloc()
                */
                double* createMappedArrayForFunctor(const double* modelData);

                /*! \brief Implements the inverse mapping function \f$ \vec{q}=m^{-1}(\vec{p})\in\mathbb{R}^Q \f$ where \f$ \vec{p} \f$ is given by \a modelData.
                           This function only copies those entries that are present in \a functorData.
                */
                void mapArrayFromModelToFunctor(double* functorData, const double* modelData);

                /*! \brief Implements the mapping function \f$ \vec{p}=m(\vec{q})\in\mathbb{R}^N \f$ where \f$ \vec{q} \f$ is given by \a functorData.
                           and \f$ \vec{p} \f$ is returned in \a modelData. This function only overwrites the entries that are present in \a functorData.
                */
                void mapArrayFromFunctorToModel(double* modelData, const double* functorData);

                /** \brief evaluate the function \f$ \vec{g}(\vec{q}) \f$ */
                virtual void evaluate(double* evalout, const double* params);

                /** \brief evaluate the functions jacobian \f$ J_{n,m}(\vec{q})=\frac{\partial g_m(\vec{q})}{\partial q_n}=-\frac{1}{\sigma_m}\cdot\frac{\partial f(x_m, m(\vec{q}))}{\partial m(q_n)} \f$ */
                virtual void evaluateJacobian(double* evalout, const double* params);

                /** \brief returns \c true if the model implements its jacobian analytically and therefore evaluateJacobian() may be used */
                virtual bool get_implementsJacobian() const { return m_model->get_implementsJacobian(); }

                /** \brief return the number of parameters \f$ Q \f$ in \f$ \vec{q}=m(\vec{p}) \f$ */
                virtual int get_paramcount() const { return m_paramCount; }
            protected:


                /** \brief  QFFitFunction object used to evaluate \f$ f(x; \vec{p}) \f$ */
                Functor* m_model;
                /** \brief number of parameters in m_model */
                int m_N;
                /** \brief number of real (non-fixed) parameters, \c m_paramCount<=m_N */
                int m_paramCount;
                /** \brief maps from function parameter index to model parameter index (size m_N) */
                int* functorFromModel;
                /** \brief maps from functor parameter index to model parameter index (size m_paramCount) */
                int* modelFromFunctor;
                /** \brief copy of the current model parameter vector (size m_N) */
                double* m_modelParams;
        };


        /** \brief this struct is used to return the fit result */
        struct QFLIB_EXPORT FitResult {
            /** \brief \c true on success */
            bool fitOK;
            /** \brief additional result parameters of the fit (maxIterations, ...).
             *         These parameters are ment superseed parameters with the same name retrieved with QFFitAlgorithm::getParameter() */
            QMap<QString, QFRawDataRecord::evaluationResult> params;
            /** \brief a report of the fit result in human-readable form, you may use HTML markup to enrich your results */
            QString message;
            /** \brief a report of the fit result in human-readable form, you may NOT use HTML markup to enrich your results */
            QString messageSimple;


            /** \brief set a result of type number */
            void addNumber(QString resultName, double value, QString unit=QString(""));
            /** \brief set a result of type number vector */
            void addNumberList(QString resultName, QVector<double>& value, QString unit=QString(""));
            /** \brief set a result of type number matrix */
            void addNumberMatrix(QString resultName, QVector<double>& value, int columns, QString unit=QString(""));
            /** \brief set a result of type number vector */
            void addNumberList(QString resultName, double* value, int items, QString unit=QString(""));
            /** \brief set a result of type number matrix */
            void addNumberMatrix(QString resultName, double* value, int columns, int rows, QString unit=QString(""));
            /** \brief set a result of type number+error */
            void addNumberError(QString resultName, double value, double error, QString unit=QString(""));
            /** \brief set a result of type integer */
            void addInteger(QString resultName, int64_t value, QString unit=QString(""));
            /** \brief set a result of type string */
            void addString(QString resultName, QString value);
            /** \brief set a result of type boolean */
            void addBoolean(QString resultName, bool value);
            /** \brief return result as string */
            QString  getAsString(QString resultName);
        };

        /** \brief class construtor */
        QFFitAlgorithm() { m_reporter=NULL; }

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
        FitResult fit(double* paramsOut, double* paramErrorsOut, const double* dataX, const double* dataY, const double* dataWeight, uint64_t N, QFFitFunction* model, const double* initialParams, const bool* fixParams=NULL, const double* paramsMin=NULL, const double* paramsMax=NULL);


        /*! \brief this wrapper routine allows to use the fitting algorithm for a general optimization problem with a given model function \f$ f(\vec{p}) \f$ encoded in \a model.
                   This routine solves the problem:
                   \f$ \vec{p}^\ast=\min\limits_{\vec{p}}\|\vec{f}(\vec{p})\| \f$

            \param[out] paramsOut The optimal parameter vector is written into this array
            \param[out] paramErrorsOut The optimal parameter error vector is written into this array
            \param N number of datapoints (i.e. size of the arrays dataX, dataY and dataWeights
            \param model the model function to use
            \param initialParams initial values for the parameters
            \param fixParams which parameters to fix (if \c NULL, no parameters are fixed)
            \param paramsMin lower parameter bound
            \param paramsMax upper parameter bound
            \return a FitResult object describing the fit result
        */
        FitResult optimize(double* paramsOut, double* paramErrorsOut, Functor* model, const double* initialParams, const bool* fixParams=NULL, const double* paramsMin=NULL, const double* paramsMax=NULL);


        /*! \brief display a modal configuration dialog for you fit algorithm
            \return \c true if the user clicked "OK"

            \note you will also have to implement the writeback of the parameters, using setParameter()
        */
        virtual bool displayConfig();

        /** \brief tell the class to use the specified reporter widget */
        void setReporter(QFFitAlgorithmReporter* reporter) {
            m_reporter=reporter;
        }

        /** \brief return the current reporter */
        QFFitAlgorithmReporter* reporter() { return m_reporter; }
    protected:
        /** \brief fit algorithm parameters */
        QMap<QString, QVariant> m_parameters;

        /** \brief reporter object */
        QFFitAlgorithmReporter* m_reporter;


        /** \brief report a status message */
        void reportStatus(const QString& message) { if (m_reporter) m_reporter->reportStatus(message); }
        /** \brief set the progress maximum to \a max */
        void setProgressMax(int max=100) { if (m_reporter) m_reporter->setProgressMax(max); }
        /** \brief set the current progress to the given value */
        void setProgress(int value) { if (m_reporter) m_reporter->setProgress(value); }
        /** \brief set the current progress to the given value */
        void setProgressFull() { if (m_reporter) m_reporter->setProgressFull(); }
        /** \brief increment the current progress */
        void incProgress(int increment=1) { if (m_reporter) m_reporter->incProgress(increment); }
        /** \brief return \c true, if the user has canceled the fit procedure */
        bool isCanceled() { if (m_reporter) return m_reporter->isCanceled(); else return false; }

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
            by the fitting algorithm. Note that this function is called for data fitting problems  (fit() method) with an internal
            implementation of Functor which evaluates to \f$ f_m=\frac{y_m-f_m(x_m, \vec{p})}{\sigma_m} \f$ where
            \f$ (x_m, y_m, \sigma_m) \f$ is a measurement vector with weights \f$ \sigma_m \f$ .
        */
        virtual FitResult intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, Functor* model, const double* paramsMin, const double* paramsMax)=0;
    public:
        /** \brief return a name for the algorithm */
        virtual QString name() const=0;
        /** \brief return a short name for the algorithm (max. around 10-20 characters) */
        virtual QString shortName() const { return name(); }
        /** \brief return a short unique algorithm ID string */
        virtual QString id() const=0;

        /** \brief \c true if the algorithm supports bounded optimization with box constraints and \c false else */
        virtual bool get_supportsBoxConstraints() const =0;
        /** \brief \c true if the algorithm may be used in a multi-threaded environment, i.e. it is threadsafe.
         *
         *  QuickFit will never use the same instance of this class from multiple threads at the same time. Nevertheless it is
         *  possible, that your fit library uses some global datastructures that are the same for all threads. These might
         *  still render it improssible to use a fit library in a multi-threaded way!
         *
         *  \note better return \c false if you are unsure!
         */
        virtual bool isThreadSafe() const =0;


};

#endif // QFFITALGORITHM_H
