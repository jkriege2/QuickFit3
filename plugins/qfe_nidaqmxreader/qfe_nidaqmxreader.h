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


#ifndef QFENIDAQMXREADER_H
#define QFENIDAQMXREADER_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include "mainwindow.h"
#include "qfpluginoptionsdialog.h"

/*!
    \defgroup qf3ext_qfe_nidaqmxreader QFExtension implementation
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtension implementation
    \ingroup qf3ext_qfe_nidaqmxreader
 */
class QFENIDAQmxReader : public QObject, public QFExtensionBase, public QFPluginOptionsDialogInterface {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFPluginOptionsDialogInterface)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFENIDAQmxReader")
#endif	
    public:
        /** Default constructor */
        QFENIDAQmxReader(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFENIDAQmxReader();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("qfe_nidaqmxreader"); }
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("NI-DAQmx Channel-Reader"); }
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("reads measurements from input-channels of one or more NI-DAQmx cards"); }
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); }
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2015 by Jan W. Krieger"); }
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); }
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/qfe_nidaqmxreader/qfe_nidaqmxreader_large.png"); }
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };
        /** \copydoc QFExtension::deinit() */
        virtual void deinit();

        QString pluginOptionsName() const;
        QIcon pluginOptionsIcon() const;
        QFPluginOptionsWidget *createOptionsWidget(QWidget *parent);

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
        QPointer<MainWindow> win;
        QAction* actStartPlugin;
		
	protected slots:
	    /** \brief target, used in example code in initExtension() */
        void startPlugin();

        void optionsFinished();

};

#endif // QFENIDAQMXREADER_H
