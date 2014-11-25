/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#ifndef QFFITALGORITHMTHREADDEDFIT_H
#define QFFITALGORITHMTHREADDEDFIT_H

#include "lib_imexport.h"
#include <QThread>
#include <QObject>
#include "qffitalgorithm.h"
#include "qffitfunction.h"

/*! \brief this allows to execute a given QFFitAlgorithm inside an extra QThread.
    \ingroup qf3lib_fitting

    After allocating the memory for an object, call init() to intialize the data. Then you may
    start() the thread. Finally the results may be read using getResult() and the fit runtime in milliseconds using
    getDeltaTime().

*/
class QFLIB_EXPORT QFFitAlgorithmThreadedFit: public QThread {
        Q_OBJECT
    public:
        /** \brief class cosntructor */
        QFFitAlgorithmThreadedFit(QObject* parent=NULL);
        /** \brief class destructor */
        virtual ~QFFitAlgorithmThreadedFit();

        /** \brief initialize the fit parameters */
        void init(QFFitAlgorithm* algorithm, double* paramsOut, double* paramErrorsOut, double* dataX, double* dataY, double* dataWeight, uint64_t N, QFFitFunction* model, double* initialParams, bool* fixParams=NULL, double* paramsMin=NULL, double* paramsMax=NULL, int repeats=1, bool fitLogY=false);

        /** \brief return the result of the last fit call */
        QFFitAlgorithm::FitResult getResult() const { return result; }
        /** \brief return the runtime of the fit in milliseconds */
        double getDeltaTime() const { return deltaTime; }

    protected:
        QFFitAlgorithm* algorithm;
        double* paramsOut;
        double* paramErrorsOut;
        double* dataX;
        double* dataY;
        double* dataWeight;
        uint64_t N;
        QFFitFunction* model;
        double* initialParams;
        bool* fixParams;
        double* paramsMin;
        double* paramsMax;
        QFFitAlgorithm::FitResult result;
        double deltaTime;
        bool fitLogY;

        int repeats;

        /** \brief here the fitting takes place */
        virtual void run();
};


#endif // QFFITALGORITHMTHREADDEDFIT_H
