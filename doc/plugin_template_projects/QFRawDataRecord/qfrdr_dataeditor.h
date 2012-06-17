#ifndef HEADER_HEDITOR_H
#define HEADER_HEDITOR_H

#include "qfrawdataeditor.h"
#include <QLabel>

/*! \brief editor for QFRawDataRecord
    \ingroup qf3rdrdp_GROUPNAME
*/
class QFRDRDataEditor : public QFRawDataEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRDataEditor(QFPluginServices* services,  QFRawDataPropertyEditor* propEditor, QWidget* parent);
        /** Default destructor */
        virtual ~QFRDRDataEditor();
    protected slots:
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged();
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old);
    protected:
        /** \brief create widgets on object creation */
        void createWidgets();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        /** \brief label for test */
        QLabel* labTest;
    private:
};

#endif // HEADER_HEDITOR_H
