#ifndef QFPLUGINRAWDATARECORDBASE_H
#define QFPLUGINRAWDATARECORDBASE_H

#include "qfpluginservices.h"
#include "programoptions.h"
#include "qfproject.h"
#include <QWidget>
#include "qfrawdatarecord.h"
#include "qfplugin.h"

/** \brief virtual interface class for all QuickFit 3 raw data record plugins
    \ingroup qf3rawdataplugins

*/
class QFPluginRawDataRecord: public QFPlugin {
    public:
        /** \brief class destructor */
        virtual ~QFPluginRawDataRecord() {};

        /** \brief Create a new QFRawDataRecord in the given project \a parent. Also adds the record to the project. */
        virtual QFRawDataRecord* createRecord(QFProject* parent)=0;

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu)=0;


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
class QFPluginRawDataRecordBase: public QFPluginRawDataRecord {
    public:
        QF_PLUGIN

        /** \brief class destructor */
        virtual ~QFPluginRawDataRecordBase() {};

        /** \brief set current project */
        virtual void setProject(QFProject* project) { this->project=project; };
        /** \brief set services class to use for reporting/status output */
        virtual void setServices(QFPluginServices* services) { this->services=services; };
        /** \brief set program options object used by the application */
        virtual void setSettings(ProgramOptions* settings) { this->settings=settings; };
        /** \brief set parent widget (e.g. for message dialogs) */
        virtual void setParentWidget(QWidget* parentWidget) { this->parentWidget=parentWidget; };

    protected:
        QFPluginServices* services;
        QFProject* project;
        ProgramOptions* settings;
        QWidget* parentWidget;
};

Q_DECLARE_INTERFACE(QFPluginRawDataRecord,
                     "www.dkfz.de.b040.quickfit3.QFPluginRawDataRecord/1.0")

#endif // QFPLUGINRAWDATARECORDBASE_H
