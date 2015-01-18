/*
    Copyright (c) 2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate: 2014-09-02 17:43:02 +0200 (Di, 02 Sep 2014) $  (revision $Rev: 3433 $)

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


#ifndef QFEVALCAMERACALIBRATIONEDITOR_H
#define QFEVALCAMERACALIBRATIONEDITOR_H

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include <QProgressDialog>
#include <QTextDocument>
#include "qftablemodel.h"

namespace Ui {
    class QFEvalCameraCalibrationEditor;
}

/*! \brief editor for QFEvalCameraCalibrationItem
    \ingroup qf3rdrdp_GROUPNAME
*/
class QFEvalCameraCalibrationEditor : public QFEvaluationEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFEvalCameraCalibrationEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFEvalCameraCalibrationEditor();
    protected slots:
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old);
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void resultsChanged();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        void saveProperties();
    protected:
        /** \brief the user interface object (using QtDesigner) */
        Ui::QFEvalCameraCalibrationEditor *ui;
        
        /** \brief progress dialog used by the fits */
        QProgressDialog* dlgEvaluationProgress;
        
        /** \brief where did we save the last report */
        QString currentSaveDirectory;
        
        QFTableModel* table;

        /*! \brief a flag that indicates whether we are currently updating the display data 
        
            This may be used to distinguish between events emitted by the user, changing something (in a widget, ...) or
            automtically by this class reloading some display ...
        */
        bool updatingData;

        /*! \brief execute the evaulation

            \param record the record to do the evaluation for
        */
        void doEvaluation();
        
        /*! \brief create an evaluation report for the current record */
        void createReportDoc(QTextDocument* document);

    protected slots:
        /** \brief activated when the highlighted record changed */
        void highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord);
    
        /** \brief evaluate current file */
        void evaluateCurrent();
        /** \brief display the data from the current raw data record */
        void displayData();
        /** \brief display the current evaluation results */
        void displayEvaluation();


};

#endif // QFEVALCAMERACALIBRATIONEDITOR_H
