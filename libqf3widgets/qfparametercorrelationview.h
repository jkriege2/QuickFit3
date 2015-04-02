/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef QFPARAMETERCORRELATIONVIEW_H
#define QFPARAMETERCORRELATIONVIEW_H

#include <QWidget>

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
#include <QSpinBox>
#include <QAbstractTableModel>
#include <QTabWidget>
#include <QSet>
#include "qfdoubleedit.h"
#include "qtriple.h"
#include "qfhtmldelegate.h"
#include <QTableView>
#include "qenhancedtableview.h"
#include "qftablemodel.h"
#include <QFormLayout>
#include <QSettings>
#include <QTextDocument>
#include "libwid_imexport.h"
#include "qfproperties.h"
#include "jkqtptools.h"
#include <QRadioButton>

/*! \brief This class displays a correlation graph, i.e. it plots two parameters against each other
    \ingroup qf3rdrdp_imaging_fcs



*/
class QFWIDLIB_EXPORT QFParameterCorrelationView : public QWidget {
        Q_OBJECT
    public:

        enum ColorCodingMode {
            ccmNone=0,
            ccmBy3rdColumn=1,
            ccmXYGR=2,
            ccmXYRB=3
        };

        struct CorrelationItem {
            CorrelationItem() {
                data1=NULL;
                data2=NULL;
                data3=NULL;
                external=true;
                size=0;
                name="";
            }

            QString name;
            double* data1;
            double* data2;
            double* data3;//color channel
            int32_t size;
            bool external;
        };

        explicit QFParameterCorrelationView(QWidget *parent = 0);
        ~QFParameterCorrelationView();
        /** \brief read the settings */
        void readSettings(QSettings& settings, const QString& prefix=QString("paramcorr/"));
        /** \brief write the settings */
        void writeSettings(QSettings& settings, const QString& prefix=QString("paramcorr/"));

        void writeQFProperties(QFProperties* current, const QString& prefix, const QString& egroup, const QString& param);
        void readQFProperties(QFProperties* current, const QString& prefix, const QString& egroup, const QString& param);

        /** \brief connect/disconnect cmbResultGroups, cmbParameters, ... to their slots */
        void connectParameterWidgets(bool connectTo=true);

        /** \brief replot histogram */
        void replotCorrelation();

        /** \brief recalculate histograms
         *
         * \which==-1 means all histograms, \a which>=0 means a specific histogram
         */
        void updateCorrelation(bool replot=false, int which=-1);

        /** \brief add an additional config widget to the config widets layout at the rhs of the window */
        void addSettingsWidget(const QString& label, QWidget* widget);



        void clear();
        /** \brief add a histogram for the given dataset, copies the data to this widget, if \c external=false */
        int addCorrelation(QString name, double* data1, double* data2, int32_t size, bool external);
        int addCorrelation(QString name, double* data1, double* data2, double* data3, int32_t size, bool external);
        /** \brief add a histogram for the given dataset, copies the data to this widget */
        int addCopiedCorrelation(QString name, const double* data1, const double* data2, int32_t size);
        /** \brief add a histogram for the given dataset, copies the data to this widget */
        int addCopiedCorrelation(QString name, const double* data1, const double* data2, const double* data3, int32_t size);
        /** \brief set the given dataset in the given histogram \a i, copies the data to this widget, if \c external=false */
        void setCorrelation(int i, QString name, double* data1, double* data2, int32_t size, bool external);
        void setCorrelation(int i, QString name, double* data1, double* data2, double* data3, int32_t size, bool external);
        void setCopiedCorrelation(int i, QString name, const double* data1, const double* data2, int32_t size);
        void setCopiedCorrelation(int i, QString name, const double* data1, const double* data2, const double* data3, int32_t size);
        void removeCorrelation(int i);
        int CorrelationCount() const;

        void setCorrelation1Label(const QString label, bool update=false);
        void setCorrelation2Label(const QString label, bool update=false);
        void setCorrelationColorLabel(const QString label, bool update=false);

        void writeReport(QTextCursor& cursor, QTextDocument* document);

        void setSpaceSavingMode(bool enabled);

        double getParam1Min() const;
        double getParam1Max() const;
        double getParam2Min() const;
        double getParam2Max() const;
        double getParamColorMin() const;
        double getParamColorMax() const;

        void setCorrelationHistogramStretch(int corrStretch=3, int histStretch=1);

        double getCurrentRangeSelectionXMin() const;
        double getCurrentRangeSelectionXMax() const;
        double getCurrentRangeSelectionYMin() const;
        double getCurrentRangeSelectionYMax() const;

        void setRangeSelectionMode(bool enabled=true);
        void setrangeSelection(double xmin, double xmax, double ymin, double ymax);

    signals:
        void settingsChanged();
        void rangeSelected(double xmin, double xmax, double ymin, double ymax);
    public slots:
        void clearRangeSelection();
        void dataSettingsChanged(bool update=true);
        void showPlotPosition(double x, double y);
        void setColorCodingMode(ColorCodingMode colorcoding);

        void printReport();
        void saveReport();
        void copyData();
        void saveData();
        void copyDataMatlab();
        void showHelp();
        void fillDataArray(QList<QVector<double> >& data, QStringList &headers);
    protected slots:
        void rangeSelectionRectangleFinished(double x, double y, double width, double height, Qt::KeyboardModifiers modifiers);
        void UpdateSelectionRangeSelected();
    protected:
        double currentRangeSelection_xmin;
        double currentRangeSelection_xmax;
        double currentRangeSelection_ymin;
        double currentRangeSelection_ymax;
        JKQTPoverlayRectangle* plteRangeSelection;

        QList<CorrelationItem> histograms;

        ColorCodingMode colorcoding;

        void createWidgets();

        int connectParameterWidgetsCounter;

        QGridLayout* layPlots;

        /** \brief plotter widget for the parameter histogram */
        QFPlotter* pltParamCorrelation;

        QFPlotter* pltParamHistogramX;
        QFPlotter* pltParamHistogramY;

        QSpinBox* spinHistogramBins1;
        QSpinBox* spinHistogramBins2;

        QGroupBox* grpHistogramSettings;
        QCheckBox* chkScatterPlot;
        QComboBox* cmb2DHistogram;
        QSpinBox* spin2DHistogramBins1;
        QSpinBox* spin2DHistogramBins2;
        QCheckBox* chkKey;



        QEnhancedTableView* tvHistogramParameters;
        QFTableModel* tabHistogramParameters;
        QVisibleHandleSplitter* splitterHistogram;

        double mainHistogramMax;

        QRadioButton* chkHistogramRangeAuto1;
        QRadioButton* chkHistogramRangeRelaxAuto1;
        QRadioButton* chkHistogramRangeManual1;
        QFDoubleEdit* edtHistogramMin1;
        QFDoubleEdit* edtHistogramMax1;
        QDoubleSpinBox* spinHistogramQL1;
        QDoubleSpinBox* spinHistogramQU1;

        QRadioButton* chkHistogramRangeAuto2;
        QRadioButton* chkHistogramRangeRelaxAuto2;
        QRadioButton* chkHistogramRangeManual2;
        QFDoubleEdit* edtHistogramMin2;
        QFDoubleEdit* edtHistogramMax2;
        QDoubleSpinBox* spinHistogramQL2;
        QDoubleSpinBox* spinHistogramQU2;

        JKQTPSymbolComboBox* cmbSymbol;
        QSpinBox* spinSymbolSize;

        JKQTPMathImageColorPaletteComboBox* cmbColorScale;
        QRadioButton* chkColorRangeAuto;
        QRadioButton* chkColorRangeRelaxAuto;
        QRadioButton* chkColorRangeManual;
        QFDoubleEdit* edtColorMin;
        QFDoubleEdit* edtColorMax;
        QDoubleSpinBox* spinColQL;
        QDoubleSpinBox* spinColQU;
        QLabel* labColorCodingHeader;
        QLabel* labColorCodingHeader1;
        QLabel* labColorCodingHeader2;
        QLabel* labColorCodingHeader3;
        QWidget* widColorCodingHeader;
        QWidget* widColorCodingHeader2;





        QFormLayout* flHistSet;
        QString histLabelX;
        QString histLabelY;
        QString histLabelColor;
        QVBoxLayout* laySplitterTable;
        QGridLayout* layHist;

        QLabel* labPlotPos;
        QAction* actHelp;
        QAction* actPrintReport;
        QAction* actSaveReport;
        QAction* actCopyData;
        QAction* actCopyDataMatlab;
        QAction* actSaveData;
        protected slots:
        void histTypeChanged(int index);
};

#endif // QFPARAMETERCORRELATIONVIEW_H

