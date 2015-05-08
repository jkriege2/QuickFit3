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

#ifndef QFFITRESULTSBYINDEXEVALUATIONEDITORBASE_H
#define QFFITRESULTSBYINDEXEVALUATIONEDITORBASE_H

#include "qffitresultsevaluationeditorbase.h"
#include "qfevaluationeditor.h"
#include "qfrawdatarecord.h"
#include "qffitresultsbyindexevaluation.h"
#include "qffitresultsevaluation.h"

/*! \brief evaluation item editor base class for data fits using QFFitAlgorithm and QFFitFunction where the
           QFRawDataRecord is a dataset with different runs (we can fit to each run). The QFEvaluationItem for
           this editor mus cast to QFFitResultsByIndexEvaluation.
    \ingroup qf3evaluationplugins


*/
class QFFitResultsByIndexEvaluationEditorBase : public QFFitResultsEvaluationEditorBase, public QFEvaluationGetPlotdataInterface {
        Q_OBJECT
        Q_INTERFACES(QFEvaluationGetPlotdataInterface)
    public:
        /** \brief class constructor */
        explicit QFFitResultsByIndexEvaluationEditorBase(QString iniPrefix, QFEvaluationPropertyEditor* propEditor, QFPluginServices* services, QWidget *parent = 0);

        /** \brief returns plot data (appends to the list \a data ) from the given record and index therein. \a option specifies, which data to return. */
        virtual void getPlotData(QFRawDataRecord* rec, int index, QList<QFGetPlotdataInterface::GetPlotDataItem>& data, int option, const QString& optionName);
        virtual void getPlotData(QList<QFGetPlotdataInterface::GetPlotDataItem>& data, int option, const QString& optionName);
        virtual void getPlotData(int index, QList<QFGetPlotdataInterface::GetPlotDataItem>& data, int option, const QString& optionName);
        virtual void getPlotData(QFRawDataRecord* rec, QList<QFGetPlotdataInterface::GetPlotDataItem>& data, int option, const QString& optionName);
        /** \brief returns \c true, if the getPlotData Feature is available and (if lists are supplied) a list of optionNames. */
        virtual bool getPlotDataSpecs(QStringList* optionNames=NULL, QList<QFGetPlotdataInterface::GetPlotPlotOptions>* listPlotOptions=NULL);

        /** \brief connect widgets to current data record */
        virtual void connectDefaultWidgets(QFEvaluationItem* current, QFEvaluationItem* old, bool updatePlots);


        /** \brief get the lower datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int,int) with a viable choice for \a defaultMin */
        virtual int getUserMin(QFRawDataRecord* rec, int index)=0;
        /** \brief get the upper datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int,int) with a viable choice for \a defaultMax */
        virtual int getUserMax(QFRawDataRecord* rec, int index)=0;
        /** \brief get the lower datacut for the current record, the \a defaultMin should be read from editors current data cut widget */
        virtual int getUserMin(QFRawDataRecord* rec, int index, int defaultMin);
        /** \brief get the upper datacut for the current record, the \a defaultMax should be read from editors current data cut widget */
        virtual int getUserMax(QFRawDataRecord* rec, int index, int defaultMax);

        /** \brief get the lower datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int) with a viable choice for \a defaultMin */
        virtual int getUserMin(QFRawDataRecord* rec);
        /** \brief get the upper datacut for the current record, reimplement this by calling getUserMin(QFRawDataRecord*,int) with a viable choice for \a defaultMax */
        virtual int getUserMax(QFRawDataRecord* rec);
    signals:

    public slots:
        /** \brief copy min cut to all files and all runs therein (except the current run in the current file) */
        void copyUserMinToAll(int userMin);

        /** \brief copy max cut to all files and all runs therein (except the current run in the current file) */
        void copyUserMaxToAll(int userMax);

        /** \brief copy min and max cut to all files and all runs therein (except the current run in the current file) */
        void copyUserMinMaxToAll(int userMin, int userMax);

        /** \brief copy min cut to all runs (except the current run) */
        void copyUserMinToAllRuns(int userMin);

        /** \brief copy min and max cut to all runs (except the current run) */
        void copyUserMinMaxToAllRuns(int userMin, int userMax);

        /** \brief copy max cut to all runs (except the current run) */
        void copyUserMaxToAllRuns(int userMax);



        /** \brief reset current */
        virtual void resetCurrent();
        /** \brief reset all files */
        virtual void resetAll();
        /** \brief copy to all files, all runs */
        virtual void copyToAll();
        /** \brief reset all runs in current file */
        virtual void resetAllRuns();
        /** \brief copy to all runs in current file */
        virtual void copyToAllRuns();
        /** \brief copy to all files, but only current run */
        virtual void copyToAllCurrentRun();

        /** \brief store a parameter image (user setable) as fit results */
        virtual void setParamAllIndexes(QFRawDataRecord* rec, const QString& parameter, const QVector<double>& datar, const QVector<double> &dataerror=QVector<double>());
        /** \brief store a parameter image (user setable) as fit results */
        virtual void setParamAllIndexesInCurrent(const QString& parameter, const QVector<double>& data);


        void log_text(QString message);
        void log_warning(QString message);
        void log_error(QString message);

        virtual void createOverlayPlot();
        virtual void createOverlayPlotRuns();
    protected:
        QAction* actOverlayPlot;
        QAction* actOverlayPlotRuns;
        QMenu* menuOverlays;
     public:

        /* explicitly make some functions visible again, as the C++ compiler hides function definitions
           from a base class that are also declared in the derived class, but with different parameter sets!

           see http://www.parashift.com/c++-faq-lite/strange-inheritance.html#faq-23.9
         */

        using QFFitResultsEvaluationEditorBase::getUserMin;
        using QFFitResultsEvaluationEditorBase::getUserMax;

};

#endif // QFFITRESULTSBYINDEXEVALUATIONEDITORBASE_H
