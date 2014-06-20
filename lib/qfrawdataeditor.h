/*
    Copyright (c) 2008-2014 Jan W. Krieger, German Cancer Research Center

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

#ifndef QFRAWDATAEDITOR_H
#define QFRAWDATAEDITOR_H

#include <QWidget>
#include "programoptions.h"
#include <iostream>
#include "qfpluginservices.h"
#include "qftools.h"
#include "lib_imexport.h"
#include <QVariant>

// forward declaration
class QFRawDataRecord;
class QFRawDataPropertyEditor;

/*! \brief base class for all raw data editors used to display/edit the raw data within QFRawDataPropertyEditor window
    \ingroup qf3lib_project

 */
class QFLIB_EXPORT QFRawDataEditor : public QWidget {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRawDataEditor(QFPluginServices* services, QFRawDataPropertyEditor* propEditor, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFRawDataEditor();
        /** \brief set the current record */
        virtual void setCurrent(QFRawDataRecord* c, int peID);
        /** \brief set the used ProgramOptions object */
        void setSettings(ProgramOptions* s, int peID);
        /** \brief save the settings to the ProgramOptions object set by setSettings() */
        void saveSettings() { writeSettings(); };
        /*! \brief execute the given command

            This function does nothing in the default implementation. It can be implemented to allow editor to perform
            certain functions that are called form the outside.
         */
        virtual void sendEditorCommand(const QString& command, const QVariant& param1=QVariant(), const QVariant& param2=QVariant(), const QVariant& param3=QVariant(), const QVariant& param4=QVariant(), const QVariant& param5=QVariant());
    protected slots:
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) =0;
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged() {};
        /** \brief save a report of the evaluation results */
        virtual void saveReport();
        /** \brief print a report of the evaluation results */
        virtual void printReport();
    public slots:
        /** \brief read the settings */
        virtual void readSettings() =0;
        /** \brief write the settings */
        virtual void writeSettings() =0;
    protected:
        virtual void createReportDoc(QTextDocument* doc);

        virtual void closeEvent( QCloseEvent * event );
        /** \brief points to the record currently displayed */
        QFRawDataRecord* current;
        /** \brief settinsg object used to store application settings */
        ProgramOptions* settings;
        /** \brief the ID of the parent QFRawDataPropertyEditor */
        int peID;
        /** \brief pointer that allows for access to central QuickFit services */
        QFPluginServices* services;
        /** \brief the propertyEditor class holding this widget */
        QFRawDataPropertyEditor* propertyEditor;

        /** \brief action to print report */
        QAction* actPrintReport;
        /** \brief action to save report */
        QAction* actSaveReport;

    private:
};



#endif // QFRAWDATAEDITOR_H
