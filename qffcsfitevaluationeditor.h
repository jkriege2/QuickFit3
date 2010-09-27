#ifndef QFFCSFITEVALUATIONEDITOR_H
#define QFFCSFITEVALUATIONEDITOR_H

#include "qfevaluationeditor.h"
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
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old);
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void resultsChanged();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();
    protected:
    private:
};

#endif // QFFCSFITEVALUATIONEDITOR_H
