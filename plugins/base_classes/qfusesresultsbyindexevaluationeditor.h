#ifndef QFUSESRESULTSBYINDEXEVALUATIONEDITOR_H
#define QFUSESRESULTSBYINDEXEVALUATIONEDITOR_H
#include "qfusesresultsbyindexevaluation.h"

#include "qfevaluationeditor.h"

class QFUsesResultsByIndexEvaluationEditor : public QFEvaluationEditor {
        Q_OBJECT
    public:
        explicit QFUsesResultsByIndexEvaluationEditor(QFPluginServices* services, QFEvaluationPropertyEditor* propEditor, QWidget *parent = 0);
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        virtual int getUserMin(QFRawDataRecord *rec, int index, int defaultMin);
        virtual int getUserMax(QFRawDataRecord *rec, int index, int defaultMax);
        virtual int getUserMin(QFRawDataRecord *rec);
        virtual int getUserMax(QFRawDataRecord *rec);
        virtual int getUserMax(QFRawDataRecord *rec, int index);
        virtual int getUserMin(QFRawDataRecord *rec, int index);

    public slots:
        virtual void setUserMin(int userMin);
        virtual void setUserMax(int userMax);
        virtual void setUserMinMax(int userMin, int userMax);
        virtual void copyUserMinToAll(int userMin);
        virtual void copyUserMaxToAll(int userMax);
        virtual void copyUserMinMaxToAll(int userMin, int userMax);
        virtual void copyUserMinToAllRuns(int userMin);
        virtual void copyUserMaxToAllRuns(int userMax);
        virtual void copyUserMinMaxToAllRuns(int userMin, int userMax);
    signals:
        
    protected:
        virtual int getUserMinAbsMax(QFRawDataRecord *rec, int index);
        
};

#endif // QFUSESRESULTSBYINDEXEVALUATIONEDITOR_H
