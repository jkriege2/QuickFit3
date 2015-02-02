/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#ifndef QFSPIMLIGHTSHEETEVALUATIONEDITOR_H
#define QFSPIMLIGHTSHEETEVALUATIONEDITOR_H

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include <QProgressDialog>
#include <QTextDocument>
#include "qfrdrimagestackinterface.h"
#include "jkqtplotter.h"
#include "jkqtpimageelements.h"
#include "qffitfunctionmanager.h"
#include "qffitalgorithmmanager.h"
#include "qffitfunctionplottools.h"
#include "qftablemodel.h"
#include "datatools.h"

class QFSPIMLightsheetEvaluationItem; // forward

namespace Ui {
    class QFSPIMLightsheetEvaluationEditor;
}

/*! \brief editor for QFSPIMLightsheetEvaluationItem
    \ingroup qf3evaldp_spim_lightsheet_eval
*/
class QFSPIMLightsheetEvaluationEditor : public QFEvaluationEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFSPIMLightsheetEvaluationEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFSPIMLightsheetEvaluationEditor();
    protected slots:
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old);
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void resultsChanged();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        void updateStack();

        void on_cmbOrientation_currentIndexChanged(int index);
        void on_cmbModel_currentIndexChanged(int index);
        void on_cmbAlgorithm_currentIndexChanged(int index);
        void on_spinDeltaX_valueChanged(double value);
        void on_spinDeltaZ_valueChanged(double value);
        void on_pltImage_plotMouseClicked(double x, double y, Qt::KeyboardModifiers modifiers=Qt::NoModifier, Qt::MouseButton button=Qt::LeftButton);
        void on_chkErrorsParam_toggled(bool checked);
        void on_chkErrorsBeamPos_toggled(bool checked);
        void on_chkUseMask_toggled(bool checked);
        void on_chkEstimateInitial_toggled(bool checked);
        void on_chkScaleIntensityFit_toggled(bool checked);
    protected:

        void prepareImagePlotter(JKQtPlotter *pltImage);

        void showImageCut(QFSPIMLightsheetEvaluationItem *eval, QFRawDataRecord* record, QFRDRImageStackInterface *data, int stack, int stack_pos, double x, double y, int channel=-1);


        /** \brief the user interface object (using QtDesigner) */
        Ui::QFSPIMLightsheetEvaluationEditor *ui;

        JKQTPMathImage* plteImage;
        JKQTPxyLineGraph* plteMark;
        double markDataX[2];
        double markDataY[2];
        int c_markDataX;
        int c_markDataY;

        double lastMousePreviewX;
        double lastMousePreviewY;

        QFTableModel* tableFitResults;

        /** \brief progress dialog used by the fits */
        QProgressDialog* dlgEvaluationProgress;
        
        /** \brief where did we save the last report */
        QString currentSaveDirectory;
        
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

        bool* getCurrentMask();

        QAction* actSaveImageCutSeries;

    protected slots:
        /** \brief activated when the highlighted record changed */
        void highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord);
    
        /** \brief evaluate all files */
        void evaluateAll();
        /** \brief evaluate current file */
        void evaluateCurrent();
        /** \brief display the data from the current raw data record */
        void displayPreview();
        /** \brief display the current evaluation results */
        void displayEvaluationResults();

        void paramChanged(int i);

        void updateFitResultRanges();

        void saveImageCutSeries();




};

#endif // QFSPIMLIGHTSHEETEVALUATIONEDITOR_H
