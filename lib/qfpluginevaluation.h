/*
    Copyright (c) 2008-2014 Jan W. Krieger, German Cancer Research Center

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

#ifndef QFPLUGINEVALUATIONITEMBASE_H
#define QFPLUGINEVALUATIONITEMBASE_H

#include "lib_imexport.h"
#include "qfpluginservices.h"
#include "programoptions.h"
#include "qfproject.h"
#include "qfevaluationitem.h"
#include <QWidget>
#include "qfplugin.h"
#include "lib_imexport.h"

/** \brief base class for all QuickFit 3 raw data record plugins
    \ingroup qf3evaluationplugins

*/
class QFPluginEvaluationItem: public QFPlugin, public QFPluginAdditionalPluginsInterface {
    public:
        /** \brief class destructor */
        virtual ~QFPluginEvaluationItem() {};

        /** \brief Create a new QFRawDataRecord in the given project \a parent. Also adds the record to the project. */
        virtual QFEvaluationItem* createRecord(QFProject* parent)=0;

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu)=0;

        /** \brief short ID for the plugin */
        virtual QString getID() const =0;

        /** \brief deinit plugin, this function is guaranteed to be called once, before  */
        virtual void deinit()=0;

        /** \brief init plugin, this function is guaranteed to be called once, before  */
        virtual void init()=0;


        /** \brief set current project */
        virtual void setProject(QFProject* project)=0;
        /** \brief set services class to use for reporting/status output */
        virtual void setServices(QFPluginServices* services)=0;
        /** \brief set program options object used by the application */
        virtual void setSettings(ProgramOptions* settings)=0;
        /** \brief set parent widget (e.g. for message dialogs) */
        virtual void setParentWidget(QWidget* parentWidget)=0;
};


class QFLIB_EXPORT QFPluginEvaluationItemBase: public QFPluginEvaluationItem {
    public:
        QF_PLUGIN

        /** \brief class destructor */
        virtual ~QFPluginEvaluationItemBase() {};

        /** \brief set current project */
        virtual void setProject(QFProject* project) { this->project=project; };
        /** \brief set services class to use for reporting/status output */
        virtual void setServices(QFPluginServices* services) {this->services=services; };
        /** \brief set program options object used by the application */
        virtual void setSettings(ProgramOptions* settings) {this->settings=settings; };
        /** \brief set parent widget (e.g. for message dialogs) */
        virtual void setParentWidget(QWidget* parentWidget) {this->parentWidget=parentWidget; };

        /** \brief deinit plugin, this function is guaranteed to be called once, before  */
        virtual void deinit() {};
        /** \brief init plugin, this function is guaranteed to be called once, before  */
        virtual void init() {};

        /** \copydoc QFPluginAdditionalPluginsInterface::getAdditionalRDRPlugins() */
        virtual QList<QFPluginRawDataRecord*> getAdditionalRDRPlugins() const { return QList<QFPluginRawDataRecord*>(); };

        /** \copydoc QFPluginAdditionalPluginsInterface::getAdditionalEvaluationPlugins() */
        virtual QList<QFPluginEvaluationItem*> getAdditionalEvaluationPlugins() const  { return QList<QFPluginEvaluationItem*>(); };

        /** \copydoc QFPluginAdditionalPluginsInterface::getAdditionalExtensionPlugins() */
        virtual QList<QFExtension*> getAdditionalExtensionPlugins() const { return QList<QFExtension*>(); };

    protected:
        QFPluginServices* services;
        QFProject* project;
        ProgramOptions* settings;
        QWidget* parentWidget;

        virtual void log_text(const QString& message) {
            if (services) services->log_text(message);
        }
        virtual void log_warning(const QString& message) {
            if (services) services->log_warning(message);
        }
        virtual void log_error(const QString& message) {
            if (services) services->log_error(message);
        }
};


Q_DECLARE_INTERFACE(QFPluginEvaluationItem,
                     "www.dkfz.de.b040.quickfit3.QFPluginEvaluationItem/1.1")


#endif // QFPLUGINEVALUATIONITEMBASE_H
