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

#ifndef QFEXTENSIONTOOL_H
#define QFEXTENSIONTOOL_H


#include "qfplugin.h"
#include <QWidget>
#include <QSettings>
#include "qfpluginservices.h"
#include <stdint.h>
#include <QAction>



/*! \brief QuickFit QFExtension implementing a tool
    \ingroup qf3extensionplugins
    


 */
class QFExtensionTool {
    public:
        virtual ~QFExtensionTool() {}

        /** \brief returns an action that may be used to start the tool */
        virtual QAction* getToolStartAction()=0;
        /** \brief starts the tool */
        virtual void startTool()=0;
        
};



Q_DECLARE_INTERFACE( QFExtensionTool,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionTool/1.0")

#endif // QFEXTENSIONTOOL_H
