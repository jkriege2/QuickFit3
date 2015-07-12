/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-26 12:40:44 +0200 (Fr, 26 Sep 2014) $  (revision $Rev: 3508 $)

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

#ifndef QFRDRFCSCrossCorrelationEditor_H
#define QFRDRFCSCrossCorrelationEditor_H

#include "qfrawdataeditor.h"
#include "qfplotter.h"
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>
#include <QSplitter>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QAbstractTableModel>
#include "datacutslider.h"
#include "qvisiblehandlesplitter.h"
#include "qfcorrelationmasktools.h"

class QFRDRFCSData; // forward
class QFRDRFCSCrossCorrelationEditor; // forward

/*! \brief model for runs in QFRDRFCSCrossCorrelationEditor
    \ingroup qf3rdrdp_fcs
*/
class QFRDRFCSCrossCorrelationEditorRunsModel: public QAbstractTableModel {
        Q_OBJECT
    public:
        QFRDRFCSCrossCorrelationEditorRunsModel(QFRDRFCSCrossCorrelationEditor* editor, QObject* parent=NULL);
        void setCurrent(QFRawDataRecord* current);
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    protected:
        QFRawDataRecord* current;
        QFRDRFCSCrossCorrelationEditor* editor;
};


/*! \brief editor for FCS correlation curves
    \ingroup qf3rdrdp_fcs
*/
class QFRDRFCSCrossCorrelationEditor : public QFRawDataEditor {
        Q_OBJECT
    protected:


        QFRDRFCSCrossCorrelationEditorRunsModel* runs;
    public:
        /** Default constructor */
        explicit QFRDRFCSCrossCorrelationEditor(QFPluginServices* services, QFRawDataPropertyEditor* propEditor, QWidget* parent);
        /** Default destructor */
        virtual ~QFRDRFCSCrossCorrelationEditor();

        /** \brief return a list (also of the roles) of all records in the current record's group */
        QList<QFRDRFCSData *> getRecordsInGroup(QFRawDataRecord *current, QStringList* roles=NULL, QList<QColor>* graph_colors=NULL, bool* isFCCS_out=NULL, QFRDRFCSData ** acf0=NULL, QFRDRFCSData **acf1=NULL, QFRDRFCSData **fccs=NULL) const;
        QList<QFRDRFCSData *> getRecordsInGroup(QStringList* roles=NULL, QList<QColor>* graph_colors=NULL, bool* isFCCS_out=NULL) const;

        /** \brief returns 0, if the run is excluded in all RDRs in the group, 1 if the files is included in all RDRs in the group and 2, if the index is included in some RDRs in the group */
        int isIncluded(QFRawDataRecord* current, int index) const;

        struct RelFCCSResult {
            inline RelFCCSResult() {
                CCF_div_ACF0=0;
                CCF_div_ACF1=0;
                ACF1CorrectedAmplitude=0;
                ACF0Amplitude=0;
                ACF1Amplitude=0;
                CCFCorrectedAmplitude=0;
                CCFCrosstalkExplainedAmplitude=0;
                CCFAmplitude=0;
                ok=false;
            }

            bool ok;
            double CCF_div_ACF0;
            double CCF_div_ACF1;
            double ACF1CorrectedAmplitude;
            double ACF0Amplitude;
            double ACF1Amplitude;
            double CCFAmplitude;
            double CCFCorrectedAmplitude;
            double CCFCrosstalkExplainedAmplitude;
        };

        RelFCCSResult getRelFCCS(QFRawDataRecord* current, int index=-1) const;


    protected slots:
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged();
        void rawDataChangedRecalc();

        /** \brief displays the data from the current data element in the plotter widget
         *
         * the parameter dummy has no function it is only there so this function may be used
         * as a slot for a signal with an integer parameter.
         */
        void replotData(int dummy=0);
        /** \brief excludes the currently selected items from the evaluation and recalculates the average */
        void excludeRuns();
        /** \brief includes the currently selected items from the evaluation and recalculates the average */
        void includeRuns();

        void selectionChanged(const QModelIndex & current, const QModelIndex & previous );
        void selectionChanged(const QItemSelection & current, const QItemSelection & previous );
        void runsModeChanged(int c);
        void slidersChanged(int userMin, int userMax, int min, int max);

        void copyNormalizedACFs();
        void correctOffset();
    protected:
        /** \brief create widgets on object creation */
        void createWidgets();
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old);
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        /** \brief plotter widget for the correlation curve */
        QFPlotter* plotter;
        /** \brief sliders to set the cut of the correlation curve */
        DataCutSliders* sliders;
        /** \brief splitter between plot and rhs widgets */
        QVisibleHandleSplitter* splitter;
        /** \brief splitter between plot and bottom widgets */
        QVisibleHandleSplitter* splitterBot;
        /** \brief a listwidget to display only a subset of runs */
        QListView* lstRunsSelect;
        /** \brief a combobox to select how the error of the average run are displayed */
        QComboBox* cmbAverageErrors;
        /** \brief a combobox to select how the error of the average run are displayed */
        QComboBox* cmbRunErrors;
        /** \brief a button which excludes the selected runs from the evaluation */
        QPushButton* btnDontUse;
        /** \brief a button which includes the selected runs to the evaluation */
        QPushButton* btnUse;
        /** \brief checkbox to select log tau-axis */
        QCheckBox* chkLogTauAxis;
        /** \brief combobox to select the run display mode */
        QComboBox* cmbRunDisplay;
        /** \brief group box for data about the correlation functions */
        QGroupBox* grpInfo;
        /** \brief label for the number of runs in the file */
        QLabel* labRuns;
        /** \brief label for the number of points in the correlation function */
        QLabel* labCorrelationPoints;
        QMenu* menuMask;
        QMenu* menuData;
        QFCorrelationMaskTools* correlationMaskTools;

        QGroupBox* grpRelCCF;
        QComboBox* cmbFCCSCorrected;
        QDoubleSpinBox* spinCrosstalk;
        QDoubleSpinBox* spinCCFAmplitudeRangeMin;
        QDoubleSpinBox* spinCCFAmplitudeRangeMax;
        QLabel* labRelCCF;
        QLabel* labRelCCFSel;
        QLabel* labRelCCFSelLab;


        QAction* actCopyNormalizedACF;
        QAction* actCorrectOffset;

    private:
};

#endif // QFRDRFCSCrossCorrelationEditor_H
