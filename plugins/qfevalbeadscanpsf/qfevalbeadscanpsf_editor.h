/*
Copyright (c) 2014
	
	last modification: $LastChangedDate: 2015-01-18 16:06:14 +0100 (So, 18 Jan 2015) $  (revision $Rev: 3726 $)

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


#ifndef QFEVALBEADSCANPSFEDITOR_H
#define QFEVALBEADSCANPSFEDITOR_H

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
#include "qfrdrimagestackinterface.h"
#include "qflistprogressdialog.h"
#include "qfexporterimageseries.h"
#include "qfexportermanager.h"

namespace Ui {
    class QFEvalBeadScanPSFEditor;
}

/*! \brief editor for QFEvalBeadScanPSFItem
    \ingroup qf3rdrdp_eval_beadscanpsf
*/
class QFEvalBeadScanPSFEditor : public QFEvaluationEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFEvalBeadScanPSFEditor(QFPluginServices* services,  QFEvaluationPropertyEditor *propEditor, QWidget* parent=NULL);
        /** Default destructor */
        virtual ~QFEvalBeadScanPSFEditor();
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

        static QStringList getImageWriterFilterList(QFPluginServices *pluginservices) ;
        static QStringList getImageWriterFormatNameList(QFPluginServices *pluginservices) ;
        static QFExporterImageSeries *getImageWriter(int idx, QFPluginServices *pluginservices) ;
        static int getImageWriterCount(QFPluginServices *pluginservices);
        static QStringList getImageWriterIDList(QFPluginServices *pluginservices);


        /** \brief the user interface object (using QtDesigner) */
        Ui::QFEvalBeadScanPSFEditor *ui;
        
        /** \brief progress dialog used by the fits */
        QFListProgressDialog* dlgEvaluationProgress;
        
        /** \brief where did we save the last report */
        QString currentSaveDirectory;
        
        /*! \brief a flag that indicates whether we are currently updating the display data 
        
            This may be used to distinguish between events emitted by the user, changing something (in a widget, ...) or
            automtically by this class reloading some display ...
        */
        bool updatingData;

        
        
        /*! \brief create an evaluation report for the current record */
        void createReportDoc(QTextDocument* document);

        QVector<double> getBeadsData(const QString &paramName, int paramIdx, const QString& subParamName,  int channel);
        QVector<int> getFilteredBeadsID(int channel);
    protected slots:
        /** \brief activated when the highlighted record changed */
        void highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord);
    
        /** \brief evaluate current file */
        void evaluateCurrent();
        void resetCurrent();
        /** \brief update the bead display */
        void displayEvaluationBead();
        /** \brief upate the histogram display */
        void displayEvaluationHistograms();
        /** \brief upate the correlation plot display */
        void displayEvaluationCorrPlot();
        /** \brief fill evaluation lists/widgets */
        void displayResults();
        


        void on_chkMedianFIlter_toggled(bool value);
        void on_spinZ_valueChanged(double value);
        void on_spinA_valueChanged(double value);
        void on_spinPSFHeight_valueChanged(double value);
        void on_spinPSFWidth_valueChanged(double value);
        void on_spinROIXY_valueChanged(int value);
        void on_spinROIZ_valueChanged(int value);
        void on_spinPixPerFrame_valueChanged(int value);
        void on_spinWZFraction_valueChanged(double value);

        void on_btnSaveAllROI_clicked();
        void on_btnSaveROI_clicked();
        void on_btnSaveAvgROI_clicked();
        void evaluateAll();
        void saveROI(const QString& filename, const QString &exportID, QFRawDataRecord* rdr, int bead=0, int channel=0);
};

#endif // QFEVALBEADSCANPSFEDITOR_H



/*
if (record->resultsGetAsString(evalID, "dataorder").toLower()=="beadslists") {
} else {
}
 */
