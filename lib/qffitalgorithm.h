/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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

#ifndef QFFITALGORITHM_H
#define QFFITALGORITHM_H

#include "lib_imexport.h"
#include "qffitfunction.h"
#include <QString>
#include <QMap>
#include <QVariant>
#include <stdint.h>
#include "qfrawdatarecord.h"
#include "qfproperties.h"
#include "qffitfunctionnd.h"
#include "qfmathtools.h"

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

/*! \brief describes a virtual base class for (least-squares) fitting algorithms that are applied to QFFitFunction objects. It also supports an interface for more general minimization algorithms.
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
      - QFFitAlgorithm::fit() is used to solve a data fitting problem with an objective function provided by a QFFitFunction and a set of measurements
        \f$ (x_i,y_i)_{i=1..M} \f$ :
            \f[ \vec{p}^\ast=\min\limits_{\vec{p}}\sum\limits_{m=1}^M\left\|\frac{y_m-f_m(x_m, \vec{p})}{\sigma_m}\right\| \f]
      - QFFitAlgorithm::fit2D() is used to solve a 2D data fitting problem with an objective function provided by a QFFitFunction2D and a set of measurements
        \f$ (x_i,y_i,f_i)_{i=1..M} \f$ :
            \f[ \vec{p}^\ast=\min\limits_{\vec{p}}\sum\limits_{m=1}^M\left\|\frac{f_m-f_m(x_m,y_m, \vec{p})}{\sigma_m}\right\| \f]
      - QFFitAlgorithm::fit3D() is used to solve a 3D data fitting problem with an objective function provided by a QFFitFunction3D and a set of measurements
        \f$ (x_i,y_i,z_i,f_i)_{i=1..M} \f$ :
            \f[ \vec{p}^\ast=\min\limits_{\vec{p}}\sum\limits_{m=1}^M\left\|\frac{f_m-f_m(x_m,y_m,z_m, \vec{p})}{\sigma_m}\right\| \f]
      - QFFitAlgorithm::lsqMinimize() is used to minimize the norm (usually L2, i.e. least-squares) of a vector-values objective function \f$ \vec{f}(\vec{p})\f$ :
            \f[ \vec{p}^\ast=\min\limits_{\vec{p}}\left\|\vec{f}(\vec{p})\right\|\underset{=}{L^2\text{-norm}}\min\limits_{\vec{p}}\sum\limits_{m=1}^M \left|f_m(\vec{p})\right|^2 \f]
        The model function is implemented by a class of basetype QFFitAlgorithm::Functor.
    .
    All these interfaces are internally mapped to the protected function intFit().

    In addition, this class allows to also represent general minimization algorithms. This can be useful if you use a general minimization
    algorithm for least-squares fitting. Such an algorithm solves the problem:
      \f[ \vec{p}^\ast=\min\limits_{\vec{p}}f(\vec{p}) \f]
    If you algorithm may be used in this way, you can also implement the function intMinimize() and get_implementsMinimize(). The latter
    function indicates whether an implementation of intMinimize() exists and if so should return \c true (the default implementation
    returns \c false ). The 1-dimensional objective function \f$ f(\vec{p}) \f$ has to be implemented as a subclass of QFFitAlgorithm::FunctorForFMin.

    This class provides the following high-level functions to perform minimization:
      - fmin() acepts the objective function as a pointer to QFFitAlgorithm::FunctorForFMin
    .

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
        /*! \brief an interface, which may be used by Functor derived classes to implement bootstrapping

            This function implements a mode for bootstrapping during the fit. In this mode, not all \f$ N \f$ data in  \f$(x_m,y_m,\sigma_m)\f$ are used for the fit,
            but only a subset of \f$ \mbox{bootstrapFraction}\cdot N \f$ datapoints, which are chosen randomly from the provided input data on every call of
            prepareBootstrapSelection(). This feature can be used to implement e.g. error estimates by bootstrapping of the fit input data vector, i.e.
            repeat the fit \f$ M \f$ times with each time a new subset of data and the calculate the standard deviation of the fit values.

            This class manages the data internally and the protected members m_dataX, m_dataY and m_dataWeight are redirected, as required.
            they are also returned by getDatX(), getDataY(), getDataWeight(). The same is done for m_M and getDataPoints().
            These arrays either point to the arrays, provided in the constructor or by the \c setData...() methods, if bootstrapping is
            disabled, or they point to internal vectors, which contain the currently selected subset of the dataset.

         */
        class QFLIB_EXPORT FunctorBootstrapInterface {
            public:
                inline virtual ~FunctorBootstrapInterface() {}
                /** \brief prepares a new selection of data for bootstrapping
                 *
                 * \note if you only want to reapply the current selection and not select new data, call reapplyBootstrapselection() instead.
                 */
                virtual void prepareBootstrapSelection()=0;
                /** \brief this function reapplies the current bootstrappig selection, i.e. if the input-data changed, but not its number and also the selection should not be recreated.
                 *
                 * \note to create a new subset from the input data, call prepareBootstrapSelection()
                 */
                virtual void reapplyBootstrapselection()=0;
                /** \brief switches bootstrapping on and off, if \c enabled=true and \c prepBootstrapping=true, the function prepareBootstrapSelection() is called */
                virtual void setBootstrappingEnabled(bool enabled, bool prepBootstrapping=true)=0;
                /** \brief sets the fraction of the datapoints, that are selected by prepareBootstrapSelection(), if \c bootstrappingEnabled=true and \c prepBootstrapping=true, the function prepareBootstrapSelection() is called */
                virtual void setBootstrappingFraction(double fraction, bool prepBootstrapping=true)=0;
        };

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
            explicit Functor(int Mevalout) { m_evalout=Mevalout; }

            inline virtual  ~Functor() {}

            /*! \brief function that evaluates the arbitrary function

                \param[out] evalout with size get_evalout()
                \param params parameter vector with size get_paramcount()
             */
            virtual void evaluate(double* evalout, const double* params) const=0;

            /*! \brief function that evaluates the first derivatives \f$ J_{m,i}=\frac{\partial y_m}{\partial p_i}\ \ \ \ \ \ \text{with}\ \ \ \ \ \ \vec{y}=\vec{f}(x, \vec{p}) \f$ of the functor function

                \param[out] evalout with size get_evalout()*get_paramcount() in the order
                            \f$ \left[ \frac{\partial f_1}{\partial p_1}, \frac{\partial f_1}{\partial p_2}, ..., \frac{\partial f_1}{\partial p_N}, \frac{\partial f_2}{\partial p_1}, \frac{\partial f_2}{\partial p_2}, ..., \frac{\partial f_2}{\partial p_N}, ..., \frac{\partial f_M}{\partial p_N} \right] \f$
                \param params parameter vector with size get_paramcount()

                \note This is only implemented if get_implementsJacobian() returns true
             */
            virtual void evaluateJacobian(double* evalout, const double* params) const;

            /** \brief return the number of parameters \f$ N \f$ */
            virtual int get_paramcount() const=0;

            /** \brief return \c true if the jacobian is implemented */
            inline virtual bool get_implementsJacobian() const { return false; }

            /** \brief return \c true if functor implements a weighted least-squares problem */
            inline virtual bool isWeightedLSQ() const { return false; }

            /** \brief return \c true if functor implements a weighted least-squares problem and all weights are equal to 1 */
            inline virtual bool areAllWeightsOne() const { return false; }

            /** \brief return dimension of function output vector \f$ M \f$ */
            inline uint64_t get_evalout() const { return m_evalout; }

            /*! \brief  function that evaluates the first derivatives \f$ J_{m,i}=\frac{\partial y_m}{\partial p_i}\ \ \ \ \ \ \text{with}\ \ \ \ \ \ \vec{y}=\vec{f}(x, \vec{p}) \f$ of the functor function numerically, using the 5-point stencil method

                \param[out] evalout with size get_evalout()*get_paramcount() in the order
                            \f$ \left[ \frac{\partial f_1}{\partial p_1}, \frac{\partial f_1}{\partial p_2}, ..., \frac{\partial f_1}{\partial p_N}, \frac{\partial f_2}{\partial p_1}, \frac{\partial f_2}{\partial p_2}, ..., \frac{\partial f_2}{\partial p_N}, ..., \frac{\partial f_M}{\partial p_N} \right] \f$
                \param params parameter vector with size get_paramcount()

                \note If you derive a class from this class Functor, which should be used for weighted least-squares, you should also overwrite
                      this function and scale each entry in the jacobian with \f$1/\sigma_i\f$ where \f$\sigma_i\f$ are the (inverse) weights (or errors)
                      of the i-th datapoint in the dataset!
             */
            virtual void evaluateJacobianNum(double* evalout, const double* params, double h=10.0*QF_SQRT_DBL_EPSILON) const;

        protected:
            /** \brief size of function output \f$ M \f$ */
            uint64_t m_evalout;

            inline Functor() { m_evalout=1; }
        };

    public:

        /*! \brief base class for QFFitAlgorthm::Functor classes that let the user fit a dataset \f$(x_m,y_m,\sigma_m)\f$ to a 1D-function

                \f[ g_m(\vec{p})=\frac{y_m-f(x_m; m(\vec{p}))}{\sigma_m} \f]

            This class does not assume anything about the function \f$f(\cdot)\f$. it's implementatoin
            is left to child classes of this virtual class (see e.g. QFFitAlgorithm::FitQFFitFunctionFunctor ).

            This function implements a mode for bootstrapping during the fit. In this mode, not all \f$ N \f$ data in  \f$(x_m,y_m,\sigma_m)\f$ are used for the fit,
            but only a subset of \f$ \mbox{bootstrapFraction}\cdot N \f$ datapoints, which are chosen randomly from the provided input data on every call of
            prepareBootstrapSelection(). This feature can be used to implement e.g. error estimates by bootstrapping of the fit input data vector, i.e.
            repeat the fit \f$ M \f$ times with each time a new subset of data and the calculate the standard deviation of the fit values.

            This class manages the data internally and the protected members m_dataX, m_dataY and m_dataWeight are redirected, as required.
            they are also returned by getDatX(), getDataY(), getDataWeight(). The same is done for m_M and getDataPoints().
            These arrays either point to the arrays, provided in the constructor or by the \c setData...() methods, if bootstrapping is
            disabled, or they point to internal vectors, which contain the currently selected subset of the dataset.

            \note The default bootstrapFraction is 0.7!
        */
        class QFLIB_EXPORT FitFunctionFunctor: public Functor, public FunctorBootstrapInterface {
            public:
                /*! \brief constructor, initializes the functor
                    \param dataX the x-values data vector \f$ x_m \f$
                    \param dataY the y-values data vector \f$ y_m \f$
                    \param dataWeights the weight vector \f$ \sigma_m \f$
                    \param M number of datapoints
                */
                explicit FitFunctionFunctor(const double* dataX, const double* dataF, const double* dataWeight, uint64_t M) ;

                virtual ~FitFunctionFunctor();
                /** \copydoc QFFitAlgorithm::FitFunctionFunctor::isWeightedLSQ() */
                virtual bool isWeightedLSQ() const;
                /** \copydoc QFFitAlgorithm::FitFunctionFunctor::areAllWeightsOne() */
                virtual bool areAllWeightsOne() const;

                /** \brief returns a pointer to the current x-data \f$ x_m \f$ */
                const double* getDataX() const;
                /** \brief returns a pointer to the current y-data \f$ y_m \f$ */
                const double* getDataF() const;
                /** \brief returns a pointer to the current weights-data \f$ \sigma_m \f$ */
                const double* getDataWeight() const;
                /** \brief returns a pointer to the current x-data \f$ x_m \f$ */
                QVector<double> getDataXV() const;
                /** \brief returns a pointer to the current y-data \f$ y_m \f$ */
                QVector<double> getDataFV() const;
                /** \brief returns a pointer to the current weights-data \f$ \sigma_m \f$ */
                QVector<double> getDataWeightV() const;
                /** \brief returns the current number of datapoints in the \f$(x_m,y_m,\sigma_m)\f$ dataset. */
                uint64_t getDataPoints() const;

                /** \brief sets a new input data vector for x-data
                 *
                 * \note You will have to call prepareBootstrapSelection() after using this function for the changes to take effect in boot strapping mode
                 */
                void setDataX(const double* data);
                /** \brief sets a new input data vector for y-data
                 *
                 * \note You will have to call prepareBootstrapSelection() after using this function for the changes to take effect in boot strapping mode
                 */
                void setDataF(const double* data);
                /** \brief sets a new input data vector for weights-data
                 *
                 * \note You will have to call prepareBootstrapSelection() after using this function for the changes to take effect in boot strapping mode
                 */
                void setDataWeight(const double* data);
                /** \brief sets a new count of input data
                 *
                 * \note You will have to call prepareBootstrapSelection() after using this function for the changes to take effect in boot strapping mode
                 */
                void setDataPoints(uint64_t data);

                /** \brief prepares a new selection of data for bootstrapping
                 *
                 * \note if you only want to reapply the current selection and not select new data, call reapplyBootstrapselection() instead.
                 */
                virtual void prepareBootstrapSelection();
                /** \brief this function reapplies the current bootstrappig selection, i.e. if the input-data changed, but not its number and also the selection should not be recreated.
                 *
                 * \note to create a new subset from the input data, call prepareBootstrapSelection()
                 */
                virtual void reapplyBootstrapselection();
                /** \brief switches bootstrapping on and off, if \c enabled=true and \c prepBootstrapping=true, the function prepareBootstrapSelection() is called */
                virtual void setBootstrappingEnabled(bool enabled, bool prepBootstrapping=true);
                virtual bool getBootstrappingEnabled() const;
                /** \brief sets the fraction of the datapoints, that are selected by prepareBootstrapSelection(), if \c bootstrappingEnabled=true and \c prepBootstrapping=true, the function prepareBootstrapSelection() is called */
                virtual void setBootstrappingFraction(double fraction, bool prepBootstrapping=true);
                virtual double getBootstrappingFraction() const;

                virtual void evaluateJacobianNum(double* evalout, const double* params, double h=10.0*QF_SQRT_DBL_EPSILON) const;

                /** \brief calculate the \f$ \chi^2=\sum\limits_i\left(\frac{y_i-f(x_i)}{\sigma_i}\right)^2 */
                double calculateChi2(const double *params) const;

            protected:

                /** \brief the x-values data vector \f$ x_m \f$, possibly with a bootstrapping selection */
                const double* m_dataX;
                /** \brief the y-values data vector \f$ y_m \f$, possibly with a bootstrapping selection */
                const double* m_dataF;
                /** \brief the weight vector \f$ \sigma_m \f$, possibly with a bootstrapping selection */
                const double* m_dataWeight;
                /** \brief number of datapoints, possibly with a bootstrapping selection      */
                uint64_t m_M;


                /** \brief fraction [0..1] of data points, selected by  prepareBootstrapSelection(). The default value is 0.7. */
                double m_bootstrapFraction;
                /** \brief indicates, whether bootstrapping is enabled */
                bool m_bootstrapEnabled;

            private:
                /** \brief the x-values data vector \f$ x_m \f$ before bootstrapping selection */
                const double* i_dataX;
                /** \brief the y-values data vector \f$ y_m \f$ before bootstrapping selection */
                const double* i_dataF;
                /** \brief the weight vector \f$ \sigma_m \f$ before bootstrapping selection */
                const double* i_dataWeight;
                /** \brief number of datapoints before bootstrapping selection      */
                uint64_t i_M;

                /** \brief helper-vector, used for bootstrapping */
                QVector<double> bootstrappedX;
                /** \brief helper-vector, used for bootstrapping */
                QVector<double> bootstrappedF;
                /** \brief helper-vector, used for bootstrapping */
                QVector<double> bootstrappedW;
                /** \brief shuffled list of indexes into the input data, used to select the bootstrap data */
                QList<uint64_t> bootstrapIDs;
        };



    public:


        /*! \brief base class for QFFitAlgorthm::Functor classes that let the user fit a dataset \f$(x_m,y_m,f_m,\sigma_m)\f$ to a 1D-function

                \f[ g_m(\vec{p})=\frac{f_m-f(x_m,y_m; m(\vec{p}))}{\sigma_m} \f]

            This class does not assume anything about the function \f$f(\cdot)\f$. it's implementatoin
            is left to child classes of this virtual class (see e.g. QFFitAlgorithm::FitQFFitFunctionFunctor ).

            This function implements a mode for bootstrapping during the fit. In this mode, not all \f$ N \f$ data in  \f$(x_m,y_m,f_m,\sigma_m)\f$ are used for the fit,
            but only a subset of \f$ \mbox{bootstrapFraction}\cdot N \f$ datapoints, which are chosen randomly from the provided input data on every call of
            prepareBootstrapSelection(). This feature can be used to implement e.g. error estimates by bootstrapping of the fit input data vector, i.e.
            repeat the fit \f$ M \f$ times with each time a new subset of data and the calculate the standard deviation of the fit values.

            This class manages the data internally and the protected members m_dataX, m_dataY and m_dataWeight are redirected, as required.
            they are also returned by getDatX(), getDataY(), getDataWeight(). The same is done for m_M and getDataPoints().
            These arrays either point to the arrays, provided in the constructor or by the \c setData...() methods, if bootstrapping is
            disabled, or they point to internal vectors, which contain the currently selected subset of the dataset.

            \note The default bootstrapFraction is 0.7!
        */
        class QFLIB_EXPORT FitFunctionFunctor2D: public Functor, public FunctorBootstrapInterface {
            public:
                /*! \brief constructor, initializes the functor
                    \param dataX the x-values data vector \f$ x_m \f$
                    \param dataY the y-values data vector \f$ y_m \f$
                    \param dataF the f-values data vector \f$ f_m \f$
                    \param dataWeights the weight vector \f$ \sigma_m \f$
                    \param M number of datapoints
                */
                explicit FitFunctionFunctor2D(const double* dataX, const double* dataY, const double* dataF, const double* dataWeight, uint64_t M) ;

                virtual ~FitFunctionFunctor2D();
                /** \copydoc QFFitAlgorithm::FitFunctionFunctor::isWeightedLSQ() */
                virtual bool isWeightedLSQ() const;
                /** \copydoc QFFitAlgorithm::FitFunctionFunctor::areAllWeightsOne() */
                virtual bool areAllWeightsOne() const;

                /** \brief returns a pointer to the current x-data \f$ x_m \f$ */
                const double* getDataX() const;
                /** \brief returns a pointer to the current y-data \f$ y_m \f$ */
                const double* getDataY() const;

                /** \brief returns a pointer to the current f-data \f$ f_m \f$ */
                const double* getDataF() const;
                /** \brief returns a pointer to the current weights-data \f$ \sigma_m \f$ */
                const double* getDataWeight() const;
                /** \brief returns the current number of datapoints in the \f$(x_m,y_m,\sigma_m)\f$ dataset. */
                uint64_t getDataPoints() const;

                /** \brief sets a new input data vector for x-data
                 *
                 * \note You will have to call prepareBootstrapSelection() after using this function for the changes to take effect in boot strapping mode
                 */
                void setDataX(const double* data);
                /** \brief sets a new input data vector for y-data
                 *
                 * \note You will have to call prepareBootstrapSelection() after using this function for the changes to take effect in boot strapping mode
                 */
                void setDataY(const double* data);

                /** \brief sets a new input data vector for f-data
                 *
                 * \note You will have to call prepareBootstrapSelection() after using this function for the changes to take effect in boot strapping mode
                 */
                void setDataF(const double* data);
                /** \brief sets a new input data vector for weights-data
                 *
                 * \note You will have to call prepareBootstrapSelection() after using this function for the changes to take effect in boot strapping mode
                 */
                void setDataWeight(const double* data);
                /** \brief sets a new count of input data
                 *
                 * \note You will have to call prepareBootstrapSelection() after using this function for the changes to take effect in boot strapping mode
                 */
                void setDataPoints(uint64_t data);

                /** \brief prepares a new selection of data for bootstrapping
                 *
                 * \note if you only want to reapply the current selection and not select new data, call reapplyBootstrapselection() instead.
                 */
                virtual void prepareBootstrapSelection();
                /** \brief this function reapplies the current bootstrappig selection, i.e. if the input-data changed, but not its number and also the selection should not be recreated.
                 *
                 * \note to create a new subset from the input data, call prepareBootstrapSelection()
                 */
                virtual void reapplyBootstrapselection();
                /** \brief switches bootstrapping on and off, if \c enabled=true and \c prepBootstrapping=true, the function prepareBootstrapSelection() is called */
                virtual void setBootstrappingEnabled(bool enabled, bool prepBootstrapping=true);
                virtual bool getBootstrappingEnabled() const;
                /** \brief sets the fraction of the datapoints, that are selected by prepareBootstrapSelection(), if \c bootstrappingEnabled=true and \c prepBootstrapping=true, the function prepareBootstrapSelection() is called */
                virtual void setBootstrappingFraction(double fraction, bool prepBootstrapping=true);
                virtual double getBootstrappingFraction() const;

                virtual void evaluateJacobianNum(double* evalout, const double* params, double h=10.0*QF_SQRT_DBL_EPSILON) const;

            protected:

                /** \brief the x-values data vector \f$ x_m \f$, possibly with a bootstrapping selection */
                const double* m_dataX;
                /** \brief the y-values data vector \f$ y_m \f$, possibly with a bootstrapping selection */
                const double* m_dataY;

                /** \brief the y-values data vector \f$ f_m \f$, possibly with a bootstrapping selection */
                const double* m_dataF;
                /** \brief the weight vector \f$ \sigma_m \f$, possibly with a bootstrapping selection */
                const double* m_dataWeight;
                /** \brief number of datapoints, possibly with a bootstrapping selection      */
                uint64_t m_M;


                /** \brief fraction [0..1] of data points, selected by  prepareBootstrapSelection(). The default value is 0.7. */
                double m_bootstrapFraction;
                /** \brief indicates, whether bootstrapping is enabled */
                bool m_bootstrapEnabled;

            private:
                /** \brief the x-values data vector \f$ x_m \f$ before bootstrapping selection */
                const double* i_dataX;
                /** \brief the y-values data vector \f$ y_m \f$ before bootstrapping selection */
                const double* i_dataY;

                /** \brief the y-values data vector \f$ f_m \f$ before bootstrapping selection */
                const double* i_dataF;
                /** \brief the weight vector \f$ \sigma_m \f$ before bootstrapping selection */
                const double* i_dataWeight;
                /** \brief number of datapoints before bootstrapping selection      */
                uint64_t i_M;

                /** \brief helper-vector, used for bootstrapping */
                QVector<double> bootstrappedX;
                /** \brief helper-vector, used for bootstrapping */
                QVector<double> bootstrappedY;
                /** \brief helper-vector, used for bootstrapping */
                QVector<double> bootstrappedF;
                /** \brief helper-vector, used for bootstrapping */
                QVector<double> bootstrappedW;
                /** \brief shuffled list of indexes into the input data, used to select the bootstrap data */
                QList<uint64_t> bootstrapIDs;
        };

    public:

        /*! \brief base class for QFFitAlgorthm::Functor classes that let the user fit a dataset \f$(x_m,y_m,z_m,f_m,\sigma_m)\f$ to a 1D-function

                \f[ g_m(\vec{p})=\frac{f_m-f(x_m,y_m,z_m; m(\vec{p}))}{\sigma_m} \f]

            This class does not assume anything about the function \f$f(\cdot)\f$. it's implementatoin
            is left to child classes of this virtual class (see e.g. QFFitAlgorithm::FitQFFitFunctionFunctor ).

            This function implements a mode for bootstrapping during the fit. In this mode, not all \f$ N \f$ data in  \f$(x_m,y_m,z_m,f_m,\sigma_m)\f$ are used for the fit,
            but only a subset of \f$ \mbox{bootstrapFraction}\cdot N \f$ datapoints, which are chosen randomly from the provided input data on every call of
            prepareBootstrapSelection(). This feature can be used to implement e.g. error estimates by bootstrapping of the fit input data vector, i.e.
            repeat the fit \f$ M \f$ times with each time a new subset of data and the calculate the standard deviation of the fit values.

            This class manages the data internally and the protected members m_dataX, m_dataY and m_dataWeight are redirected, as required.
            they are also returned by getDatX(), getDataY(), getDataWeight(). The same is done for m_M and getDataPoints().
            These arrays either point to the arrays, provided in the constructor or by the \c setData...() methods, if bootstrapping is
            disabled, or they point to internal vectors, which contain the currently selected subset of the dataset.

            \note The default bootstrapFraction is 0.7!
        */
        class QFLIB_EXPORT FitFunctionFunctor3D: public Functor, public FunctorBootstrapInterface {
            public:
                /*! \brief constructor, initializes the functor
                    \param dataX the x-values data vector \f$ x_m \f$
                    \param dataY the y-values data vector \f$ y_m \f$
                    \param dataZ the z-values data vector \f$ z_m \f$
                    \param dataF the f-values data vector \f$ f_m \f$
                    \param dataWeights the weight vector \f$ \sigma_m \f$
                    \param M number of datapoints
                */
                explicit FitFunctionFunctor3D(const double* dataX, const double* dataY, const double* dataZ, const double* dataF, const double* dataWeight, uint64_t M) ;

                virtual ~FitFunctionFunctor3D();

                /** \brief returns a pointer to the current x-data \f$ x_m \f$ */
                const double* getDataX() const;
                /** \brief returns a pointer to the current y-data \f$ y_m \f$ */
                const double* getDataY() const;
                /** \brief returns a pointer to the current z-data \f$ z_m \f$ */
                const double* getDataZ() const;
                /** \brief returns a pointer to the current f-data \f$ f_m \f$ */
                const double* getDataF() const;
                /** \brief returns a pointer to the current weights-data \f$ \sigma_m \f$ */
                const double* getDataWeight() const;
                /** \brief returns the current number of datapoints in the \f$(x_m,y_m,\sigma_m)\f$ dataset. */
                uint64_t getDataPoints() const;
                /** \copydoc QFFitAlgorithm::FitFunctionFunctor::isWeightedLSQ() */
                virtual bool isWeightedLSQ() const;
                /** \copydoc QFFitAlgorithm::FitFunctionFunctor::areAllWeightsOne() */
                virtual bool areAllWeightsOne() const;

                /** \brief sets a new input data vector for x-data
                 *
                 * \note You will have to call prepareBootstrapSelection() after using this function for the changes to take effect in boot strapping mode
                 */
                void setDataX(const double* data);
                /** \brief sets a new input data vector for y-data
                 *
                 * \note You will have to call prepareBootstrapSelection() after using this function for the changes to take effect in boot strapping mode
                 */
                void setDataY(const double* data);
                /** \brief sets a new input data vector for z-data
                 *
                 * \note You will have to call prepareBootstrapSelection() after using this function for the changes to take effect in boot strapping mode
                 */
                void setDataZ(const double* data);
                /** \brief sets a new input data vector for f-data
                 *
                 * \note You will have to call prepareBootstrapSelection() after using this function for the changes to take effect in boot strapping mode
                 */
                void setDataF(const double* data);
                /** \brief sets a new input data vector for weights-data
                 *
                 * \note You will have to call prepareBootstrapSelection() after using this function for the changes to take effect in boot strapping mode
                 */
                void setDataWeight(const double* data);
                /** \brief sets a new count of input data
                 *
                 * \note You will have to call prepareBootstrapSelection() after using this function for the changes to take effect in boot strapping mode
                 */
                void setDataPoints(uint64_t data);

                /** \brief prepares a new selection of data for bootstrapping
                 *
                 * \note if you only want to reapply the current selection and not select new data, call reapplyBootstrapselection() instead.
                 */
                virtual void prepareBootstrapSelection();
                /** \brief this function reapplies the current bootstrappig selection, i.e. if the input-data changed, but not its number and also the selection should not be recreated.
                 *
                 * \note to create a new subset from the input data, call prepareBootstrapSelection()
                 */
                virtual void reapplyBootstrapselection();
                /** \brief switches bootstrapping on and off, if \c enabled=true and \c prepBootstrapping=true, the function prepareBootstrapSelection() is called */
                virtual void setBootstrappingEnabled(bool enabled, bool prepBootstrapping=true);
                virtual bool getBootstrappingEnabled() const;
                /** \brief sets the fraction of the datapoints, that are selected by prepareBootstrapSelection(), if \c bootstrappingEnabled=true and \c prepBootstrapping=true, the function prepareBootstrapSelection() is called */
                virtual void setBootstrappingFraction(double fraction, bool prepBootstrapping=true);
                virtual double getBootstrappingFraction() const;

                virtual void evaluateJacobianNum(double* evalout, const double* params, double h=10.0*QF_SQRT_DBL_EPSILON) const;

            protected:

                /** \brief the x-values data vector \f$ x_m \f$, possibly with a bootstrapping selection */
                const double* m_dataX;
                /** \brief the y-values data vector \f$ y_m \f$, possibly with a bootstrapping selection */
                const double* m_dataY;
                /** \brief the y-values data vector \f$ z_m \f$, possibly with a bootstrapping selection */
                const double* m_dataZ;
                /** \brief the y-values data vector \f$ f_m \f$, possibly with a bootstrapping selection */
                const double* m_dataF;
                /** \brief the weight vector \f$ \sigma_m \f$, possibly with a bootstrapping selection */
                const double* m_dataWeight;
                /** \brief number of datapoints, possibly with a bootstrapping selection      */
                uint64_t m_M;


                /** \brief fraction [0..1] of data points, selected by  prepareBootstrapSelection(). The default value is 0.7. */
                double m_bootstrapFraction;
                /** \brief indicates, whether bootstrapping is enabled */
                bool m_bootstrapEnabled;

            private:
                /** \brief the x-values data vector \f$ x_m \f$ before bootstrapping selection */
                const double* i_dataX;
                /** \brief the y-values data vector \f$ y_m \f$ before bootstrapping selection */
                const double* i_dataY;
                /** \brief the y-values data vector \f$ z_m \f$ before bootstrapping selection */
                const double* i_dataZ;
                /** \brief the y-values data vector \f$ f_m \f$ before bootstrapping selection */
                const double* i_dataF;
                /** \brief the weight vector \f$ \sigma_m \f$ before bootstrapping selection */
                const double* i_dataWeight;
                /** \brief number of datapoints before bootstrapping selection      */
                uint64_t i_M;

                /** \brief helper-vector, used for bootstrapping */
                QVector<double> bootstrappedX;
                /** \brief helper-vector, used for bootstrapping */
                QVector<double> bootstrappedY;
                /** \brief helper-vector, used for bootstrapping */
                QVector<double> bootstrappedZ;
                /** \brief helper-vector, used for bootstrapping */
                QVector<double> bootstrappedF;
                /** \brief helper-vector, used for bootstrapping */
                QVector<double> bootstrappedW;
                /** \brief shuffled list of indexes into the input data, used to select the bootstrap data */
                QList<uint64_t> bootstrapIDs;
        };

        /** \brief returns \c true, if the given functor is a weighted least-squares functor, i.e. represents the problem \f$ (m(x_i,\vec{p})-f_i)/\sigma_i\f$
         *
         * This is checked by first casting the functor to FitFunctionFunctor, FitFunctionFunctor2D or FitFunctionFunctor3D and then
         * checking whether the weights have been set properly.
         */
        static bool functorHasWeights(const Functor* f);

        static bool functorAllWeightsOne(const Functor* f);

        static const double* functorGetWeights(const Functor* f, uint64_t *N=NULL);
    public:


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
                explicit IRLSFunctorAdaptor( FitFunctionFunctor* functor, double irls_parameter=1.1) ;

                virtual ~IRLSFunctorAdaptor() ;


                /*! \brief function that evaluates the arbitrary function

                    \param[out] evalout with size get_evalout()
                    \param params parameter vector with size get_paramcount()
                 */
                virtual void evaluate(double* evalout, const double* params) const;

                /*! \brief function that evaluates the arbitrary function

                    \param[out] evalout with size get_evalout()*get_paramcount() in the order
                                \f$ \left[ \frac{\partial f_1}{\partial p_1}, \frac{\partial f_1}{\partial p_2}, ..., \frac{\partial f_1}{\partial p_N}, \frac{\partial f_2}{\partial p_1}, \frac{\partial f_2}{\partial p_2}, ..., \frac{\partial f_2}{\partial p_N}, ..., \frac{\partial f_M}{\partial p_N} \right] \f$
                    \param params parameter vector with size get_paramcount()

                    \note This is only implemented if get_implementsJacobian() returns true
                 */
                virtual void evaluateJacobian(double* evalout, const double* params) const;

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

    public:

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
                explicit FitQFFitFunctionFunctor(QFFitFunction* model, const double* currentParams, const bool* fixParams, const double* dataX, const double* dataY, const double* dataWeight, uint64_t M) ;

                virtual ~FitQFFitFunctionFunctor();

                /*! \brief Implements the inverse mapping function \f$ \vec{q}=m^{-1}(\vec{p})\in\mathbb{R}^Q \f$ where \f$ \vec{p} \f$ is given by \a modelData.
                           The result is a NEW array created by calling \c calloc()
                */
                double* createMappedArrayForFunctor(const double* modelData) const;

                /*! \brief Implements the inverse mapping function \f$ \vec{q}=m^{-1}(\vec{p})\in\mathbb{R}^Q \f$ where \f$ \vec{p} \f$ is given by \a modelData.
                           This function only copies those entries that are present in \a functorData.
                */
                void mapArrayFromModelToFunctor(double* functorData, const double* modelData) const;

                /*! \brief Implements the mapping function \f$ \vec{p}=m(\vec{q})\in\mathbb{R}^N \f$ where \f$ \vec{q} \f$ is given by \a functorData.
                           and \f$ \vec{p} \f$ is returned in \a modelData. This function only overwrites the entries that are present in \a functorData.
                */
                void mapArrayFromFunctorToModel(double* modelData, const double* functorData) const;

                /*! \brief Implements the mapping function \f$ \vec{p}=m(\vec{q})\in\mathbb{R}^N \f$
                */
                inline int mapFromFunctorToModel(int functorIndex) const{
                    return modelFromFunctor[functorIndex];
                }

                /** \brief evaluate the function \f$ \vec{g}(\vec{q}) \f$ */
                virtual void evaluate(double* evalout, const double* params) const;
                /** \brief evaluate the function underlying model function only (not taling into account the weights and measured data!) */
                virtual void evaluateModelOnly(double* evalout, const double* params) const;

                /** \brief evaluate the functions jacobian \f$ J_{n,m}(\vec{q})=\frac{\partial g_m(\vec{q})}{\partial q_n}=-\frac{1}{\sigma_m}\cdot\frac{\partial f(x_m, m(\vec{q}))}{\partial m(q_n)} \f$ */
                virtual void evaluateJacobian(double* evalout, const double* params) const;

                /** \brief returns \c true if the model implements its jacobian analytically and therefore evaluateJacobian() may be used */
                virtual bool get_implementsJacobian() const { return m_model->get_implementsDerivatives(); }

                /** \brief return the number of parameters \f$ Q \f$ in \f$ \vec{q}=m(\vec{p}) \f$ */
                virtual int get_paramcount() const { return m_paramCount; }

                /** \brief return the used QFFitFunction */
                QFFitFunction* getModel() const { return m_model; }

                /** \brief return a pointer to the stored initial fit parameters */
                const double* getModelParams() const { return m_modelParams; }
                /** \brief return a pointer to an array indicating which parameters are fixed */
                 bool* getModelParamsFix() const { return m_modelparamsFix; }
                /** \brief return the number of to the stored initial fit parameters */
                int getModelParamsCount() const { return m_model->paramCount(); }

                inline bool isParameterTransformsEnabled() const { return enableParameterTransforms; }
                inline void setParameterTransformsEnabled(bool enabled) {
                    enableParameterTransforms=enabled;
                }
                inline bool isFitLogY() const { return fitLogY; }
                inline void setFitLogY(bool enabled) {
                    fitLogY=enabled;
                }

                /** \brief transform fit parameters */
                void transfromParameters(double* params) const;
                /** \brief backtransform fit parameters */
                void backtransfromParameters(double* params) const;
                /** \brief transform fit parameters */
                QVector<double> transfromParametersCopy(const double* params) const;
                /** \brief backtransform fit parameters */
                QVector<double> backtransfromParametersCopy(const double* params) const;
            protected:


                /** \brief  QFFitFunction object used to evaluate \f$ f(x; \vec{p}) \f$ */
                QFFitFunction* m_model;
                /** \brief number of parameters in m_model */
                int m_N;
                /** \brief number of real (non-fixed) parameters, \c m_paramCount<=m_N */
                int m_paramCount;
                /** \brief maps from model parameter index to functor parameter index (size m_N) */
                int* functorFromModel;
                /** \brief maps from functor parameter index to model parameter index (size m_paramCount) */
                int* modelFromFunctor;
                /** \brief copy of the current model parameter vector (size m_N) */
                double* m_modelParams;
                /** \brief vector containing which parameters are fixed */
                bool* m_modelparamsFix;
                /** \brief does the model contain parameter transforms? */
                bool hasParameterTransforms;
                /** \brief enable or disable parameter transforms */
                bool enableParameterTransforms;
                /** \brief parameter transform mode for the functor parameters (calculated in the constructor from the model parameters/properties)*/
                QVector<QFFitFunctionBase::ParameterType> paramTransforms;
                /** \brief if set \c true, the functor calculates log(dataY) and log(model(x,p)), which can improve the fit in some cases  */
                bool fitLogY;
        };



    public:



        /*! \brief this is a special functor which is used for data fitting to objective functions declared by QFFitFunction

            The QFFitFunction declares a 1D function \f$ f(x,y,z; \vec{p}), \vec{p}\in\mathbb{R}^N \f$ . This functor is provided with a set of data \f$ (x_m, y_m, z_m, f_m, \sigma_m), m=1..M \f$
            where \f$ \sigma_m \f$ are weights for the data. Then This Functor implementation maps the 1D Function \f$ f(x,y,z; \vec{p}) \f$ to a \f$ M \f$ dimensional
            function \f$ \vec{g}(\vec{p}) \f$ with:
                \f[ g_m(\vec{p})=\frac{f_m-f(x_m,y_m,z_m; m(\vec{p}))}{\sigma_m} \f]
            As QFFitFunction may contain parameters \f$ p_n, n=1..N \f$ that are no fitting parameters (either because they are defined as such, or because they were fixed
            by the user, or they are currently hidden) we have to do parameter reordering. So the fitting algorithm is presented with a modified parameter vector
            \f$ \vec{q}\in\mathbb{R}^Q \f$ with \f$ Q<N \f$ . The mapping is done using a function \f$ m(\cdot) \f$ with \f$ \vec{p}=m(\vec{q})\in\mathbb{R}^N \f$ and
            \f$ \vec{q}=m^{-1}(\vec{p})\in\mathbb{R}^Q \f$ which is defined below, so this Functor finally calculates:
                \f[ g_m(\vec{q})=\frac{f_m-f(x_m,y_m,z_m; m(\vec{q}))}{\sigma_m} \f]
            The data, the weights and the fix-vector is given to this functor in the constructor. Afterwards you may call evaluate() to calculate \f$ \vec{g}(\vec{q}) \f$ .


        */
        class QFLIB_EXPORT FitQFFitFunction3DFunctor: public FitFunctionFunctor3D {
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
                explicit FitQFFitFunction3DFunctor(QFFitFunction3D* model, const double* currentParams, const bool* fixParams, const double* dataX, const double* dataY, const double* dataZ, const double* dataF, const double* dataWeight, uint64_t M) ;

                virtual ~FitQFFitFunction3DFunctor();

                /*! \brief Implements the inverse mapping function \f$ \vec{q}=m^{-1}(\vec{p})\in\mathbb{R}^Q \f$ where \f$ \vec{p} \f$ is given by \a modelData.
                           The result is a NEW array created by calling \c calloc()
                */
                double* createMappedArrayForFunctor(const double* modelData) const;

                /*! \brief Implements the inverse mapping function \f$ \vec{q}=m^{-1}(\vec{p})\in\mathbb{R}^Q \f$ where \f$ \vec{p} \f$ is given by \a modelData.
                           This function only copies those entries that are present in \a functorData.
                */
                void mapArrayFromModelToFunctor(double* functorData, const double* modelData) const;

                /*! \brief Implements the mapping function \f$ \vec{p}=m(\vec{q})\in\mathbb{R}^N \f$ where \f$ \vec{q} \f$ is given by \a functorData.
                           and \f$ \vec{p} \f$ is returned in \a modelData. This function only overwrites the entries that are present in \a functorData.
                */
                void mapArrayFromFunctorToModel(double* modelData, const double* functorData) const;

                /*! \brief Implements the mapping function \f$ \vec{p}=m(\vec{q})\in\mathbb{R}^N \f$
                */
                inline int mapFromFunctorToModel(int functorIndex) const{
                    return modelFromFunctor[functorIndex];
                }

                /** \brief evaluate the function \f$ \vec{g}(\vec{q}) \f$ */
                virtual void evaluate(double* evalout, const double* params) const;

                /** \brief evaluate the functions jacobian \f$ J_{n,m}(\vec{q})=\frac{\partial g_m(\vec{q})}{\partial q_n}=-\frac{1}{\sigma_m}\cdot\frac{\partial f(x_m, m(\vec{q}))}{\partial m(q_n)} \f$ */
                virtual void evaluateJacobian(double* evalout, const double* params) const;

                /** \brief returns \c true if the model implements its jacobian analytically and therefore evaluateJacobian() may be used */
                inline virtual bool get_implementsJacobian() const { return false; }

                /** \brief return the number of parameters \f$ Q \f$ in \f$ \vec{q}=m(\vec{p}) \f$ */
                inline virtual int get_paramcount() const { return m_paramCount; }

                /** \brief return the used QFFitFunction */
                inline QFFitFunction3D* getModel() const { return m_model; }

                /** \brief return a pointer to the stored initial fit parameters */
                inline double* getModelParams() const { return m_modelParams; }
                /** \brief return a pointer to an array indicating which parameters are fixed */
                inline bool* getModelParamsFix() const { return m_modelparamsFix; }
                /** \brief return the number of to the stored initial fit parameters */
                inline int getModelParamsCount() const { return m_model->paramCount(); }

                inline bool isParameterTransformsEnabled() const { return enableParameterTransforms; }
                inline void setParameterTransformsEnabled(bool enabled) {
                    enableParameterTransforms=enabled;
                }
                inline bool isFitLogY() const { return fitLogY; }
                inline void setFitLogY(bool enabled) {
                    fitLogY=enabled;
                }

                /** \brief transform fit parameters */
                void transfromParameters(double* params) const;
                /** \brief backtransform fit parameters */
                void backtransfromParameters(double* params) const;
                /** \brief transform fit parameters */
                QVector<double> transfromParametersCopy(const double* params) const;
                /** \brief backtransform fit parameters */
                QVector<double> backtransfromParametersCopy(const double* params) const;
            protected:


                /** \brief  QFFitFunction3D object used to evaluate \f$ f(x,y,z; \vec{p}) \f$ */
                QFFitFunction3D* m_model;
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
                /** \brief does the model contain parameter transforms? */
                bool hasParameterTransforms;
                /** \brief enable or disable parameter transforms */
                bool enableParameterTransforms;
                /** \brief parameter transform mode for the functor parameters (calculated in the constructor from the model parameters/properties)*/
                QVector<QFFitFunctionBase::ParameterType> paramTransforms;
                /** \brief if set \c true, the functor calculates log(dataY) and log(model(x,p)), which can improve the fit in some cases  */
                bool fitLogY;
        };





    public:

        /*! \brief this is a special functor which is used for data fitting to objective functions declared by QFFitFunction

            The QFFitFunction declares a 1D function \f$ f(x,y,z; \vec{p}), \vec{p}\in\mathbb{R}^N \f$ . This functor is provided with a set of data \f$ (x_m, y_m, z_m, f_m, \sigma_m), m=1..M \f$
            where \f$ \sigma_m \f$ are weights for the data. Then This Functor implementation maps the 1D Function \f$ f(x,y,z; \vec{p}) \f$ to a \f$ M \f$ dimensional
            function \f$ \vec{g}(\vec{p}) \f$ with:
                \f[ g_m(\vec{p})=\frac{f_m-f(x_m,y_m,z_m; m(\vec{p}))}{\sigma_m} \f]
            As QFFitFunction may contain parameters \f$ p_n, n=1..N \f$ that are no fitting parameters (either because they are defined as such, or because they were fixed
            by the user, or they are currently hidden) we have to do parameter reordering. So the fitting algorithm is presented with a modified parameter vector
            \f$ \vec{q}\in\mathbb{R}^Q \f$ with \f$ Q<N \f$ . The mapping is done using a function \f$ m(\cdot) \f$ with \f$ \vec{p}=m(\vec{q})\in\mathbb{R}^N \f$ and
            \f$ \vec{q}=m^{-1}(\vec{p})\in\mathbb{R}^Q \f$ which is defined below, so this Functor finally calculates:
                \f[ g_m(\vec{q})=\frac{f_m-f(x_m,y_m,z_m; m(\vec{q}))}{\sigma_m} \f]
            The data, the weights and the fix-vector is given to this functor in the constructor. Afterwards you may call evaluate() to calculate \f$ \vec{g}(\vec{q}) \f$ .


        */
        class QFLIB_EXPORT FitQFFitFunction2DFunctor: public FitFunctionFunctor2D {
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
                explicit FitQFFitFunction2DFunctor(QFFitFunction2D* model, const double* currentParams, const bool* fixParams, const double* dataX, const double* dataY, const double* dataF, const double* dataWeight, uint64_t M) ;

                virtual ~FitQFFitFunction2DFunctor();

                /*! \brief Implements the inverse mapping function \f$ \vec{q}=m^{-1}(\vec{p})\in\mathbb{R}^Q \f$ where \f$ \vec{p} \f$ is given by \a modelData.
                           The result is a NEW array created by calling \c calloc()
                */
                double* createMappedArrayForFunctor(const double* modelData) const;

                /*! \brief Implements the inverse mapping function \f$ \vec{q}=m^{-1}(\vec{p})\in\mathbb{R}^Q \f$ where \f$ \vec{p} \f$ is given by \a modelData.
                           This function only copies those entries that are present in \a functorData.
                */
                void mapArrayFromModelToFunctor(double* functorData, const double* modelData) const;

                /*! \brief Implements the mapping function \f$ \vec{p}=m(\vec{q})\in\mathbb{R}^N \f$ where \f$ \vec{q} \f$ is given by \a functorData.
                           and \f$ \vec{p} \f$ is returned in \a modelData. This function only overwrites the entries that are present in \a functorData.
                */
                void mapArrayFromFunctorToModel(double* modelData, const double* functorData) const;

                /*! \brief Implements the mapping function \f$ \vec{p}=m(\vec{q})\in\mathbb{R}^N \f$
                */
                inline int mapFromFunctorToModel(int functorIndex) const{
                    return modelFromFunctor[functorIndex];
                }

                /** \brief evaluate the function \f$ \vec{g}(\vec{q}) \f$ */
                virtual void evaluate(double* evalout, const double* params) const;

                /** \brief evaluate the functions jacobian \f$ J_{n,m}(\vec{q})=\frac{\partial g_m(\vec{q})}{\partial q_n}=-\frac{1}{\sigma_m}\cdot\frac{\partial f(x_m, m(\vec{q}))}{\partial m(q_n)} \f$ */
                virtual void evaluateJacobian(double* evalout, const double* params) const;

                /** \brief returns \c true if the model implements its jacobian analytically and therefore evaluateJacobian() may be used */
                inline virtual bool get_implementsJacobian() const { return false; }

                /** \brief return the number of parameters \f$ Q \f$ in \f$ \vec{q}=m(\vec{p}) \f$ */
                inline virtual int get_paramcount() const { return m_paramCount; }

                /** \brief return the used QFFitFunction */
                inline QFFitFunction2D* getModel() const { return m_model; }

                /** \brief return a pointer to the stored initial fit parameters */
                inline double* getModelParams() const { return m_modelParams; }
                /** \brief return a pointer to an array indicating which parameters are fixed */
                inline bool* getModelParamsFix() const { return m_modelparamsFix; }
                /** \brief return the number of to the stored initial fit parameters */
                inline int getModelParamsCount() const { return m_model->paramCount(); }

                inline bool isParameterTransformsEnabled() const { return enableParameterTransforms; }
                inline void setParameterTransformsEnabled(bool enabled) {
                    enableParameterTransforms=enabled;
                }
                inline bool isFitLogY() const { return fitLogY; }
                inline void setFitLogY(bool enabled) {
                    fitLogY=enabled;
                }

                /** \brief transform fit parameters */
                void transfromParameters(double* params) const;
                /** \brief backtransform fit parameters */
                void backtransfromParameters(double* params) const;
                /** \brief transform fit parameters */
                QVector<double> transfromParametersCopy(const double* params) const;
                /** \brief backtransform fit parameters */
                QVector<double> backtransfromParametersCopy(const double* params) const;
            protected:


                /** \brief  QFFitFunction2D object used to evaluate \f$ f(x,y,z; \vec{p}) \f$ */
                QFFitFunction2D* m_model;
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
                /** \brief does the model contain parameter transforms? */
                bool hasParameterTransforms;
                /** \brief enable or disable parameter transforms */
                bool enableParameterTransforms;
                /** \brief parameter transform mode for the functor parameters (calculated in the constructor from the model parameters/properties)*/
                QVector<QFFitFunctionBase::ParameterType> paramTransforms;
                /** \brief if set \c true, the functor calculates log(dataY) and log(model(x,p)), which can improve the fit in some cases  */
                bool fitLogY;
        };

    protected:


        /*! \brief this is a special \b internal functor which is used for data fitting to objective functions declared by QFFitFunction

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
                explicit FitQFOptimizeFunctionFunctor(Functor* model, const double* currentParams, const bool* fixParams) ;

                ~FitQFOptimizeFunctionFunctor();

                /*! \brief Implements the inverse mapping function \f$ \vec{q}=m^{-1}(\vec{p})\in\mathbb{R}^Q \f$ where \f$ \vec{p} \f$ is given by \a modelData.
                           The result is a NEW array created by calling \c calloc()
                */
                double* createMappedArrayForFunctor(const double* modelData) const;

                /*! \brief Implements the inverse mapping function \f$ \vec{q}=m^{-1}(\vec{p})\in\mathbb{R}^Q \f$ where \f$ \vec{p} \f$ is given by \a modelData.
                           This function only copies those entries that are present in \a functorData.
                */
                void mapArrayFromModelToFunctor(double* functorData, const double* modelData) const;

                /*! \brief Implements the mapping function \f$ \vec{p}=m(\vec{q})\in\mathbb{R}^N \f$ where \f$ \vec{q} \f$ is given by \a functorData.
                           and \f$ \vec{p} \f$ is returned in \a modelData. This function only overwrites the entries that are present in \a functorData.
                */
                void mapArrayFromFunctorToModel(double* modelData, const double* functorData) const;

                /** \brief evaluate the function \f$ \vec{g}(\vec{q}) \f$ */
                virtual void evaluate(double* evalout, const double* params) const;

                /** \brief evaluate the functions jacobian \f$ J_{n,m}(\vec{q})=\frac{\partial g_m(\vec{q})}{\partial q_n}=-\frac{1}{\sigma_m}\cdot\frac{\partial f(x_m, m(\vec{q}))}{\partial m(q_n)} \f$ */
                virtual void evaluateJacobian(double* evalout, const double* params) const;

                /** \brief returns \c true if the model implements its jacobian analytically and therefore evaluateJacobian() may be used */
                inline virtual bool get_implementsJacobian() const { return m_model->get_implementsJacobian(); }

                /** \brief return the number of parameters \f$ Q \f$ in \f$ \vec{q}=m(\vec{p}) \f$ */
                inline virtual int get_paramcount() const { return m_paramCount; }

                /** \brief return the underlying functor */
                inline virtual Functor* get_model() const { return m_model; }
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

    protected:

        /*! \brief this \b internal functor extends FitQFOptimizeFunctionFunctor with bootstrapping functionality */
        class QFLIB_EXPORT FitQFOptimizeFunctionBootstrapFunctor: public FitQFOptimizeFunctionFunctor, public FunctorBootstrapInterface {
            public:
                inline virtual ~FitQFOptimizeFunctionBootstrapFunctor() {}
                explicit FitQFOptimizeFunctionBootstrapFunctor(Functor* model, const double* currentParams, const bool* fixParams) ;

                /** \brief prepares a new selection of data for bootstrapping
                 *
                 * \note if you only want to reapply the current selection and not select new data, call reapplyBootstrapselection() instead.
                 */
                virtual void prepareBootstrapSelection();
                /** \brief this function reapplies the current bootstrappig selection, i.e. if the input-data changed, but not its number and also the selection should not be recreated.
                 *
                 * \note to create a new subset from the input data, call prepareBootstrapSelection()
                 */
                virtual void reapplyBootstrapselection();
                /** \brief switches bootstrapping on and off, if \c enabled=true and \c prepBootstrapping=true, the function prepareBootstrapSelection() is called */
                virtual void setBootstrappingEnabled(bool enabled, bool prepBootstrapping=true);
                /** \brief sets the fraction of the datapoints, that are selected by prepareBootstrapSelection(), if \c bootstrappingEnabled=true and \c prepBootstrapping=true, the function prepareBootstrapSelection() is called */
                virtual void setBootstrappingFraction(double fraction, bool prepBootstrapping=true);

                FunctorBootstrapInterface *getModelAsBootstrap() const;
        };


    public:


        /*! \brief functor base class for fmin() and variants thereof that allow to model a 1-dimensional objective functio that shall be minimized

            This functor allows to evaluate arbitrary 1-dimensional function
              \f[ f(\vec{p})\in\mathbb{R}, \vec{p}\in\mathbb{R}^N. \f]
            For some data fitting/optimization algorithms we also have to know the jacobian of the model function, i.e.
              \f[ J_{i}=\frac{\partial f}{\partial p_i} \f]
            This function is implemented in evaluateJacobian(). As it is impossible to calculate the jacobian analytically for
            some functions, or because the implementor didn't do it, there is the function get_implementsJacobian() which tells
            whether the jacobian was implemented or not.

         */
        class QFLIB_EXPORT FunctorForFMin {
        public:
            inline explicit FunctorForFMin() {  }

            inline virtual  ~FunctorForFMin() {}

            /*! \brief function that evaluates the arbitrary function

                \param[out] evalout with size get_evalout()
                \param params parameter vector with size get_paramcount()
             */
            virtual double evaluate(const double* params)=0;

            /*! \brief function that evaluates the first derivatives \f$ J_{i}=\frac{\partial f}{\partial p_i} \f$ of the functor function

                \param[out] evalout with size get_paramcount() in the order
                            \f$ \left[ \frac{\partial f}{\partial p_1}, \frac{\partial f}{\partial p_2}, ..., \frac{\partial f}{\partial p_N}  \right]\f$
                \param params parameter vector with size get_paramcount()

                \note This is only implemented if get_implementsJacobian() returns true
             */
            virtual void evaluateJacobian(double* evalout, const double* params);

            /** \brief return the number of parameters \f$ N \f$ */
            virtual int get_paramcount() const=0;

            /** \brief return \c true if the jacobian is implemented */
            inline virtual bool get_implementsJacobian() const { return false; }


            /*! \brief  function that evaluates the first derivatives \f$ J_{i}=\frac{\partial f}{\partial p_i} \f$ of the functor function numerically, using the 5-point stencil method

                \param[out] evalout with size get_paramcount() in the order
                            \f$ \left[ \frac{\partial f}{\partial p_1}, \frac{\partial f}{\partial p_2}, ..., \frac{\partial f}{\partial p_N} \right] \f$
                \param params parameter vector with size get_paramcount()

             */
            virtual void evaluateJacobianNum(double* evalout, const double* params, double h=10.0*QF_SQRT_DBL_EPSILON);

        };

        /** \brief a general function that may be used to represent an objective function for fmin(). The parameters are ( \a params, \a userData ). */
        typedef double (*FMinFunctionP)(const double*, void*);
        /** \brief a general function that may be used to represent an objective function for fmin(). The parameters are ( \a dfdp_out, \a params, \a userData ).*/
        typedef void (*FMinFunctionDerivP)(double*, const double*, void*);

        /** \brief a general function that may be used to represent an objective function for fmin(). The parameters are ( \a params ). */
        typedef double (*FMinFunctionNoUserDataP)(const double*);
        /** \brief a general function that may be used to represent an objective function for fmin(). The parameters are ( \a dfdp_out, \a params ).*/
        typedef void (*FMinFunctionNoUserDataDerivP)(double*, const double*);

        /*! \brief an implementation of FunctorForFMin that calls C-functions as objective functions

            \note you can supply user data (as a void*) which is handed over to the function if it is supplied and if
         */
        class QFLIB_EXPORT FunctorForFMinFromCFunc: public FunctorForFMin {
            public:
                explicit FunctorForFMinFromCFunc(int Nparams, FMinFunctionP* F);
                explicit FunctorForFMinFromCFunc(int Nparams, FMinFunctionP* F, FMinFunctionDerivP* dFdp);
                explicit FunctorForFMinFromCFunc(int Nparams, FMinFunctionNoUserDataP* F);
                explicit FunctorForFMinFromCFunc(int Nparams, FMinFunctionNoUserDataP* F, FMinFunctionNoUserDataDerivP* dFdp);


                inline virtual  ~FunctorForFMinFromCFunc() {}

                /*! \brief function that evaluates the arbitrary function

                    \param[out] evalout with size get_evalout()
                    \param params parameter vector with size get_paramcount()
                 */
                virtual double evaluate(const double* params);

                /*! \brief function that evaluates the first derivatives \f$ J_{i}=\frac{\partial f}{\partial p_i} \f$ of the functor function

                    \param[out] evalout with size get_paramcount() in the order
                                \f$ \left[ \frac{\partial f}{\partial p_1}, \frac{\partial f}{\partial p_2}, ..., \frac{\partial f}{\partial p_N}  \right]\f$
                    \param params parameter vector with size get_paramcount()

                    \note This is only implemented if get_implementsJacobian() returns true
                 */
                virtual void evaluateJacobian(double* evalout, const double* params);

                /** \brief return the number of parameters \f$ N \f$ */
                virtual int get_paramcount() const;

                /** \brief return \c true if the jacobian is implemented */
                virtual bool get_implementsJacobian() const;

                void setUserData(void* userData);
                void* getUserData() const;

            protected:
                FMinFunctionP* F;
                FMinFunctionDerivP* dFdp;
                FMinFunctionNoUserDataP* FnoUser;
                FMinFunctionNoUserDataDerivP* dFdpNoUser;
                int Nparams;
                void* m_userData;
        };
    protected:

        /*! \brief this is a special \b internal functor which is used for objective function minimization in fmin() when parameters are fixed.

            It adapts another FunctorForFMin for use with fixed parameters, i.e. it represents an interface with a reduced parameter set to the minimization
            algorithm and an interface with the full parameters to the functor.

            The Functor declares a 1D function \f$ f(\vec{p})\in\mathbb{R}, \vec{p}\in\mathbb{R}^N \f$ .

            As Functor may contain parameters \f$ p_n, n=1..N \f$ that are no fitting parameters (e.g. because they are fixed) we have to do parameter reordering.
            So the fitting algorithm is presented with a modified parameter vector \f$ \vec{q}\in\mathbb{R}^Q \f$ with \f$ Q<N \f$ . The mapping is done using a
             function \f$ m(\cdot) \f$ with \f$ \vec{p}=m(\vec{q})\in\mathbb{R}^N \f$ and
            \f$ \vec{q}=m^{-1}(\vec{p})\in\mathbb{R}^Q \f$ which is defined below, so this Functor finally calculates:
                \f[ g_m(\vec{q})=\frac{y_m-f(x_m; m(\vec{q}))}{\sigma_m} \f]
            Afterwards you may call evaluate() to calculate \f$ \vec{g}(\vec{q}) \f$ .
        */
        class QFLIB_EXPORT FunctorForFMinFixAdaptor: public FunctorForFMin {
            public:
                /*! \brief constructor, initializes the functor
                    \param model Functor object used to evaluate \f$ f(x; \vec{p}) \f$
                    \param params parameter count for model
                    \param currentParams the initial parameters. You will have to give the values of ALL non-fit parameters in this vector!
                    \param fixParams \c true if a parameter is fixed by the user
                */
                explicit FunctorForFMinFixAdaptor(FunctorForFMin* model, const double* currentParams, const bool* fixParams) ;

                virtual ~FunctorForFMinFixAdaptor();

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

                /** \brief evaluate the function \f$ g(\vec{q}) \f$ */
                virtual double evaluate(const double* params);

                /** \brief evaluate the functions jacobian \f$ J_{n}(\vec{q})=\frac{\partial g(\vec{q})}{\partial q_n} \f$ */
                virtual void evaluateJacobian(double* evalout, const double* params);

                /** \brief returns \c true if the model implements its jacobian analytically and therefore evaluateJacobian() may be used */
                inline virtual bool get_implementsJacobian() const { return m_model->get_implementsJacobian(); }

                /** \brief return the number of parameters \f$ Q \f$ in \f$ \vec{q}=m(\vec{p}) \f$ */
                inline virtual int get_paramcount() const { return m_paramCount; }
            protected:


                /** \brief  QFFitFunction object used to evaluate \f$ f(x; \vec{p}) \f$ */
                FunctorForFMin* m_model;
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
                /** \brief copy of the currently fixed model parameters (size m_N) */
                bool* m_fixParams;
        };




    public:


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
            void addNumberList(QString resultName, const double* value, int items, QString unit=QString(""));
            /** \brief set a result of type number matrix */
            void addNumberMatrix(QString resultName, const double* value, int columns, int rows, QString unit=QString(""));
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

    public:

        /** \brief class construtor */
        QFFitAlgorithm();

        /** \brief class destructor */
        virtual ~QFFitAlgorithm();


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

        /** \brief defines the possibilities to estimate the fit parameter errors, when calling fit() */
        enum FitParameterErrorEstimates {
            fpeAlgorithm=0,
            fpeBootstrapping=1
        };

        /** \brief sets the number of bootstrapping repeats, that fit() will do, if setErrorEstimateModeFit() was set to fpeBootstrapping */
        void setBootstrapRepeats(int repeats);
        /** \brief returns the number of bootstrapping repeats, that fit() will do, if setErrorEstimateModeFit() was set to fpeBootstrapping */
        int getBootstrapRepeats() const;

        /** \brief sets the bootstrap fraction, that fit() will do, if setErrorEstimateModeFit() was set to fpeBootstrapping */
        void setBootstrapFraction(double fraction);
        /** \brief returns the bootstrap fraction, that fit() will do, if setErrorEstimateModeFit() was set to fpeBootstrapping */
        double getBootstrapFraction() const;

        /** \brief sets the relative bootstrap distortion, that fit() will do, if setErrorEstimateModeFit() was set to fpeBootstrapping */
        void setBootstrapDistortion(double distortion);
        /** \brief returns the relative bootstrap distortion, that fit() will do, if setErrorEstimateModeFit() was set to fpeBootstrapping */
        double getBootstrapDistortion() const;

        /** \brief define, how fit() estimates the parameter errors */
        void setErrorEstimateModeFit(FitParameterErrorEstimates mode);
        /** \brief define, how fit() estimates the parameter errors */
        void setErrorEstimateModeFit(int mode);
        /** \brief define, how fit() estimates the parameter errors */
        void setErrorEstimateModeFit(FitParameterErrorEstimates mode, int repeats);
        /** \brief define, how fit() estimates the parameter errors */
        void setErrorEstimateModeFit(FitParameterErrorEstimates mode, int repeats, double bootstrapFraction);
        /** \brief define, how fit() estimates the parameter errors */
        void setErrorEstimateModeFit(FitParameterErrorEstimates mode, int repeats, double bootstrapFraction, double distortion);
        /** \brief returns, how fit() estimates the parameter errors */
        FitParameterErrorEstimates getErrorEstimateModeFit() const;

        void readErrorEstimateParametersFit(const QFProperties *props, const QString& prefix=QString());


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
            \param fitLogY if this is \c true, this function solves the logarithmized problem \f$ \vec{p}^\ast=\min\limits_{\vec{p}}\sum\limits_{i=1}^M\left\|\frac{\log(y_i)-\log(f(x_i; \vec{p}))}{\log(\sigma_i)}\right\| \f$
            \param[out] chi2 If a pointer is given, the chi^2 is stored into the given memory location
            \param[out] cov_matrix This function may return the variance/covariance matrix of the problem in this parameter (if a pointer is given!). This is a square matrix, of size \c NC*NC, where \c NC is the number of actual fit parameters (i.e. parameters thatare fit parameters and NOT fixed and visible!). The order of the parameters is the same as in the result vector, but with the non-fit parameters left out!!!
            \return a FitResult object describing the fit result

            \note If the bootstrapping mode is activated (see setErrorEstimateModeFit() ) then this function will first do the actual fit with the full input data vector
                  (of which the parameters are returned). Then it will perform a given number of bootstrapping repeats, where the fit is repeated with the result of the
                  first fit as initial parameters (distorted by a small fraction), but only for a subset of the input data. Then, from the results of these fits, a standard
                  deviation for each parameter is calculated and returned as error estimate. <b>Note, that this routine will require significantly more time, than a simple fit!</b>
        */
        FitResult fit(double* paramsOut, double* paramErrorsOut, const double* dataX, const double* dataY, const double* dataWeight, uint64_t N, QFFitFunction* model, const double* initialParams, const bool* fixParams=NULL, const double* paramsMin=NULL, const double* paramsMax=NULL, bool fitLogY=false, double *chi2=NULL, QVector<double>* cov_matrix=NULL);


        /*! \brief this wrapper routine allows to use the fitting algorithm for 3D data fitting with a given model function \f$ f(x,y,z; \vec{p}) \f$ encoded in \a model
                   and a set of measurements \f$ (x_i, y_i, f_i \sigma_i) \f$ where \f$ \sigma_i \f$ are the weights for the measurements. Then this routine solves the problem:
                   \f$ \vec{p}^\ast=\min\limits_{\vec{p}}\sum\limits_{i=1}^M\left\|\frac{f_i-f(x_i,y_i; \vec{p})}{\sigma_i}\right\| \f$

            \param[out] paramsOut The optimal parameter vector is written into this array
            \param[out] paramErrorsOut The optimal parameter error vector is written into this array
            \param dataX \f$ x_i\f$ -value of the data
            \param dataY \f$ y_i\f$ -values of the data
            \param dataF \f$ f_i\f$ -values of the data
            \param dataWeight weights \f$ w_i\f$
            \param N number of datapoints (i.e. size of the arrays dataX, dataY and dataWeights
            \param model the model function to use
            \param initialParams initial values for the parameters
            \param fixParams which parameters to fix (if \c NULL, no parameters are fixed)
            \param paramsMin lower parameter bound
            \param paramsMax upper parameter bound
            \param fitLogY if this is \c true, this function solves the logarithmized problem \f$ \vec{p}^\ast=\min\limits_{\vec{p}}\sum\limits_{i=1}^M\left\|\frac{\log(y_i)-\log(f(x_i; \vec{p}))}{\log(\sigma_i)}\right\| \f$
            \return a FitResult object describing the fit result

            \note If the bootstrapping mode is activated (see setErrorEstimateModeFit() ) then this function will first do the actual fit with the full input data vector
                  (of which the parameters are returned). Then it will perform a given number of bootstrapping repeats, where the fit is repeated with the result of the
                  first fit as initial parameters (distorted by a small fraction), but only for a subset of the input data. Then, from the results of these fits, a standard
                  deviation for each parameter is calculated and returned as error estimate. <b>Note, that this routine will require significantly more time, than a simple fit!</b>
        */
        FitResult fit2D(double* paramsOut, double* paramErrorsOut, const double* dataX, const double* dataY, const double* dataF, const double* dataWeight, uint64_t N, QFFitFunction2D *model, const double* initialParams, const bool* fixParams=NULL, const double* paramsMin=NULL, const double* paramsMax=NULL, bool fitLogY=false);


        /*! \brief this wrapper routine allows to use the fitting algorithm for 3D data fitting with a given model function \f$ f(x,y,z; \vec{p}) \f$ encoded in \a model
                   and a set of measurements \f$ (x_i, y_i, z_i, f_i \sigma_i) \f$ where \f$ \sigma_i \f$ are the weights for the measurements. Then this routine solves the problem:
                   \f$ \vec{p}^\ast=\min\limits_{\vec{p}}\sum\limits_{i=1}^M\left\|\frac{f_i-f(x_i,y_i,z_i; \vec{p})}{\sigma_i}\right\| \f$

            \param[out] paramsOut The optimal parameter vector is written into this array
            \param[out] paramErrorsOut The optimal parameter error vector is written into this array
            \param dataX \f$ x_i\f$ -value of the data
            \param dataY \f$ y_i\f$ -values of the data
            \param dataZ \f$ z_i\f$ -values of the data
            \param dataF \f$ f_i\f$ -values of the data
            \param dataWeight weights \f$ w_i\f$
            \param N number of datapoints (i.e. size of the arrays dataX, dataY and dataWeights
            \param model the model function to use
            \param initialParams initial values for the parameters
            \param fixParams which parameters to fix (if \c NULL, no parameters are fixed)
            \param paramsMin lower parameter bound
            \param paramsMax upper parameter bound
            \param fitLogY if this is \c true, this function solves the logarithmized problem \f$ \vec{p}^\ast=\min\limits_{\vec{p}}\sum\limits_{i=1}^M\left\|\frac{\log(y_i)-\log(f(x_i; \vec{p}))}{\log(\sigma_i)}\right\| \f$
            \return a FitResult object describing the fit result

            \note If the bootstrapping mode is activated (see setErrorEstimateModeFit() ) then this function will first do the actual fit with the full input data vector
                  (of which the parameters are returned). Then it will perform a given number of bootstrapping repeats, where the fit is repeated with the result of the
                  first fit as initial parameters (distorted by a small fraction), but only for a subset of the input data. Then, from the results of these fits, a standard
                  deviation for each parameter is calculated and returned as error estimate. <b>Note, that this routine will require significantly more time, than a simple fit!</b>
        */
        FitResult fit3D(double* paramsOut, double* paramErrorsOut, const double* dataX, const double* dataY, const double* dataZ, const double* dataF, const double* dataWeight, uint64_t N, QFFitFunction3D *model, const double* initialParams, const bool* fixParams=NULL, const double* paramsMin=NULL, const double* paramsMax=NULL, bool fitLogY=false);


        /*! \brief this wrapper routine allows to use the fitting algorithm for a general least-squares minimization problem which is defined by a vector-valued model function \f$ \vec{f}(\vec{p}) \f$ encoded in \a model.
                   The norm (usually the L2-norm, i.e. least-squares fit) if \f$ \vec{f}(\vec{p})\f$ is then minimized by this function. This routine solves the problem:
                   \f$ \vec{p}^\ast=\min\limits_{\vec{p}}\|\vec{f}(\vec{p})\| \f$

            \param[out] paramsOut The optimal parameter vector is written into this array
            \param[out] paramErrorsOut The optimal parameter error vector is written into this array
            \param model the model function to use
            \param initialParams initial values for the parameters
            \param fixParams which parameters to fix (if \c NULL, no parameters are fixed)
            \param paramsMin lower parameter bound
            \param paramsMax upper parameter bound
            \return a FitResult object describing the fit result
        */
        FitResult lsqMinimize(double* paramsOut, double* paramErrorsOut, Functor* model, const double* initialParams, const bool* fixParams=NULL, const double* paramsMin=NULL, const double* paramsMax=NULL);


        /*! \brief high-level interface to the intMinimize() if it is implemented (check get_implementsMinimize() ). It allows to find a parameterset \f$\vec{p}\f$
                   that minimized the objective function \f$ f(\vec{p})\f$ provided by \a model, i.e. it solves the problem:
                       \f$ \vec{p}^\ast=\min\limits_{\vec{p}}f(\vec{p}) \f$

            \param[out] paramsOut The optimal parameter vector is written into this array
            \param model the model function to use
            \param initialParams initial values for the parameters
            \param fixParams which parameters to fix (if \c NULL, no parameters are fixed)
            \param paramsMin lower parameter bound
            \param paramsMax upper parameter bound
            \return a FitResult object describing the fit result
        */
        FitResult fmin(double* paramsOut, FunctorForFMin* model, const double* initialParams, const bool* fixParams=NULL, const double* paramsMin=NULL, const double* paramsMax=NULL);



        /*! \brief high-level interface to the intMinimize() if it is implemented (check get_implementsMinimize() ). It allows to find a parameterset \f$\vec{p}\f$
                   that minimized the objective function \f$ f(\vec{p})\f$ provided by \a model, i.e. it solves the problem:
                       \f$ \vec{p}^\ast=\min\limits_{\vec{p}}f(\vec{p}) \f$

            \param[out] paramsOut The optimal parameter vector is written into this array
            \param model the model function to use
            \param Nparams number of parameters of the model
            \param initialParams initial values for the parameters
            \param dFdp \b optional: the derivatives of the objective function
            \param fixParams which parameters to fix (if \c NULL, no parameters are fixed)
            \param paramsMin lower parameter bound
            \param paramsMax upper parameter bound
            \param userData a pointer (to user-suppplied data) simply handed over to model and dFdp
            \return a FitResult object describing the fit result
        */
        FitResult fmin(double* paramsOut, FMinFunctionP* model, int Nparams, const double* initialParams, FMinFunctionDerivP* dFdp=NULL, const bool* fixParams=NULL, const double* paramsMin=NULL, const double* paramsMax=NULL, void* userData=NULL);



        /*! \brief high-level interface to the intMinimize() if it is implemented (check get_implementsMinimize() ). It allows to find a parameterset \f$\vec{p}\f$
                   that minimized the objective function \f$ f(\vec{p})\f$ provided by \a model, i.e. it solves the problem:
                       \f$ \vec{p}^\ast=\min\limits_{\vec{p}}f(\vec{p}) \f$

            \param[out] paramsOut The optimal parameter vector is written into this array
            \param model the model function to use
            \param Nparams number of parameters of the model
            \param initialParams initial values for the parameters
            \param dFdp \b optional: the derivatives of the objective function
            \param fixParams which parameters to fix (if \c NULL, no parameters are fixed)
            \param paramsMin lower parameter bound
            \param paramsMax upper parameter bound
            \param userData a pointer (to user-suppplied data) simply handed over to model and dFdp
            \return a FitResult object describing the fit result
        */
        FitResult fmin(double* paramsOut, FMinFunctionNoUserDataP* model, int Nparams, const double* initialParams, FMinFunctionNoUserDataDerivP* dFdp=NULL, const bool* fixParams=NULL, const double* paramsMin=NULL, const double* paramsMax=NULL, void* userData=NULL);

        /*! \brief display a modal configuration dialog for you fit algorithm
            \return \c true if the user clicked "OK"

            \note you will also have to implement the writeback of the parameters, using setParameter()
        */
        virtual bool displayConfig();

        /** \brief tell the class to use the specified reporter widget */
        inline void setReporter(QFFitAlgorithmReporter* reporter) {
            m_reporter=reporter;
        }

        /** \brief return the current reporter */
        QFFitAlgorithmReporter* reporter() { return m_reporter; }
    protected:
        /** \brief fit algorithm parameters */
        QMap<QString, QVariant> m_parameters;

        /** \brief reporter object */
        QFFitAlgorithmReporter* m_reporter;

        /** \brief number of repeats, when calculating errors with bootstrapping. The default value is 20. */
        int m_bootstrapRepeats;
        /** \brief bootstrapping fraction, initially 0.6 */
        double m_bootstrapFraction;
        /** \brief relative bootstrapping distortion of the fit values, initially 2%=0.02 */
        double m_bootstrapDistortion;
        /** \brief the error estimateion mode for fit() */
        FitParameterErrorEstimates m_errorEstimateModeFit;


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

    public:
        /** \brief indicates whether this object implements the method intMinimize() and you may therefore also call fmin()
         *
         * \note overwrite this function (and return \c true) if you implement intMinimize()
         */
        inline virtual bool get_implementsMinimize() { return false; }
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
        /*! \brief this routine implements objective function minimization

            \param[out] paramsOut The optimal parameter vector is written into this array
            \param initialParams initial values for the parameters
            \param model the model function to minimize
            \param paramsMin lower parameter bound
            \param paramsMax upper parameter bound
            \return a FitResult object describing the fit result


            This method uses the nested FunctorForFMin class to solve a problem of the form
                \f[ \vec{p}^\ast=\min\limits_{\vec{p} f(\vec{p}) \f]
        */
        virtual FitResult intMinimize(double* paramsOut, const double* initialParams, FunctorForFMin* model, const double* paramsMin, const double* paramsMax);
    public:
        /** \brief return a name for the algorithm */
        virtual QString name() const=0;
        /** \brief return a short name for the algorithm (max. around 10-20 characters) */
        virtual QString shortName() const { return name(); }
        /** \brief return a short unique algorithm ID string */
        virtual QString id() const=0;

        /** \brief indicates, that this fit algorithm should not be used any longer, i.e. is DEPRECATED */
        virtual bool isDeprecated() const;

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
