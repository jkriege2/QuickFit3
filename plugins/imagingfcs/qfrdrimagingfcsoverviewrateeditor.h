/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#ifndef QFRDRIMAGINGFCSOVERVIEWRATEEDITOR_H
#define QFRDRIMAGINGFCSOVERVIEWRATEEDITOR_H

#include <QWidget>
#include "qfrawdatarecord.h"
#include "qfrawdataeditor.h"
#include "qfrdrimagingfcs_data.h"
#include "qfrdrimagingfcscountratedisplay.h"
#include "qfrdroverviewimagedisplay.h"
#include "qvisiblehandlesplitter.h"

class QFRDRImagingFCSOverviewRateEditor : public QFRawDataEditor
{
        Q_OBJECT
    public:
        explicit QFRDRImagingFCSOverviewRateEditor(QFPluginServices* services, QFRawDataPropertyEditor* propEditor, QWidget *parent = 0);
        virtual ~QFRDRImagingFCSOverviewRateEditor();

    protected slots:
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) ;
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged() ;

    public slots:
        /** \brief read the settings */
        virtual void readSettings() ;
        /** \brief write the settings */
        virtual void writeSettings() ;
    protected:
        void createWidgets();

        QFRDRImagingFCSCountrateDisplay* crEditor;
        QFRDROverviewImageDisplay* imEditor1;

        QVisibleHandleSplitter* mainSplitter;

};

#endif // QFRDRIMAGINGFCSOVERVIEWRATEEDITOR_H
