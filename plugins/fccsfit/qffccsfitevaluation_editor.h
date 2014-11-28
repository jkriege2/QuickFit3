/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFFCCSFITEVALUATIONEDITOR_H
#define QFFCCSFITEVALUATIONEDITOR_H

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include <QProgressDialog>
#include <QTextDocument>
#include "qffitfunctionplottools.h"
#include "jkqtplotter.h"
#include "qffcsweightingtools.h"
#include "dlgqffitalgorithmprogressdialog.h"
#include "qffitfunctionconfigforglobalfitinterface.h"

namespace Ui {
    class QFFCCSFitEvaluationEditor;
}

class QFFCCSFitEvaluationItem; // forward

/*! \brief editor for QFFCCSFitEvaluationItem
    \ingroup qf3rdrdp_FCCSfit

    The main part of this editor is a QTableView which allows to edit the fit parameters of several files.
    A special item delegate is used to create special editing widgets for the values and to select fit functions and RDRs.
*/
class QFFCCSFitEvaluationEditor : public QFEvaluationEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFFCCSFitEvaluationEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFFCCSFitEvaluationEditor();


        /** \brief get the lower datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int,int) with a viable choice for \a defaultMin */
        virtual int getUserMin(int index);
        /** \brief get the upper datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int,int) with a viable choice for \a defaultMax */
        virtual int getUserMax(int index);
        /** \brief get the lower datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int,int) with a viable choice for \a defaultMin */
        virtual int getUserMin(QFRawDataRecord* rec, int index);
        /** \brief get the upper datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int,int) with a viable choice for \a defaultMax */
        virtual int getUserMax(QFRawDataRecord* rec, int index);
        virtual int getUserRangeMax(QFRawDataRecord *rec, int index);
        virtual int getUserRangeMin(QFRawDataRecord *rec, int index);
        /** \brief get the lower datacut for the current record, the \a defaultMin should be read from editors current data cut widget */
        virtual int getUserMin(QFRawDataRecord* rec, int index, int defaultMin);
        /** \brief get the upper datacut for the current record, the \a defaultMax should be read from editors current data cut widget */
        virtual int getUserMax(QFRawDataRecord* rec, int index, int defaultMax);
        /** \brief set the upper and lower datacut for the current record */
        virtual void setUserMinMax(int userMin, int userMax);

    protected slots:
        /** \brief executed when the sliders values change */
        void slidersChanged(int userMin, int userMax, int min, int max);


        /** \brief this signal is emited when the user clicks the "copy to all" context menu of the min widget item. */
        void slidersCopyUserMinToAll(int userMin);

        /** \brief this signal is emited when the user clicks the "copy to all" context menu of the max widget item. */
        void slidersCopyUserMaxToAll(int userMax);

        /** \brief this signal is emited when the user clicks the "copy to all runs" context menu of the max widget item. */
        void slidersCopyUserMinMaxToAll(int userMin, int userMax);

        void setUserMinMaxInternal(QFFCCSFitEvaluationItem* data, QFRawDataRecord* rdr, int userMin, int userMax);
        void setUserMinInternal(QFFCCSFitEvaluationItem* data, QFRawDataRecord* rdr, int userMin);
        void setUserMaxInternal(QFFCCSFitEvaluationItem* data, QFRawDataRecord* rdr, int userMax);



        void zoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter *sender);
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old);
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void resultsChanged();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        /** \brief display fit algorithm help */
        void showFitAlgorithmHelp();
        /** \brief configure the fitting algorithm */
        void configFitAlgorithm();
        /** \brief called when the fit algorithm changes */
        void fitAlgorithmChanged(int model);

        void displayOptionsChanged();

        void on_cmbWeight_currentWeightChanged(QFFCSWeightingTools::DataWeight weight);

        void setCurrentRun(int run);

        void configureForSPFCCS();
        void configureForASPFCCS();
        void configureFor2CSPIMFCCS();
        void onConfigureGlobalItemClicked();
        void configureFitFromGlobal(const QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig& config, bool loadParams);
        QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig getCurrentGlobalFitConfig() const;

        void filesSetActivated(const QModelIndex& idx);
        void on_chkSaveStrings_toggled(bool checked);
        void on_btnEditRanges_toggled(bool enabled);
        void setParameterVisibility();
        void on_btnAddFile_clicked();
        void on_btnRemoveFile_clicked();

        void loadGlobalFitConfig();
        void saveGlobalFitConfig();
    protected:
        /** \brief the user interface object (using QtDesigner) */
        Ui::QFFCCSFitEvaluationEditor *ui;
        
        QList<QFFitFunctionConfigForGlobalFitInterface::GlobalFitConfig> globalConfig;
        QList<QAction*> actsGlobalConfig;
        QList<QMenu*> menusGlobalConfig;

        void buildGlobalConfigs(QFFCCSFitEvaluationItem* current);

        /** \brief where did we save the last report */
        QString currentSaveDirectory;
        QAction* actConfigureForNormalFCCS;
        QAction* actConfigureFor2CFCCS;
        QAction* actConfigureForAnomalousFCCS;
        QAction* actFitCurrent;
        QAction* actFitAllRunsMT;
        QAction* actFitAllRuns;
        QAction* actFitAllFilesetsAllPixels;
        QAction* actPrintReport;
        QAction* actSaveReport;
        QAction* actGuess;
        QAction* actLoadGLobalFitConfig;
        QAction* actSaveGLobalFitConfig;


        QAction* actResetCurrent;
        QAction* actResetAllRuns;
        QAction* actResetAllPixelsInAllFilesets;
        QAction* actCopyToInitial;

        QMenu* menuFCCSFit;
        QMenu* menuEvaluation;

        /** \brief fit progress dialog */
        dlgQFFitAlgorithmProgressDialog* dlgFitProgress;
        /** \brief reporter for dlgFitProgress */
        dlgQFFitAlgorithmProgressDialogReporter* dlgFitProgressReporter;

        /*! \brief a flag that indicates whether we are currently updating the display data 
        
            This may be used to distinguish between events emitted by the user, changing something (in a widget, ...) or
            automtically by this class reloading some display ...
        */
        bool updatingData;

        /*! \brief execute the evaulation

            \param record the record to do the evaluation for
        */
        void doEvaluation(QFRawDataRecord* record);
        
        /*! \brief create an evaluation report for the current record */
        void createReportDoc(QTextDocument* document);

    protected slots:

        /** \brief ensures that the parameter table model is displayed properly */
        void ensureCorrectParamaterModelDisplay();

        void fileChanged(int num, QFRawDataRecord* file);

        void plotMouseMoved(double x, double y);


    
        /** \brief evaluate current file */
        void fitCurrent();
        /** \brief all runs/pixels current file, multi-threaded version */
        void fitAllRunsThreaded();
        /** \brief all runs/pixels current file */
        void fitAllRuns();

        /** \brief all runs/pixels in all filesets */
        void fitAllFilesetsAllPixels();

        /** \brief display the data from the current raw data record */
        void displayData();
        /** \brief display the current evaluation results */
        void displayEvaluation();

        void resetCurrent();
        void resetAllPixels();
        void resetAllPixelsInAllFilesets();
        void copyToInitial();

        void setParameterTableSpans();

        void guessFromCurrentFileSet();


        void errorEstimateModeChanged();
};

#endif // QFFCCSFITEVALUATIONEDITOR_H
