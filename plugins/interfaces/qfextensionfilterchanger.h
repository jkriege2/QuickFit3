/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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



#ifndef QFEXTENSIONFILTERCHANGER_H
#define QFEXTENSIONFILTERCHANGER_H


#include <QtPlugin>
#include <QWidget>
#include <QSettings>
#include "qfpluginservices.h"
#include <stdint.h>



/*! \brief QuickFit QFExtension to control filter wheels and comparable devices
    \ingroup qf3extensionplugins

    \note <b>Your implementation has to be thread-safe!!! So it can be called simultaneously from multiple threads, even when the invocations use shared data, because all references to the shared data are serialized</b>

 */
class QFExtensionFilterChanger {
    public:
        virtual ~QFExtensionFilterChanger() {}

        /** \brief returns the number of filter wheels controlable by this plugin */
        virtual unsigned int  getFilterChangerCount()  =0;
        /** \brief connect to a filter wheel controller/driver/.... */
        virtual void filterChangerConnect(unsigned int filterChanger)=0;
        /** \brief disconnect from a filter wheel controller/driver/.... */
        virtual void filterChangerDisonnect(unsigned int filterChanger)=0;
        /** \brief set QFPluginLogServices to use (or \c NULL) for message/error logging */
        virtual void setFilterChangerLogging(QFPluginLogService* logService)=0;

        /** \brief returns \c true if the device is connected */
        virtual bool isFilterChangerConnected(unsigned int filterChanger) =0;

        /** \brief return the number of filters selctable by the filter wheel */
        virtual unsigned int getFilterChangerFilterCount(unsigned int filterChanger) =0;

        /** \brief set filter in the given filter wheel */
        virtual void setFilterChangerFilter(unsigned int filterChanger, unsigned int filter) =0;

        /** \brief return the currently selected filter */
        virtual unsigned int getFilterChangerCurrentFilter(unsigned int filterChanger) =0;

        /** \brief return \c true if the last command, sent to the given filter wheel was executet, i.e. the filter wheel has settled to its new position
         *
         *  \note Use this to check, whether the filter wheel has reached its target position after a call of setFilterChangerFilter(), as the second
         *        one might not be defined (and return a wrong value) during the filter wheel movement!
         */
        virtual bool isLastFilterChangerActionFinished(unsigned int filterChanger)=0;
        /** \brief return a human-readable description for the given filter wheel */
        virtual QString getFilterChangerDescription(unsigned int filterChanger) =0;
        /** \brief return a human-readable short name for the given filter wheel */
        virtual QString getFilterChangerShortName(unsigned int filterChanger) =0;
        /*! \brief displays a modal dialog which allows the user to set the configuration options
                   of the controled filter wheel.

            The options are stored internally and written/read using QFExtension::readSettings() and
            QFExtension::writeSettings().

            \param[in] filterChanger the filter wheel for which to display the dialog
            \param[in] parent parent widget for the returned QWidget
         */
         virtual void showFilterChangerSettingsDialog(unsigned int filterChanger, QWidget* parent=NULL)=0;
};



Q_DECLARE_INTERFACE( QFExtensionFilterChanger,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionFilterChanger/1.0")

#endif // QFEXTENSIONFILTERCHANGER_H
