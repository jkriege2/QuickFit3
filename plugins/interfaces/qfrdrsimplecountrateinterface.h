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

#ifndef QFRDRSIMPLECOUNTRATEINTERFACE_H
#define QFRDRSIMPLECOUNTRATEINTERFACE_H

#include <QtPlugin>

/*! \brief interface for simple count rate information
    \ingroup qf3rdrdp_fcs

*/
class QFRDRSimpleCountRatesInterface {
    public:
        virtual ~QFRDRSimpleCountRatesInterface() {};
        /** \brief returns the average count rate for the given run (or a common value for index<0) */
        virtual double getSimpleCountrateAverage(int run=-1, int channel=0, bool swapChannels=false) const=0;
        /** \brief returns the variance of the count rate for the given run (or a common value for index<0) */
        virtual double getSimpleCountrateStdDev(int run=-1, int channel=0, bool swapChannels=false) const=0;
        /** \brief returns the number of countrate channels */
        virtual int getSimpleCountrateChannels() const=0;
};

Q_DECLARE_INTERFACE( QFRDRSimpleCountRatesInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRSimpleCountRatesInterface/1.1")

#endif // QFRDRSIMPLECOUNTRATEINTERFACE_H
