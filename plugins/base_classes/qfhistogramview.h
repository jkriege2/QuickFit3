#ifndef QFHISTOGRAMVIEW_H
#define QFHISTOGRAMVIEW_H

#include <QWidget>

#include <QLabel>
#include "qt/jkqtplotter.h"
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
#include "jkdoubleedit.h"
#include "qtriple.h"
#include "qfhtmldelegate.h"
#include <QTableView>
#include "qenhancedtableview.h"
#include "qftablemodel.h"
#include <QFormLayout>
#include <QSettings>
#include <QTextDocument>

/*! \brief This class displays a histogram together with some statistical data on it
    \ingroup qf3rdrdp_imaging_fcs


*/
class QFHistogramView : public QWidget {
        Q_OBJECT
    public:
        struct Histogram {
            QString name;
            double* data;
            int32_t size;
        };


        explicit QFHistogramView(QWidget *parent = 0);
        virtual ~QFHistogramView();
        /** \brief read the settings */
        void readSettings(QSettings& settings, const QString& prefix=QString("histogram/"));
        /** \brief write the settings */
        void writeSettings(QSettings& settings, const QString& prefix=QString("histogram/"));

        /** \brief connect/disconnect cmbResultGroups, cmbParameters, ... to their slots */
        void connectParameterWidgets(bool connectTo=true);

        /** \brief replot histogram */
        void replotHistogram();

        /** \brief recalculate histogram over all pixels
         *
         * \which==-1 means all histograms, \a which >=0 means a specific histogram
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

        void clear();
        int addHistogram(QString name, double* data, int32_t size);
        void setHistogram(int i, QString name, double* data, int32_t size);
        void removeHistogram(int i);
        int histogramCount() const;

        void writeReport(QTextCursor& cursor, QTextDocument* document);
    signals:
        void settingsChanged();
    public slots:
        void histogramSettingsChanged(bool update=true);


    protected:

        QList<Histogram> histograms;

        void createWidgets();

        int connectParameterWidgetsCounter;

        /** \brief plotter widget for the parameter histogram */
        JKQtPlotter* pltParamHistogram;
        QCheckBox* chkLogHistogram;
        QSpinBox* spinHistogramBins;
        QGroupBox* grpHistogramSettings;
        QEnhancedTableView* tvHistogramParameters;
        QFTableModel* tabHistogramParameters;
        QVisibleHandleSplitter* splitterHistogram;
        QCheckBox* chkNormalizedHistograms;
        double mainHistogramMax;
        QCheckBox* chkHistogramRangeAuto;
        JKDoubleEdit* edtHistogramMin;
        JKDoubleEdit* edtHistogramMax;
        QFormLayout* flHistSet;





};

#endif // QFHISTOGRAMVIEW_H
