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

#ifndef QFUSESRESULTSBYINDEXEVALUATIONEDITOR_H
#define QFUSESRESULTSBYINDEXEVALUATIONEDITOR_H
#include "qfusesresultsbyindexevaluation.h"
#include "qfgetplotdatainterface.h"
#include "qfevaluationeditor.h"

class QFUsesResultsByIndexEvaluationEditor : public QFEvaluationEditor, public QFEvaluationGetPlotdataInterface {
        Q_OBJECT
        Q_INTERFACES(QFEvaluationGetPlotdataInterface)
    public:
        explicit QFUsesResultsByIndexEvaluationEditor(QFPluginServices* services, QFEvaluationPropertyEditor* propEditor, QWidget *parent = 0, const QString& iniPrefix=QString("QFUsesResultsByIndexEvaluationEditor/"));
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();
        /** \brief returns plot data (appends to the list \a data ) from the given record and index therein. \a option specifies, which data to return. */
        virtual void getPlotData(QFRawDataRecord* rec, int index, QList<QFGetPlotdataInterface::GetPlotDataItem>& data, int option, const QString& optionName);
        virtual void getPlotData(QList<QFGetPlotdataInterface::GetPlotDataItem> &data, int option, const QString& optionName);
        virtual void getPlotData(int index, QList<QFGetPlotdataInterface::GetPlotDataItem> &data, int option, const QString& optionName);
        virtual void getPlotData(QFRawDataRecord *rec, QList<QFGetPlotdataInterface::GetPlotDataItem> &data, int option, const QString& optionName);
        /** \brief returns \c true, if the getPlotData Feature is available and (if lists are supplied) a list of optionNames. */
        virtual bool getPlotDataSpecs(QStringList* optionNames=NULL, QList<QFGetPlotdataInterface::GetPlotPlotOptions>* listPlotOptions=NULL);
        virtual int getUserMin(QFRawDataRecord *rec, int index, int defaultMin);
        virtual int getUserMax(QFRawDataRecord *rec, int index, int defaultMax);
        virtual int getUserMin(QFRawDataRecord *rec);
        virtual int getUserMax(QFRawDataRecord *rec);
        virtual int getUserMax(QFRawDataRecord *rec, int index);
        virtual int getUserMin(QFRawDataRecord *rec, int index);

        virtual void connectDefaultWidgets(QFEvaluationItem *current, QFEvaluationItem *old);


    public slots:
        virtual void setUserMin(int userMin);
        virtual void setUserMax(int userMax);
        virtual void setUserMinMax(int userMin, int userMax);
        virtual void copyUserMinToAll(int userMin);
        virtual void copyUserMaxToAll(int userMax);
        virtual void copyUserMinMaxToAll(int userMin, int userMax);
        virtual void copyUserMinToAllRuns(int userMin);
        virtual void copyUserMaxToAllRuns(int userMax);
        virtual void copyUserMinMaxToAllRuns(int userMin, int userMax);
        virtual void createOverlayPlot();
    protected:
        QAction* actOverlayPlot;


    protected:
        virtual int getUserMinAbsMax(QFRawDataRecord *rec, int index);

        QString m_iniPrefix;
        
};

#endif // QFUSESRESULTSBYINDEXEVALUATIONEDITOR_H
