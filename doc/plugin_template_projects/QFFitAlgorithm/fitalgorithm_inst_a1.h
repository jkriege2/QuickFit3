/*
Copyright (c) 2014
	
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


#ifndef HEADER_A1_H
#define HEADER_A1_H
#include "qffitalgorithm.h"


/*! \brief QQFFitAlgorithm implementation
    \ingroup doxygen_GROUPNAME


*/
class QFFitAlgorithmInst_A1: public QFFitAlgorithm {
    protected:

        /** \copydoc QFFitAlgorithm::intFit() */
        virtual FitResult intFit(double* paramsOut, double* paramErrorsOut, const double* initialParams, Functor* model, const double* paramsMin, const double* paramsMax);


    public:
        /** \brief class constructor */
        QFFitAlgorithmInst_A1();
        /** \brief class destructor */
        virtual ~QFFitAlgorithmInst_A1() {}
        /** \copydoc QFFitAlgorithm::name() */
        virtual QString name() const { return QObject::tr("long fit algorithm name"); }
        /** \copydoc QFFitAlgorithm::shortName() */
        virtual QString shortName() const { return QObject::tr("short name"); }
        /** \copydoc QFFitAlgorithm::id() */
        virtual QString id() const { return QString("fa_id"); }
        /** \copydoc QFFitAlgorithm::get_supportsBoxConstraints() */
        virtual bool get_supportsBoxConstraints() const { return true; }
        /** \copydoc QFFitAlgorithm::displayConfig() */
        virtual bool displayConfig();
        /** \copydoc QFFitAlgorithm::isThreadSafe() */
        virtual bool isThreadSafe() const { return false; }
};


#endif // HEADER_A1_H
