/*
Copyright (c) 2014
	
	last modification: $LastChangedDate: 2015-02-02 16:43:09 +0100 (Mo, 02 Feb 2015) $  (revision $Rev: 3775 $)

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


#ifndef QFEVALCOLOCALIZATIONEDITOR_H
#define QFEVALCOLOCALIZATIONEDITOR_H

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include <QProgressDialog>
#include <QTextDocument>
#include <stdint.h>
#include <qfplotter.h>
#include "qftablemodel.h"
namespace Ui {
    class QFEValColocalizationEditor;
}

/*! \brief editor for QFEValColocalizationItem
    \ingroup qf3evalp_eval_colocalization
*/
class QFEValColocalizationEditor : public QFEvaluationEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFEValColocalizationEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFEValColocalizationEditor();
    protected slots:
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old);
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void resultsChanged();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();
        void userRangeChanged(double xmin, double xmax,  double ymin, double ymax);
        void on_btnEstimateBackground_clicked();
        void writeWidgetValues(QFRawDataRecord* formerRecord);
    protected:
        /** \brief the user interface object (using QtDesigner) */
        Ui::QFEValColocalizationEditor *ui;

        
        /** \brief progress dialog used by the fits */
        QProgressDialog* dlgEvaluationProgress;
        QFTableModel modelresults;
        
        
        /*! \brief a flag that indicates whether we are currently updating the display data 
        
            This may be used to distinguish between events emitted by the user, changing something (in a widget, ...) or
            automtically by this class reloading some display ...
        */
        bool updatingData;

        
        
        /*! \brief create an evaluation report for the current record */
        void createReportDoc(QTextDocument* document);

    protected slots:
        /** \brief activated when the highlighted record changed */
        void highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord);
    
        /** \brief evaluate all files */
        void evaluateAll();
        /** \brief evaluate current file */
        void evaluateCurrent();
        /** \brief display the data from the current raw data record */
        void displayData();
        /** \brief display the current evaluation results */
        void displayEvaluation();
        


};

#endif // QFEVALCOLOCALIZATIONEDITOR_H
