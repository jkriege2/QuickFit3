#ifndef FCS_FITFUCTIONS_H
#define FCS_FITFUCTIONS_H

#include "qfpluginfitalgorithm.h"
#include <levmar.h>

/*!
    \defgroup qf3fitalgp_levmar Levmar Fitting Algorithm Plugin
    \ingroup qf3fitalgplugins
*/



/*! \brief QQFFitAlgorithm class for <a href="http://www.ics.forth.gr/~lourakis/levmar/">levmar</a> implementation of Leneberg-Marquardt Fit Algorithm with box constraints
    \ingroup qf3fitalgp_levmar


    \section QFFitAlgorithmLevmar_desc Algorithm Description

    <a href="http://www.ics.forth.gr/~lourakis/levmar/">levmar</a> is an open source implementation of the
    Levenberg-Marquardt fitting algorithm which is widely used in different open source projects.

    Note that this implementation expects the model function to return a vector which contains the evaluation
    of the model function at every data point in the measured data.

    This implementation supports unconstrained and box-constrained fits, depending on the protected data member
    constrained. By default it is set \c true which tells the class to use a box-constrained LM variant.
    The child class quickfitFitLevmarUnconstrained makes the unconstrained fit available.

    The next lines will give a short (and basic) description of the Levenberg-Marquardt method, as it is also
    described in
    <blockquote><b>[PRE92]</b>	Press, W. H., Teukolsky, S. A., Vetterling, W. T. und Flannery, B. P.: <b>Numerical Recipes in C.</b> The Art of Scientific Computing. 2. Auflage, Cambridge University Press, Cambridge - New York - Port Chester - Melbourne - Sydney, 1992</blockquote>


    We start from a least-squares score function
      \f[ \chi^2(\vec{p})=\sum\limits_{i=1}^N\frac{\bigl(y_i-m(\tau_i, \vec{p})\bigr)^2}{\sigma_i^2} \f]
    which we want to minimize with respect to the M parameters \f$ \vec{p}=(p_1,...,p_M)^t \f$, i.e. we
    want to solve the minimization problem \f[ \min\limits_{\vec{p}}\chi^2(\vec{p}) \f] with the N measured
    data points \f$ (\tau_i, y_i)_{i=1..N} \f$.

    To do so we first note that it should be possible to approximate \f$ \chi^2 \f$ by a quadratic form,
    if the local interval we look at is small enough. In this approximative case we have:
      \f[ \chi^2(\vec{p})\approx \gamma+\vec{d}\cdot\vec{p}+\frac{1}{2}\vec{p}\cdot\underline{H}\cdot\vec{p} \f]
    Here \f$ \vec{d}=\vec{\nabla}\chi^2(\vec{p}_{opt}) \f$ is the gradient of the function in the optimum \f$ \vec{p}_{opt} \f$
    which has to be 0 for an extremum. \f$ \underline{H}=(H_{i,j})=\frac{\partial^2\chi^2}{\partial p_i\;\partial p_j} \f$ is the
    Hessian matrix or the matrix of the second derivatives also evaluated at \f$ \vec{p}_{opt} \f$.

    If this approximation is valid, one may <i>jump to the minimum of this quadratic form in
    one single leap</i>. Say the current parameter estimation is \f$ \vec{p}_{cur} \f$, then
    the optimal parameter vector is \f[ \vec{p}_{opt}=\vec{p}_{cur}+\underline{H}^{-1}\cdot\bigl[-\vec{\nabla}\chi^2(\vec{p}_{cur})\bigr]. \f]
    This result may be optained from the above approximation if you demand \f$ \vec{\nabla}\chi^2 \f$ to vanish
    at \f$ \vec{p}=\vec{p}_{opt}. \f$ Note that \f$ \vec{d} \f$ and \f$ \underline{H} \f$ have to be known
    constants, so the algorithm has to estimate these to do the jump.

    If the above approximation is not valid we can not do much more than do a leap in the direction
    of the <i>steepest descent:</i> \f[ \vec{p}_{nex}=\vec{p}_{cur}-c\cdot\vec{\nabla}\chi^2(\vec{p}_{cur}) \f]
    Here c denotes a positive constant which influences the jump size.

    The Levenberg-Marquardt method is designed to vary smoothly between the two extremal cases described here.
    It uses the fact that the form of the function \f$ \chi^2(\vec{p}) \f$ is known to be a quadratic form. So
    it is easy to estimate the first and second dervative if \f$ \chi^2(\vec{p}) \f$ if the user provides the
    derivatives of the model function \f$ \bigl(\vec{\nabla}_{\vec{p}}m\bigr)(x,\vec{p}). \f$

    This implementation of the Levenberg-Marquardt scheme uses numerical methods to calculate the gradient, so
    it is not neccessary to know an analytic form of the derivatives. Levmar estimates the derivatives with the
    central difference scheme which is slower, but mor accurate than the standard forward/backward schemes.

*/
class QFFitAlgorithmLevmar: public QFFitAlgorithm {
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
        virtual FitResult intFit(double* paramsOut, double* paramErrorsOut, double* initialParams, Functor* model, double* paramsMin, double* paramsMax);


    public:
        /** \brief class constructor */
        QFFitAlgorithmLevmar();
        /** \brief return a name for the algorithm */
        virtual QString name() const { return QObject::tr("levmar: Levenberg-Marquardt Fitting Algorithm with box constraints"); };
        /** \brief return a short unique algorithm ID string */
        virtual QString id() const { return QString("fit_levmar"); };
        /** \brief return a HTML file to be displayed as algorithm help. This file has to be positioned in \c plugins/fitalgorithms/help/<plugin_id> */
        virtual QString helpFile() const { return id()+".html"; };
        /** \brief \c true if the algorithm supports bounded optimization with box constraints and \c false else */
        virtual bool get_supportsBoxConstraints() const { return true; };
};



/*! \brief plugin class for Levmar Fit Algorithm
    \ingroup qf3fitalgp_levmar
*/
class QFPFitAlgorithmLevmar : public QObject, public QFPluginFitAlgorithm {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitAlgorithm)
    public:
        /** Default constructor */
        QFPFitAlgorithmLevmar(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFPFitAlgorithmLevmar() {}

        /** \brief unique id for the plugin */
        virtual QString getID() const {
            return QString("fit_levmar");
        };


        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("levmar: Levenberg-Marquardt Fitting Algorithm with box constraints");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("Levenberg-Marquardt Fitting Algorithm with box constraints using levmar library (v %1) from http://www.ics.forth.gr/~lourakis/levmar/.").arg(LM_VERSION);
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan Krieger");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("plugin: (c) 2011 by Jan Krieger<br/>levmar: (c) 2004  Manolis Lourakis (lourakis at ics forth gr)");
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("");
        };

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const {
            QStringList sl;
            sl<<"fit_levmar";
            return sl;
        }

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitAlgorithm* get(QString id, QObject* parent) const {
            if (id=="fit_levmar") return new QFFitAlgorithmLevmar();
            return NULL;
        }
    private:
};

#endif // FCS_FITFUCTIONS_H
