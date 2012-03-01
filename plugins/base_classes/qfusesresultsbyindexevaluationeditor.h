#ifndef QFUSESRESULTSBYINDEXEVALUATIONEDITOR_H
#define QFUSESRESULTSBYINDEXEVALUATIONEDITOR_H
#include "qfusesresultsbyindexevaluation.h"

#include "qfevaluationeditor.h"

class QFUsesResultsByIndexEvaluationEditor : public QFEvaluationEditor {
        Q_OBJECT
    public:
        explicit QFUsesResultsByIndexEvaluationEditor(QFPluginServices* services, QWidget *parent = 0);
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();
    signals:
        
    public slots:
        
};

#endif // QFUSESRESULTSBYINDEXEVALUATIONEDITOR_H
