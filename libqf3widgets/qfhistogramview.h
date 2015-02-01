/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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


#ifndef QFHISTOGRAMVIEW_H
#define QFHISTOGRAMVIEW_H

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
#include "qffitfunctioncombobox.h"
#include <QRadioButton>

/*! \brief This class displays a histogram together with some statistical data on it
    \ingroup qf3lib_widgets



*/
class QFWIDLIB_EXPORT QFHistogramView : public QWidget {
        Q_OBJECT
    public:
        struct Histogram {
            Histogram() {
                data=NULL;
                external=true;
                size=0;
                name="";
            }

            QString name;
            double* data;
            int32_t size;
            bool external;
        };


        explicit QFHistogramView(QWidget *parent = 0);
        virtual ~QFHistogramView();
        /** \brief read the settings */
        void readSettings(QSettings& settings, const QString& prefix=QString("histogram/"));
        /** \brief write the settings */
        void writeSettings(QSettings& settings, const QString& prefix=QString("histogram/"));

        /** NOTE: egroup is mainly ignored by this write function!!! */
        void writeQFProperties(QFProperties* current, const QString& prefix, const QString& egroup, const QString& param);
        void readQFProperties(QFProperties* current, const QString& prefix, const QString& egroup, const QString& param);

        /** \brief connect/disconnect cmbResultGroups, cmbParameters, ... to their slots */
        void connectParameterWidgets(bool connectTo=true);

        /** \brief replot histogram */
        void replotHistogram();

        /** \brief recalculate histograms
         *
         * \which==-1 means all histograms, \a which>=0 means a specific histogram
         */
        void updateHistogram(bool replot=false, int which=-1);

        /** \brief add an additional config widget to the config widets layout at the rhs of the window */
        void addSettingsWidget(const QString& label, QWidget* widget);

        bool getLog() const ;
        void setLog(bool enabled);

        int getBins() const;
        void setBins(int bins);

        bool getNormalized() const;
        void setNormalized(bool norm);

        bool getAutorange() const;
        void setAutorange(bool autorange);

        double getMin() const;
        void setMin(double min);

        double getMax() const;
        void setMax(double max);

        inline void setDefaultColor(int i, QColor col) {
            defaultColors[i]=col;
        }

        void clear();
        /** \brief add a histogram for the given dataset, copies the data to this widget, if \c external=false */
        int addHistogram(QString name, double* data, int32_t size, bool external);
        /** \brief add a histogram for the given dataset, copies the data to this widget */
        int addCopiedHistogram(QString name, const double* data, int32_t size);
        /** \brief set the given dataset in the given histogram \a i, copies the data to this widget, if \c external=false */
        void setHistogram(int i, QString name, double* data, int32_t size, bool external);
        void setCopiedHistogram(int i, QString name, const double* data, int32_t size);
        void removeHistogram(int i);
        int histogramCount() const;

        void setHistogramXLabel(const QString label, bool update=false);

        void writeReport(QTextCursor& cursor, QTextDocument* document);

        void setSpaceSavingMode(bool enabled);
    signals:
        void settingsChanged();
    public slots:
        void histogramSettingsChanged(bool update=true);

        void printReport();
        void saveReport();

        void copyData();
        void saveData();
        void copyDataMatlab();
        void showHelp();
        void fillDataArray(QList<QVector<double> >& data, QStringList &headers);
    protected:

        QList<Histogram> histograms;

        void createWidgets();

        int connectParameterWidgetsCounter;

        QMap<int,QColor> defaultColors;

        /** \brief plotter widget for the parameter histogram */
        QFPlotter* pltParamHistogram;
        QCheckBox* chkLogHistogram;
        QSpinBox* spinHistogramBins;
        QGroupBox* grpHistogramSettings;
        QEnhancedTableView* tvHistogramParameters;
        QFTableModel* tabHistogramParameters;
        QVisibleHandleSplitter* splitterHistogram;
        QCheckBox* chkNormalizedHistograms;
        double mainHistogramMax;
        QRadioButton* chkHistogramRangeAuto;
        QRadioButton* chkHistogramRangeRelaxAuto;
        QRadioButton* chkHistogramRangeManual;
        QDoubleSpinBox* edtHistogramRelaxedRangePercent;
        QDoubleSpinBox* edtHistogramRelaxedRangePercentUp;
        QFDoubleEdit* edtHistogramMin;
        QFDoubleEdit* edtHistogramMax;
        QFormLayout* flHistSet;
        QString histLabel;
        QVBoxLayout* laySplitterTable;
        QGridLayout* layHist;
        QCheckBox* chkKey;
        QFFitFunctionComboBox* cmbFitFunction;

        QAction* actPrintReport;
        QAction* actSaveReport;
        QAction* actCopyData;
        QAction* actCopyDataMatlab;
        QAction* actSaveData;
        QAction* actHelp;



};

#endif // QFHISTOGRAMVIEW_H
