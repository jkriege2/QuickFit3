#ifndef QFIMFCSFITEVALUATIONEDITOR_H
#define QFIMFCSFITEVALUATIONEDITOR_H

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
#include "qffitparameterwidget.h"
#include "jkverticalscrollarea.h"
#include "dlgqffitalgorithmprogressdialog.h"
#include "qffitalgorithmthreaddedfit.h"
#include "qvisiblehandlesplitter.h"
#include "../base_classes/qffitresultsevaluationeditorbase.h"
#include "../base_classes/qffitresultsbyindexevaluationeditorbase.h"
#include "../base_classes/qffitresultsbyindexevaluationeditorwithwidgets.h"
#include "../interfaces/qfrdrimagetoruninterface.h"
#include "qffitresultsbyindexevaluationfitthread.h"
#include "dlgqfprogressdialog.h"


/*! \brief editor class for imaging FCS least-square fits
    \ingroup qf3evalp_imfcsfit


*/
class QFImFCSFitEvaluationEditor : public QFFitResultsByIndexEvaluationEditorWithWidgets {
        Q_OBJECT
    public:
        /** Default constructor */
        QFImFCSFitEvaluationEditor(QFPluginServices* services, QFEvaluationPropertyEditor* propEditor, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFImFCSFitEvaluationEditor();

        /** \brief get the lower datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int,int) with a viable choice for \a defaultMin */
        virtual int getUserMin(QFRawDataRecord* rec, int index);
        /** \brief get the upper datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int,int) with a viable choice for \a defaultMax */
        virtual int getUserMax(QFRawDataRecord* rec, int index);

        /** \brief get the lower datacut for the current record, reimplement this by calling getUserMin(,int) with a viable choice for \a defaultMin */
        virtual int getUserMin();
        /** \brief get the upper datacut for the current record, reimplement this by calling getUserMin(int) with a viable choice for \a defaultMax */
        virtual int getUserMax();
    public:

    protected slots:
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old);
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();


    protected:


        /** \brief combobox to select a model for the weighting */
        QComboBox* cmbWeights;

        QCheckBox* chkDontSaveFitResultMessage;




        /** \brief create a report in a given QTextDocument object
         *
         *  For correct sizing of the plots: set the textWidth of the document before calling this function!
         */
        virtual void createReportDoc(QTextDocument* document);

    protected slots:
        /** \brief display all data and parameters describing the current record */
        virtual void displayModel(bool newWidget);
        virtual void highlightingChanged(QFRawDataRecord *formerRecord, QFRawDataRecord *currentRecord);

        /** \brief replot curves */
        virtual void replotData();

        /*! \brief update the fit model function and the residuals in the graphs

            This function only adds the fit function plot objects and does NOT cause a replot of the graphs. It is called by
            replotData().
        */
        virtual void updateFitFunctions();

        /** \brief called when the weights changes */
        void weightsChanged(int weight);


        void dontSaveFitResultMessageChanged(bool checked);

    private:
        /** \brief create all widgets on the form */
        void createWidgets();



    public:

        /* explicitly make some functions visible again, as the C++ compiler hides function definitions
           from a base class that are also declared in the derived class, but with different parameter sets!

           see http://www.parashift.com/c++-faq-lite/strange-inheritance.html#faq-23.9
         */

        using QFFitResultsByIndexEvaluationEditorBase::getUserMin;
        using QFFitResultsByIndexEvaluationEditorBase::getUserMax;



        virtual int getUserRangeMax(QFRawDataRecord *rec, int index);
        virtual int getUserRangeMin(QFRawDataRecord *rec, int index);
};

#endif // QFIMFCSFITEVALUATIONEDITOR_H
