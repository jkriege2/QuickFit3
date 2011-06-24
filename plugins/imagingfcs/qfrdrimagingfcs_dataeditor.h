#ifndef QFRDRIMAGINGFCSEDITOR_H
#define QFRDRIMAGINGFCSEDITOR_H

#include "qfrawdataeditor.h"
#include <QLabel>

/*! \brief editor for QFRawDataRecord
    \ingroup qf3rdrdp_imaging_fcs
*/
class QFRDRImagingFCSDataEditor : public QFRawDataEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRImagingFCSDataEditor(QFPluginServices* services, QWidget* parent);
        /** Default destructor */
        virtual ~QFRDRImagingFCSDataEditor();
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

#endif // QFRDRIMAGINGFCSEDITOR_H
