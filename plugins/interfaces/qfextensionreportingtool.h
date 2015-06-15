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

#ifndef QFEXTENSIONREPORTINGTOOL_H
#define QFEXTENSIONREPORTINGTOOL_H


#include "qfplugin.h"
#include "qfpluginservices.h"
#include <stdint.h>
#include <QVariant>



/*! \brief QuickFit QFExtension implementing a tool that may have a return value
    \ingroup qf3extensionplugins
    
    \note this interface only has functions to get a return value, but as signals&slots may not be declared
          in interfaces does not have a signal, when the user selected a new values, so you'll have to poll
          every second (or so) to check whether there is a new value.

 */
class QFExtensionReportingTool {
    public:
        virtual ~QFExtensionReportingTool() {}

        /** \brief returns the last value selected by the user for the given name */
        virtual QVariant getReportingToolValue(const QString& name=QString(""))=0;
        
};



Q_DECLARE_INTERFACE( QFExtensionReportingTool,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionReportingTool/1.0")

#endif // QFEXTENSIONREPORTINGTOOL_H
