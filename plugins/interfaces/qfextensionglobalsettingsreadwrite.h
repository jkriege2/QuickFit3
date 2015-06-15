/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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



#ifndef QFEXTENSIONGLOBALSETTINGSREADWRITE_H
#define QFEXTENSIONGLOBALSETTINGSREADWRITE_H



#include "qfplugin.h"
#include <QWidget>
#include <QSettings>
#include "qfpluginservices.h"
#include <stdint.h>



/*! \brief Functions in this interface should be used to read and write global settings for hardware control QFExtensions
    \ingroup qf3extensionplugins

 */
class QFExtensionGlobalSettingsReadWrite {
    public:
        virtual ~QFExtensionGlobalSettingsReadWrite() {}

        /** \brief read the global settings from the given QSettings object. Each name is prepended the supplied \a prefix */
        virtual void readGlobalSettings(QSettings& settings, const QString& prefix=QString("")) =0;
        /** \brief save the global settings to the given QSettings object. Each name is prepended the supplied \a prefix */
        virtual void writeGlobalSettings(QSettings& settings, const QString& prefix=QString("")) =0;

};

Q_DECLARE_INTERFACE( QFExtensionGlobalSettingsReadWrite,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionGlobalSettingsReadWrite/1.0")

#endif // QFEXTENSIONGLOBALSETTINGSREADWRITE_H
