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


#ifndef QFE_ALEXCONTROL_H
#define QFE_ALEXCONTROL_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include "controlWidget.h"
#include "qfpluginoptionsdialog.h"


/*!
    \defgroup qf3ext_qfe_alexcontrol QFExtension implementation
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtension implementation
    \ingroup qf3ext_qfe_alexcontrol
 */
class QFE_ALEXControl : public QObject, public QFExtensionBase, public QFPluginOptionsDialogInterface {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFPluginOptionsDialogInterface)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFE_ALEXControl")
#endif	
    public:
        /** Default constructor */
        QFE_ALEXControl(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFE_ALEXControl();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("qfe_alexcontrol"); }
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("ALEX spFRET Control Plugin"); }
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return getName(); }
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Sebastian Isbaner, Jan W. Krieger (QuickFit 3 integration)"); }
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2013-2015 by Sebastian Isbaner"); }
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/"); }
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/qfe_alexcontrol/qfe_alexcontrol.png"); }
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
        QPointer<controlWidget> win;
        QAction* actStartPlugin;
	protected slots:
	    /** \brief target, used in example code in initExtension() */
        void startPlugin();

        void optionsFinished();
};

#endif // QFE_ALEXCONTROL_H
