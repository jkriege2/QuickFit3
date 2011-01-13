#ifndef QFRAWDATAEDITOR_H
#define QFRAWDATAEDITOR_H

#include <QWidget>
#include "programoptions.h"
#include <iostream>
#include "qfpluginservices.h"


// forward declaration
class QFRawDataRecord;

/*! \brief base class for all raw data editors used to display/edit the raw data within QFRawDataPropertyEditor window
    \ingroup qf3lib_project

 */
class QFRawDataEditor : public QWidget {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRawDataEditor(QFPluginServices* services, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFRawDataEditor();
        /** \brief set the current record */
        virtual void setCurrent(QFRawDataRecord* c, int peID);
        /** \brief set the used ProgramOptions object */
        void setSettings(ProgramOptions* s, int peID);
        /** \brief save the settings to the ProgramOptions object set by setSettings() */
        void saveSettings() { writeSettings(); };
    protected slots:
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) =0;
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged() {};
    public slots:
        /** \brief read the settings */
        virtual void readSettings() =0;
        /** \brief write the settings */
        virtual void writeSettings() =0;
    protected:
        virtual void closeEvent( QCloseEvent * event );
        /** \brief points to the record currently displayed */
        QFRawDataRecord* current;
        /** \brief settinsg object used to store application settings */
        ProgramOptions* settings;
        /** \brief the ID of the parent QFRawDataPropertyEditor */
        int peID;
        /** \brief pointer that allows for access to central QuickFit services */
        QFPluginServices* services;
    private:
};



#endif // QFRAWDATAEDITOR_H
