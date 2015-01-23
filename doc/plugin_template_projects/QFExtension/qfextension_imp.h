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


#ifndef HEADER_H
#define HEADER_H

#include <time.h>
#include <QObject>
#include "qfextension.h"

/*!
    \defgroup qf3ext_GROUPNAME QFExtension implementation
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtension implementation
    \ingroup qf3ext_GROUPNAME
 */
class QFExtensionImplementation : public QObject, public QFExtensionBase {
        Q_OBJECT
        Q_INTERFACES(QFExtension)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFExtensionImplementation")
#endif	
    public:
        /** Default constructor */
        QFExtensionImplementation(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFExtensionImplementation();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("target_id"); };
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr(/* human readable name for plugin */); };
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr(/* human-readable description for plugin */); };
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr(/* author name */); };
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr(/* copyright information */); };
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr(/* your homepage */); };
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/target_id/qfextension_imp.png"); };
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };
        /** \copydoc QFExtension::deinit() */
        virtual void deinit();

    protected:
        /** \copydoc QFExtensionBase::projectChanged() */
        virtual void projectChanged(QFProject* oldProject, QFProject* project);
        /** \copydoc QFExtensionBase::initExtension() */
        virtual void initExtension();
        /** \copydoc QFExtensionBase::loadSettings() */
        virtual void loadSettings(ProgramOptions* settings);
        /** \copydoc QFExtensionBase::storeSettings() */
        virtual void storeSettings(ProgramOptions* settings);



        /** \brief log project text message
         *  \param message the message to log
         */
        virtual void log_text(QString message);
        /** \brief log project warning message
         *  \param message the warning message to log
         */
        virtual void log_warning(QString message);
        /** \brief log project error message
         *  \param message the error message to log
         */
        virtual void log_error(QString message);

	protected:
        QFPluginLogService* logService;
		
	protected slots:
	    /** \brief target, used in example code in initExtension() */
	    //void startPlugin();

};

#endif // HEADER_H
