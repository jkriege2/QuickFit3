/*
Copyright (c) 2014
	
	last modification: $LastChangedDate: 2014-06-24 16:05:58 +0200 (Di, 24 Jun 2014) $  (revision $Rev: 3289 $)

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


#ifndef QFFITALGORITHMNLOPTDIRECT_H
#define QFFITALGORITHMNLOPTDIRECT_H

#include "qffitalgorithmnloptbase.h"

/*! \brief QFFitAlgorithm implementation for NLOpt: NLOPT_GN_DIRECT
    \ingroup qf3fitfunp_fit_nlopt


*/
class QFFitAlgorithmNLOptDIRECT: public QFFitAlgorithmNLOptBASE {

    public:
        /** \brief class constructor */
        inline QFFitAlgorithmNLOptDIRECT():
            QFFitAlgorithmNLOptBASE(NLOPT_GN_DIRECT_NOSCAL) {
        }
        /** \copydoc QFFitAlgorithm::id() */
        inline virtual QString id() const { return QString("fit_nlopt_direct"); }
};


class QFFitAlgorithmNLOptDIRECTL: public QFFitAlgorithmNLOptBASE {

    public:
        /** \brief class constructor */
        inline QFFitAlgorithmNLOptDIRECTL():
            QFFitAlgorithmNLOptBASE(NLOPT_GN_DIRECT_L_NOSCAL) {
        }
        /** \copydoc QFFitAlgorithm::id() */
        inline virtual QString id() const { return QString("fit_nlopt_directl"); }
};


class QFFitAlgorithmNLOptDIRECTLRAND: public QFFitAlgorithmNLOptBASE {

    public:
        /** \brief class constructor */
        inline QFFitAlgorithmNLOptDIRECTLRAND():
            QFFitAlgorithmNLOptBASE(NLOPT_GN_DIRECT_L_RAND_NOSCAL) {
        }
        /** \copydoc QFFitAlgorithm::id() */
        inline virtual QString id() const { return QString("fit_nlopt_directl_rand"); }
};

class QFFitAlgorithmNLOptCRS2_LM: public QFFitAlgorithmNLOptBASE {

    public:
        /** \brief class constructor */
        inline QFFitAlgorithmNLOptCRS2_LM():
            QFFitAlgorithmNLOptBASE(NLOPT_GN_CRS2_LM) {
        }
        /** \copydoc QFFitAlgorithm::id() */
        inline virtual QString id() const { return QString("fit_nlopt_CRS2_LM"); }
};


class QFFitAlgorithmNLOptMLSL: public QFFitAlgorithmNLOptBASE {

    public:
        /** \brief class constructor */
        inline QFFitAlgorithmNLOptMLSL():
            QFFitAlgorithmNLOptBASE(NLOPT_G_MLSL) {
        }
        /** \copydoc QFFitAlgorithm::id() */
        inline virtual QString id() const { return QString("fit_nlopt_MLSL"); }
};



class QFFitAlgorithmNLOptStoGO: public QFFitAlgorithmNLOptBASE {

    public:
        /** \brief class constructor */
        inline QFFitAlgorithmNLOptStoGO():
            QFFitAlgorithmNLOptBASE(NLOPT_GD_STOGO) {
        }
        /** \copydoc QFFitAlgorithm::id() */
        inline virtual QString id() const { return QString("fit_nlopt_STOGO"); }
};

class QFFitAlgorithmNLOptStoGORand: public QFFitAlgorithmNLOptBASE {

    public:
        /** \brief class constructor */
        inline QFFitAlgorithmNLOptStoGORand():
            QFFitAlgorithmNLOptBASE(NLOPT_GD_STOGO_RAND) {
        }
        /** \copydoc QFFitAlgorithm::id() */
        inline virtual QString id() const { return QString("fit_nlopt_STOGO_RAND"); }
};


class QFFitAlgorithmNLOptISRES: public QFFitAlgorithmNLOptBASE {

    public:
        /** \brief class constructor */
        inline QFFitAlgorithmNLOptISRES():
            QFFitAlgorithmNLOptBASE(NLOPT_GN_ISRES) {
        }
        /** \copydoc QFFitAlgorithm::id() */
        inline virtual QString id() const { return QString("fit_nlopt_ISRES"); }
};


class QFFitAlgorithmNLOptESCH: public QFFitAlgorithmNLOptBASE {

    public:
        /** \brief class constructor */
        inline QFFitAlgorithmNLOptESCH():
            QFFitAlgorithmNLOptBASE(NLOPT_GN_ESCH) {
        }
        /** \copydoc QFFitAlgorithm::id() */
        inline virtual QString id() const { return QString("fit_nlopt_ESCH"); }
};


#endif // QFFITALGORITHMNLOPTDIRECT_H
