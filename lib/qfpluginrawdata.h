/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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

#ifndef QFPLUGINRAWDATARECORDBASE_H
#define QFPLUGINRAWDATARECORDBASE_H

#include "lib_imexport.h"
#include "qfpluginservices.h"
#include "programoptions.h"
#include "qfproject.h"
#include <QWidget>
#include "qfrawdatarecord.h"
#include "qfplugin.h"
#include "lib_imexport.h"



/** \brief virtual interface class for all QuickFit 3 raw data record plugins
    \ingroup qf3rawdataplugins

*/
class QFPluginRawDataRecord: public QFPlugin, public QFPluginAdditionalPluginsInterface {
    public:
        /** \brief class destructor */
        virtual ~QFPluginRawDataRecord() {};

        /** \brief Create a new QFRawDataRecord in the given project \a parent. Also adds the record to the project. */
        virtual QFRawDataRecord* createRecord(QFProject* parent)=0;

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu)=0;
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

/** \brief virtual base class for all QuickFit 3 raw data record plugins
    \ingroup qf3rawdataplugins

    Use this class as base when developing raw data record plugins. This class implements some of
    the methods from QFPluginRawDataRecord, as they are generic to all plugins!

*/
class QFLIB_EXPORT QFPluginRawDataRecordBase: public QFPluginRawDataRecord {
    public:
        /** \brief class constructor */
        QFPluginRawDataRecordBase();

        /** \brief class destructor */
        virtual ~QFPluginRawDataRecordBase();

        //QF_PLUGIN
        virtual void getQFLibVersion(int& major, int& minor) const;

        /** \brief set current project */
        virtual void setProject(QFProject* project);
        /** \brief set services class to use for reporting/status output */
        virtual void setServices(QFPluginServices* services);
        /** \brief set program options object used by the application */
        virtual void setSettings(ProgramOptions* settings);
        /** \brief set parent widget (e.g. for message dialogs) */
        virtual void setParentWidget(QWidget* parentWidget);
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
};

Q_DECLARE_INTERFACE(QFPluginRawDataRecord,
                     "www.dkfz.de.b040.quickfit3.QFPluginRawDataRecord/1.1")

#endif // QFPLUGINRAWDATARECORDBASE_H
