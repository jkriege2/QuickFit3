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

#ifndef QFEXTENSIONTMCLFILTERCHANGER_H
#define QFEXTENSIONTMCLFILTERCHANGER_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include <QVector>
#include <QList>
#include "qfextension.h"
#include "qfextensionfilterchanger.h"
#include "../base_classes/qf3tmclprotocolhandler.h"
#include "qf3comportmanager.h"
#include "qfserialconnection.h"
#include "qfextensionglobalsettingsreadwrite.h"

/*!
    \defgroup qf3ext_filterc_tmcl QFExtensioFilterChanger test device
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtensioFilterChanger test device implementation
    \ingroup qf3ext_filterc_tmcl
 */
class QFExtensionTMCLFilterChanger : public QObject, public QFExtensionBase, public QFExtensionFilterChanger, public QFExtensionGlobalSettingsReadWrite {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFExtensionFilterChanger QFExtensionGlobalSettingsReadWrite)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFExtensionTMCLFilterChanger")
#endif

    public:
        /** Default constructor */
        QFExtensionTMCLFilterChanger(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFExtensionTMCLFilterChanger();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("filterc_tmcl"); }
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("TMCL Filter Wheel"); }
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("filter wheel controlled by a stepper motor and TMCL"); }
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); }
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2012 by J. Krieger"); }
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); }
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/filterc_tmcl/qfextensiontmclfilterchanger.png"); }
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        }
        /** \copydoc QFExtension::deinit() */
        virtual void deinit();














        /** \copydoc QFExtensionFilterChanger::getFilterChangerCount() */
        virtual unsigned int getFilterChangerCount()  ;
        /** \copydoc QFExtensionFilterChanger::filterChangerConnect() */
        virtual void filterChangerConnect(unsigned int filterChanger);
        /** \copydoc QFExtensionFilterChanger::filterChangerDisonnect() */
        virtual void filterChangerDisonnect(unsigned int filterChanger);
        /** \copydoc QFExtensionFilterChanger::setFilterChangerLogging() */
        virtual void setFilterChangerLogging(QFPluginLogService* logService);

        /** \copydoc QFExtensionFilterChanger::isFilterChangerConnected() */
        virtual bool isFilterChangerConnected(unsigned int filterChanger) ;

        /** \copydoc QFExtensionFilterChanger::getFilterChangerFilterCount() */
        virtual unsigned int getFilterChangerFilterCount(unsigned int filterChanger) ;

        /** \copydoc QFExtensionFilterChanger::setFilterChangerFilter() */
        virtual void setFilterChangerFilter(unsigned int filterChanger, unsigned int filter) ;

        /** \copydoc QFExtensionFilterChanger::getFilterChangerCurrentFilter() */
        virtual unsigned int getFilterChangerCurrentFilter(unsigned int filterChanger) ;

        /** \copydoc QFExtensionFilterChanger::isLastFilterChangerActionFinished() */
        virtual bool isLastFilterChangerActionFinished(unsigned int filterChanger);
        /** \copydoc QFExtensionFilterChanger::getFilterChangerDescription() */
        virtual QString getFilterChangerDescription(unsigned int filterChanger) ;
        /** \copydoc QFExtensionFilterChanger::getFilterChangerShortName() */
        virtual QString getFilterChangerShortName(unsigned int filterChanger) ;
        /** \copydoc QFExtensionFilterChanger::showFilterChangerSettingsDialog() */
        virtual void showFilterChangerSettingsDialog(unsigned int filterChanger, QWidget* parent=NULL);




        /** \copydoc QFExtensionGlobalSettingsReadWrite::readGlobalSettings() */
        virtual void readGlobalSettings(QSettings& settings, const QString& prefix=QString(""));
        /** \copydoc QFExtensionGlobalSettingsReadWrite::writeGlobalSettings() */
        virtual void writeGlobalSettings(QSettings& settings, const QString& prefix=QString("")) ;


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
        QF3ComPortManager ports;
        QMenu* fcMenu;

        struct FILTERWHEEL {
            QF3TMCLProtocolHandler* tmcl;
            int port;
            int id;
            QString infoMessage;
            QTime lastAction;
            int filters;
            uint64_t steps_per_revolution;
            int32_t maximumSpeed;
            int32_t maximumAcceleration;
            int32_t maximumCurrent;
            int32_t standbyCurrent;
            int32_t slowRFSSpeed;
            int32_t fastRFSSpeed;
            QString motorName;
            int currentFilter;
            uint8_t motor;
            QAction* actRealign;
        };

        QList<FILTERWHEEL> wheels;

        int findActRealign(QAction* realign);

        bool TMCLRealignFW(int filterChanger);

    protected slots:
        void realignFW();



};

#endif // QFEXTENSIONTMCLFILTERCHANGER_H
