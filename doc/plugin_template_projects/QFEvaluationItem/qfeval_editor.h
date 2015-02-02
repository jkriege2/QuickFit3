/*
Copyright (c) 2014
	
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


#ifndef HEADER_HEDITOR_H
#define HEADER_HEDITOR_H

#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include <QProgressDialog>
#include <QTextDocument>

namespace Ui {
    class QFEVALEditor;
}

/*! \brief editor for QFEVALItem
    \ingroup qf3evalp_GROUPNAME
*/
class QFEVALEditor : public QFEvaluationEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFEVALEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFEVALEditor();
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
        /** \brief the user interface object (using QtDesigner) */
        Ui::QFEVALEditor *ui;
        
        /** \brief progress dialog used by the fits */
        QProgressDialog* dlgEvaluationProgress;
        
        /** \brief where did we save the last report */
        QString currentSaveDirectory;
        
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

#endif // HEADER_HEDITOR_H
