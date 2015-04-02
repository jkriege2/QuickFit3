/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#ifndef QFRDRFCSDATAINTERFACE_H
#define QFRDRFCSDATAINTERFACE_H

#include <QtPlugin>

/*! \brief represents a common data interface to access FCS correlation curves
    \ingroup qf3rdrdp_fcs

    This interfaces allows access to these features:
      - Each dataset may contain a set of runs over which the program averages. The interface allows access to every
        single of these runs and also the average (and standard deviation) over all these runs. The number of runs is
        returned by getCorrelationRuns(). Note that only an average run has to be available, the singe runs are optional.
      - every run (and also the average/stddev) contains the same number of data points \f$ (\tau_i, g_i) \f$ which is
        returned by getCorrelationN()
      - all runs (and also the average/stddev) have the same lag time axis. The lags are returned by getCorrelationT().
      - The data of the runs is returned by getCorrelationRun(), getCorrelationMean() and getCorrelationStdDev()
      - It is possible to select only a subset of runs. In that case isCorrelationRunVisible() returns \c true if a run is really visible
        and to be used.
    .

 */
class QFRDRFCSDataInterface {
    public:
        virtual ~QFRDRFCSDataInterface() {}

        /** \brief number of correlation runs in this object */
        virtual int getCorrelationRuns() const=0;
        /** \brief number of datapoints in every correlation curve */
        virtual long long getCorrelationN() const=0;
        /** \brief sample points (times \f$ \tau \f$ ) of the correlation function.
         *         This is a 1D array of size correlationN */
        virtual double* getCorrelationT() const=0;
        /** \brief values of the correlation function.
         *         This is a 2D array of size runs * correlationN
         *
         * access this as \code correlation[run*correlationN + n] \endcode
         */
        virtual double* getCorrelation() const=0;
        /** \brief values of the correlation function for a specified run.
         *         This is a 1D array of size correlationN
         */
        virtual double* getCorrelationRun(int run) const=0;
        /** \brief errors, associated with the correlation function for a specified run.
         *         This is a 1D array of size correlationN
         */
        virtual double* getCorrelationRunError(int run) const=0;
        /** \brief values of the correlation function errors.
         *         This is a 2D array of size runs * correlationN
         *
         * access this as \code correlationRunErros[run*correlationN + n] \endcode
         */
        virtual double* getCorrelationRunErrors() const=0;
        /** \brief return a string naming/describing a run (should be short) */
        virtual QString getCorrelationRunName(int run) const=0;
        /** \brief values of the averaged correlation function (averaged over all runs).
         *         This is a 1D array of size correlationN */
        virtual double* getCorrelationMean() const=0;
        /** \brief values of the standard deviation of the correlation function (averaged over all runs).
         *         This is a 1D array of size correlationN */
        virtual double* getCorrelationStdDev() const=0;

        /** \brief returns true when a given run is visible. the average run is indicated by -1 */
        virtual bool isCorrelationRunVisible(int run) const=0;

};

Q_DECLARE_INTERFACE( QFRDRFCSDataInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRFCSDataInterface/1.1")



//
//
//
//        /** \brief recalculate correlation curve mean and standard deviation */
//        virtual void recalculateCorrelations()=0;
//
//
//        /** \brief channel represented by this object (a file e.g. from ALV may contain several channels,
//         *         but only one channel is represented by one object. This is saved in the property
//         *         CHANNEL. */
//        virtual int getChannel()=0;

//
//        /** \brief returns whether to leave out a run */
//        virtual bool leaveoutRun(int run)=0;
//        /** \brief add a run to the leaveouts */
//        virtual void leaveoutAddRun(int run)=0;
//        /** \brief remove a run from the leaveouts */
//        virtual void leaveoutRemoveRun(int run)=0;
//        /** \brief clear all leaveouts */
//        virtual void leaveoutClear()=0;
//        /** \brief set whether the given run is visible. the average run is indicated by -1 */
//        virtual void setRunVisible(int run, bool vis)=0;



#endif // QFRDRFCSDATAINTERFACE_H
