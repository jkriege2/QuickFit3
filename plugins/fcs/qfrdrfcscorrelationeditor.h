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

#ifndef QFRDRFCSCorrelationEditor_H
#define QFRDRFCSCorrelationEditor_H

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
#include "qfselectrdrdialog.h"
/*! \brief model for runs in QFRDRFCSCorrelationEditor
    \ingroup qf3rdrdp_fcs
*/
class QFRDRFCSCorrelationEditorRunsModel: public QAbstractTableModel {
        Q_OBJECT
    public:
        QFRDRFCSCorrelationEditorRunsModel(QObject* parent=NULL);
        void setCurrent(QFRawDataRecord* current);
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    protected:
        QFRawDataRecord* current;
};


/*! \brief editor for FCS correlation curves
    \ingroup qf3rdrdp_fcs
*/
class QFRDRFCSCorrelationEditor : public QFRawDataEditor {
        Q_OBJECT
    protected:


        QFRDRFCSCorrelationEditorRunsModel runs;
    public:
        /** Default constructor */
        QFRDRFCSCorrelationEditor(QFPluginServices* services, QFRawDataPropertyEditor* propEditor, QWidget* parent);
        /** Default destructor */
        virtual ~QFRDRFCSCorrelationEditor();
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

        //void copyNormalizedACFs();
        void correctOffset();
        void correctBackground();
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

        QAction* actCopyNormalizedACF;
        QAction* actCorrectOffset;
        QCheckBox* chkKeepZoom;

    private:
};

#endif // QFRDRFCSCorrelationEditor_H
