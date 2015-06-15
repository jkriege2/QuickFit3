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

#ifndef QFRDRIMFCSCORRELATORREMOTE_H
#define QFRDRIMFCSCORRELATORREMOTE_H

#include "qfplugin.h"
#include <stdint.h>

/*! \brief allows to remotely control an imFCS correltor plugin
    \ingroup qf3rdrdp_imaging_fcs

*/
class QFRDRIMFCSCorrelatorRemote {
    public:
        virtual ~QFRDRIMFCSCorrelatorRemote() {}

        virtual void imfcsCorrRemoteUserSelectFile()=0;
        virtual void imfcsCorrRemoteSelectFile(const QString& filename)=0;
        virtual void imfcsCorrRemoteAddJob()=0;
        virtual void imfcsCorrRemoteAddJobSeries(const QString& parameter, double start, double end, double inc=1)=0;

};

Q_DECLARE_INTERFACE( QFRDRIMFCSCorrelatorRemote,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRIMFCSCorrelatorRemote/1.0")

#endif // QFRDRIMFCSCORRELATORREMOTE_H
