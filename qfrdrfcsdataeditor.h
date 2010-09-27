#ifndef QFRDRFCSDATAEDITOR_H
#define QFRDRFCSDATAEDITOR_H

#include "qfrawdataeditor.h"
#include "../lib/qt/jkqtplotter.h"


class QFRDRFCSDataEditor : public QFRawDataEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRFCSDataEditor(QWidget* parent);
        /** Default destructor */
        virtual ~QFRDRFCSDataEditor();
    protected:
        /** \brief create widgets on object creation */
        void createWidgets();
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old);
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();
        /** \brief displays the data from the current data element in the plotter widget */
        void replotData();

        /** \brief plotter widget for the correlation curve */
        JKQtPlotter* plotter;
    private:
};

#endif // QFRDRFCSDATAEDITOR_H
