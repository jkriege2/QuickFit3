/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef QFRDRPLOTEDITOR_H
#define QFRDRPLOTEDITOR_H

#include <QVBoxLayout>
#include <QWidget>
#include <QTableView>
#include <QAction>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include "qfrawdataeditor.h"
#include "qenhancedtableview.h"
#include "qfrawdatapropertyeditor.h"
#include "qfplotter.h"
#include "qfrdrtableplotwidget.h"
#include "qfenhancedtabwidget.h"


class QFRDRTable; // forward

/*! \brief editor class for plots from data in the associated table
    \ingroup qf3rdrdp_table
*/
class QFRDRPlotEditor : public QFRawDataEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRPlotEditor(QFPluginServices* services, QFRawDataPropertyEditor *propEditor, QWidget *parent=NULL);
        /** Default destructor */
        virtual ~QFRDRPlotEditor();
    public slots:
        void rebuildPlotWidgets(bool keepPosition=false);
    signals:
        void performFit(int xCol, int yCol, int sigmaCol, int plot, int graph, QString function, QFRDRTable::GraphDataSelection sel, bool xlog, bool ylog);
        void performRegression(int xCol, int yCol, int sigmaCol, int plot, int graph, QFRDRTable::GraphDataSelection sel, bool xlog, bool ylog);
        void performRefit(int plot, int graph);
    protected slots:
        void doFit(int xCol, int yCol, int sigmaCol, int plot, int graph, QString function, QFRDRTable::GraphDataSelection sel, bool xlog, bool ylog);
        void doRegression(int xCol, int yCol, int sigmaCol, int plot, int graph, QFRDRTable::GraphDataSelection sel, bool xlog, bool ylog);
        void doRefit(int plot, int graph);
    protected slots:
        /** \brief create widgets on object creation */
        void createWidgets();
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old);
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        void addPlot();
        void deleteCurrentPlot();
        void duplicatePlot();
        void movePlotUp();
        void movePlotDown();
        void deletePlot(int i);

        void clearPlotWidgets();
        void updatePlotName(int plot, QString title);
    public:
        /*! \brief execute an editor command

            These commands are defined:
              - \c showPlot: param1 is the name or index of the given plot. This will display the given (by param1) plot tab.
            .
          */
        virtual void sendEditorCommand(const QString& command, const QVariant& param1=QVariant(), const QVariant& param2=QVariant(), const QVariant& param3=QVariant(), const QVariant& param4=QVariant(), const QVariant& param5=QVariant());
    protected:

        QPointer<QFRDRTable> current;
        QFEnhancedTabWidget* tabPlots;
        QAction* actAddPlot;
        QAction* actDeletePlot;
        QAction* actDuplicatePlot;
        QAction* actMovePlotLeft;
        QAction* actMovePlotRight;
        QString currentPlotDir;
        QToolBar* tbMain;
        QMenu* menuEdit;
        QMenu* menuPlot;
        QList<QPointer<QFRDRTablePlotWidget> > plotWidgets;

        bool updating;
    private:
};

#endif // QFRDRPLOTEDITOR_H
