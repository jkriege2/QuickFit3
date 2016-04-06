/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#ifndef FITALGORITHM_LEVMAR_H
#define FITALGORITHM_LEVMAR_H

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
    described in:
      - <b>[PRE92]</b>	Press, W. H., Teukolsky, S. A., Vetterling, W. T. und Flannery, B. P.: <b>Numerical Recipes in C.</b> The Art of Scientific Computing. 2. Auflage, Cambridge University Press, Cambridge - New York - Port Chester - Melbourne - Sydney, 1992</blockquote>
    .

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

        /** \copydoc QFFitAlgorithm::intFit() */
        virtual FitResult intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, Functor* model, const double* paramsMin, const double* paramsMax);


    public:
        /** \brief class constructor */
        QFFitAlgorithmLevmar();
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const { return QObject::tr("levmar: Levenberg-Marquardt Fitting Algorithm with box constraints"); }
        /** \copydoc QFFitAlgorithm::id() */
        virtual QString id() const { return QString("fit_levmar"); }
        /** \copydoc QFFitAlgorithm::get_supportsBoxConstraints() */
        virtual bool get_supportsBoxConstraints() const { return true; }
        /** \copydoc QFFitAlgorithm::displayConfig() */
        virtual bool displayConfig();
        /** \copydoc QFFitAlgorithm::isThreadSafe() */
        virtual bool isThreadSafe() const ;
        virtual bool isDeprecated() const;
};



/*! \brief plugin class for Levmar Fit Algorithm
    \ingroup qf3fitalgp_levmar
*/
class QFPFitAlgorithmLevmar : public QObject, public QFPluginFitAlgorithm {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitAlgorithm)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFPFitAlgorithmLevmar")
#endif

    public:
        QF_PLUGIN

        /** Default constructor */
        QFPFitAlgorithmLevmar(QObject* parent=NULL): QObject(parent) {}
        /** Default destructor */
        virtual ~QFPFitAlgorithmLevmar() {}

        /** \brief unique id for the plugin */
        virtual QString getID() const {
            return QString("fit_levmar");
        }


        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("levmar: Levenberg-Marquardt Fitting Algorithm with box constraints");
        }

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("Levenberg-Marquardt Fitting Algorithm with box constraints using levmar library (version %1) from http://www.ics.forth.gr/~lourakis/levmar/.").arg(LM_VERSION);
        }

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan Krieger");
        }

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("plugin: (c) 2011 by Jan Krieger<br/>levmar (version %1) : (c) 2004  Manolis Lourakis (lourakis at ics forth gr)").arg(LM_VERSION);
        }

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("");
        }


        /** \brief plugin icon  */
        virtual QString getIconFilename() const {
            return ":/lib/fitalg_icon.png";
        };
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const {
            QStringList sl;
            sl<<"fit_levmar";
            return sl;
        }

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitAlgorithm* get(const QString& id) const {
            if (id=="fit_levmar") return new QFFitAlgorithmLevmar();
            return NULL;
        }
    private:
};

#endif // FITALGORITHM_LEVMAR_H
