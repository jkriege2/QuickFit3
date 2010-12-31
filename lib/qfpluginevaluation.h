#ifndef QFPLUGINEVALUATIONITEMBASE_H
#define QFPLUGINEVALUATIONITEMBASE_H

#include "qfpluginreportwrapper.h"
#include "programoptions.h"
#include "qfproject.h"
#include "qfevaluationitem.h"
#include <QWidget>


/** \brief base class for all QuickFit 3 raw data record plugins

*/
class QFPluginEvaluationItem {
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
        virtual QString getID()=0;

        /** \brief name for the plugin */
        virtual QString getName()=0;

        /** \brief short description for the plugin */
        virtual QString getDescription()=0;

        /** \brief author the plugin */
        virtual QString getAuthor()=0;

        /** \brief copyright information the plugin */
        virtual QString getCopyright()=0;

        /** \brief weblink for the plugin */
        virtual QString getWeblink()=0;

        /** \brief icon for the plugin */
        virtual QString getIconFilename()=0;

        /** \brief set current project */
        virtual void setProject(QFProject* project)=0;
        /** \brief set reporter class to use for reporting/status output */
        virtual void setReporter(QFPluginReportWrapper* reporter)=0;
        /** \brief set program options object used by the application */
        virtual void setSettings(ProgramOptions* settings)=0;
        /** \brief set parent widget (e.g. for message dialogs) */
        virtual void setParentWidget(QWidget* parentWidget)=0;
};


class QFPluginEvaluationItemBase: public QFPluginEvaluationItem {
    public:
        /** \brief class destructor */
        virtual ~QFPluginEvaluationItemBase() {};

        /** \brief set current project */
        virtual void setProject(QFProject* project) { this->project=project; };
        /** \brief set reporter class to use for reporting/status output */
        virtual void setReporter(QFPluginReportWrapper* reporter) {this->reporter=reporter; };
        /** \brief set program options object used by the application */
        virtual void setSettings(ProgramOptions* settings) {this->settings=settings; };
        /** \brief set parent widget (e.g. for message dialogs) */
        virtual void setParentWidget(QWidget* parentWidget) {this->parentWidget=parentWidget; };
    protected:
        QFPluginReportWrapper* reporter;
        QFProject* project;
        ProgramOptions* settings;
        QWidget* parentWidget;
};

Q_DECLARE_INTERFACE(QFPluginEvaluationItem,
                     "www.dkfz.de.b040.quickfit3.QFPluginEvaluationItem/1.0")


#endif // QFPLUGINEVALUATIONITEMBASE_H
