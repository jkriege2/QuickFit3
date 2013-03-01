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
class QFPluginEvaluationItem: public QFPlugin {
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
                     "www.dkfz.de.b040.quickfit3.QFPluginEvaluationItem/1.0")


#endif // QFPLUGINEVALUATIONITEMBASE_H
