#ifndef QFFCSMAXENTEVALUATIONEDITOR_H
#define QFFCSMAXENTEVALUATIONEDITOR_H

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include <QProgressDialog>
#include <QTextDocument>

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include "qt/jkqtplotter.h"
#include "tools.h"
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QToolBar>
#include <QToolButton>
#include <QAction>
#include <QList>
#include <QPointer>
#include <QFormLayout>
#include <QScrollArea>
#include <QSpinBox>
#include <QTextEdit>
#include <QToolBox>
#include <QTabWidget>
#include <QGridLayout>
#include <QTabBar>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <QTextDocument>
#include "datacutslider.h"
#include "qfhtmlhelpwindow.h"
#include "jkverticalscrollarea.h"
#include "qvisiblehandlesplitter.h"
#include "../interfaces/qfrdrfcsdatainterface.h"
#include "qfsimplefitparameterswidget.h"
#include "qfusesresultsbyindexevaluationeditor.h"
#include "qffcsbyindexandmodelevaluationeditor.h"


/*! \brief editor for QFFCSMaxEntEvaluationItem
    \ingroup qf3rdrdp_GROUPNAME
*/
class QFFCSMaxEntEvaluationEditor : public QFFCSByIndexAndModelEvaluationEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFFCSMaxEntEvaluationEditor(QFPluginServices* services, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFFCSMaxEntEvaluationEditor();



    protected slots:
        void alphaChanged(double alpha);
        void weightsChanged(int weights);


        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old);
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();


        /** \brief activated when the highlighted record changed */
        virtual void highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord);

        /** \brief evaluate all files */
        virtual void fitRunsAll();
        /** \brief evaluate current file */
        virtual void fitCurrent();
        /** \brief display the current evaluation results */
        virtual void fitAll();

        virtual void fitRunsCurrent();

        virtual void displayData();
        virtual void updateFitFunctions();

    protected:       
        /*! \brief create an evaluation report for the current record */
        void createReportDoc(QTextDocument* document);

        /** \brief number edit to change alpha */
        NumberEdit* edtAlpha;
        /** \brief combobox to select a model for the weighting */
        QComboBox* cmbWeights;

    private:
        void createWidgets();

};

#endif // QFFCSMAXENTEVALUATIONEDITOR_H
