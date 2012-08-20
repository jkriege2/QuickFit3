#ifndef QFRDRNUMBERANDBRIGHTNESSEDITOR_H
#define QFRDRNUMBERANDBRIGHTNESSEDITOR_H

#include "qfrawdataeditor.h"
#include <QLabel>

/*! \brief editor for QFRawDataRecord
    \ingroup qf3rdrdp_number_and_brightness
*/
class QFRDRNumberAndBrightnessDataEditor : public QFRawDataEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRNumberAndBrightnessDataEditor(QFPluginServices* services,  QFRawDataPropertyEditor* propEditor, QWidget* parent);
        /** Default destructor */
        virtual ~QFRDRNumberAndBrightnessDataEditor();
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
    private:
};

#endif // QFRDRNUMBERANDBRIGHTNESSEDITOR_H
