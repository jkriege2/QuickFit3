#ifndef QFFCSFITEVALUATIONEDITOR_H
#define QFFCSFITEVALUATIONEDITOR_H

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include "../lib/qt/jkqtplotter.h"


class QFFCSFitEvaluationEditor : public QFEvaluationEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFFCSFitEvaluationEditor(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFFCSFitEvaluationEditor();
    protected slots:
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(const QFEvaluationItem* current, const QFEvaluationItem* old);
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void resultsChanged();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();
    protected:
        /** \brief label displaying the current record */
        QLabel* labRecord;
    protected slots:
        /** \brief activated when the highlighted record changed */
        void highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord);
    private:
        /** \brief create all widgets on the form */
        void createWidgets();
};

#endif // QFFCSFITEVALUATIONEDITOR_H
