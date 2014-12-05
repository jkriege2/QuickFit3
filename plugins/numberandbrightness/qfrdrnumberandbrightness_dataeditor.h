/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#ifndef QFRDRNUMBERANDBRIGHTNESSEDITOR_H
#define QFRDRNUMBERANDBRIGHTNESSEDITOR_H

#include "qfrawdataeditor.h"
#include <QLabel>
#include "qfplotter.h"
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>
#include "qvisiblehandlesplitter.h"
#include <QSplitter>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QAbstractTableModel>
#include "datacutslider.h"
#include "jkqtptools.h"
#include "qfhistogramview.h"
#include "qfrdrimagemaskedittools.h"
#include "qfparametercorrelationview.h"
#include "qfenhancedcombobox.h"
#include <QCheckBox>
#include "qfdoubleedit.h"
#include "jkqtpgeoelements.h"

class QFRDRNumberAndBrightnessData; //forward

/*! \brief editor for QFRawDataRecord
    \ingroup qf3rdrdp_number_and_brightness
*/
class QFRDRNumberAndBrightnessDataEditor : public QFRawDataEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRNumberAndBrightnessDataEditor(QFPluginServices* services,  QFRawDataPropertyEditor* propEditor, QWidget* parent);
        /** Default destructor */
        virtual ~QFRDRNumberAndBrightnessDataEditor();
    protected slots:
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged();
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old);
        /** \brief displays the data from the current data element in the plotter widget
         *
         * the parameter dummy has no function it is only there so this function may be used
         * as a slot for a signal with an integer parameter.
         */
        void replotData();

        void updateHistograms();

        void imageZoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter *sender);

        void updateCorrSelection();
    protected:
        /** \brief create widgets on object creation */
        void createWidgets();

        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        void loadPlotSettings();
        void savePlotSettings();

        void addPlotter( QFPlotter*& plotter, JKQTPMathImage*& plot, JKQTPOverlayImage*& plteSelected, JKQTPOverlayImage*& plteExcluded);

        void reallocMem(int width, int height);



        QColor selectionColor;
        QColor excludedColor;


        /** \brief plotter widget for the brightness image */
        QFPlotter* pltBrightness;
        /** \brief plotter widget for the particle number image */
        QFPlotter* pltNumber;
        /** \brief plotter widget for the overview image */
        QFPlotter* pltOverview;
        /** \brief plotter widget for the overview image */
        QFPlotter* pltCorrelation;

        JKQTPMathImage* plteNumber;
        JKQTPOverlayImage* plteNumberSelected;
        JKQTPOverlayImage* plteNumberExcluded;

        JKQTPMathImage* plteBrightness;
        JKQTPOverlayImage* plteBrightnessSelected;
        JKQTPOverlayImage* plteBrightnessExcluded;

        JKQTPMathImage* plteOverview;
        JKQTPOverlayImage* plteOverviewSelected;
        JKQTPOverlayImage* plteOverviewExcluded;

        JKQTPxyLineGraph* plteCorrelation;
        JKQTPhorizontalRange* plteRangeB;
        JKQTPverticalRange* plteRangeN;

        QFHistogramView* histNumber;
        QFHistogramView* histBrightness;
        QFHistogramView* histIntensity;
        QFParameterCorrelationView* widCorrelation;
        QTabWidget* tabMain;

        QFEnhancedComboBox* cmbCorrelationP1;
        QFEnhancedComboBox* cmbCorrelationP2;
        QFEnhancedComboBox* cmbCorrelationPCol;

        QFDoubleEdit* edtNMin;
        QFDoubleEdit* edtNMax;
        QFDoubleEdit* edtBMin;
        QFDoubleEdit* edtBMax;
        QCheckBox* chkRangeN;
        QCheckBox* chkRangeB;

        QToolBar* toolbar;

        QPointer<QFRDRImageMaskEditTools> maskTools;


        QMenu* menuMask;



        double* plteNumberData;
        double* plteBrightnessData;
        double* plteOverviewData;
        double* plteCorrelationData;

        /** \brief data in plteOverviewSelected */
        bool* plteOverviewSelectedData;
        /** \brief size of plteOverviewSelectedData */
        int plotsSize;


};

#endif // QFRDRNUMBERANDBRIGHTNESSEDITOR_H
