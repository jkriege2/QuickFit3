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

#ifndef QFEVALUATIONEDITOR_H
#define QFEVALUATIONEDITOR_H

#include "lib_imexport.h"
#include <QWidget>
#include <QPointer>
#include "programoptions.h"
#include "qfpluginservices.h"
#include <QTextDocument>
#include "qfrawdatarecord.h"
#include <QVariant>

// forward declaration
class QFEvaluationItem;
class QFEvaluationPropertyEditor;

/*! \brief base class for all evaluation widgets
    \ingroup qf3lib_project
*/
class QFLIB_EXPORT QFEvaluationEditor : public QWidget {
        Q_OBJECT
    public:
        /** Default constructor */
        QFEvaluationEditor(QFPluginServices* services, QFEvaluationPropertyEditor* propEditor, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFEvaluationEditor();
        /** \brief set the current record */
        virtual void setCurrent(QFEvaluationItem* c, int peID);
        /** \brief set the used ProgramOptions object */
        void setSettings(ProgramOptions* s, int peID) {
            this->peID=peID;
            settings=s;
            if (settings) readSettings();
        }
        void saveSettings() { writeSettings(); };
        /*! \brief execute the given command

            This function does nothing in the default implementation. It can be implemented to allow editor to perform
            certain functions that are called form the outside.
         */
        virtual void sendEditorCommand(const QString& command, const QVariant& param1=QVariant(), const QVariant& param2=QVariant(), const QVariant& param3=QVariant(), const QVariant& param4=QVariant(), const QVariant& param5=QVariant());

    protected slots:
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFEvaluationItem* current, QFEvaluationItem* old) =0;
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void resultsChanged(QFRawDataRecord* record, const QString& evalName=QString(""), const QString& resultName=QString("")) {};

    public slots:
        /** \brief read the settings */
        virtual void readSettings() =0;
        /** \brief write the settings */
        virtual void writeSettings() =0;
        /** \brief save a report of the evaluation results */
        virtual void saveReport();
        /** \brief print a report of the evaluation results */
        virtual void printReport();
    protected:
        virtual void createReportDoc(QTextDocument* doc);
        virtual bool	event ( QEvent * event );
        void closeEvent( QCloseEvent * event );
        /** \brief points to the record currently displayed */
        QPointer<QFEvaluationItem> current;
        /** \brief settinsg object used to store application settings */
        ProgramOptions* settings;
        /** \brief ID of the owning QFEvaluationPropertyEditor */
        int peID;
        /** \brief pointer that allows for access to central QuickFit services */
        QFPluginServices* services;
        /** \brief button to print report */
        QAction* actPrintReport;
        /** \brief button to save report */
        QAction* actSaveReport;

        /** \brief the property editor holfing this widget */
        QFEvaluationPropertyEditor* propertyEditor;


};



#endif // QFEVALUATIONEDITOR_H
