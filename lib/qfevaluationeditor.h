#ifndef QFEVALUATIONEDITOR_H
#define QFEVALUATIONEDITOR_H

#include "lib_imexport.h"
#include <QWidget>
#include <QPointer>
#include "programoptions.h"
#include "qfpluginservices.h"

// forward declaration
class QFEvaluationItem;

/*! \brief base class for all evaluation widgets
    \ingroup qf3lib_project
*/
class QFLIB_EXPORT QFEvaluationEditor : public QWidget {
        Q_OBJECT
    public:
        /** Default constructor */
        QFEvaluationEditor(QFPluginServices* services, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFEvaluationEditor();
        /** \brief set the current record */
        virtual void setCurrent(QFEvaluationItem* c, int peID);
        /** \brief set the used ProgramOptions object */
        void setSettings(ProgramOptions* s, int peID) {
            this->peID=peID;
            settings=s;
            if (settings) readSettings();
        }
        void saveSettings() { writeSettings(); };
    protected slots:
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) =0;
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void resultsChanged() {};
    public slots:
        /** \brief read the settings */
        virtual void readSettings() =0;
        /** \brief write the settings */
        virtual void writeSettings() =0;
    protected:
        void closeEvent( QCloseEvent * event );
        /** \brief points to the record currently displayed */
        QPointer<QFEvaluationItem> current;
        /** \brief settinsg object used to store application settings */
        ProgramOptions* settings;
        /** \brief ID of the owning QFEvaluationPropertyEditor */
        int peID;
        /** \brief pointer that allows for access to central QuickFit services */
        QFPluginServices* services;

};



#endif // QFEVALUATIONEDITOR_H
