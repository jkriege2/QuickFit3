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

/*! \brief This class displays a correlation graph, i.e. it plots two parameters against each other
    \ingroup qf3rdrdp_imaging_fcs



*/
class QFWIDLIB_EXPORT QFParameterCorrelationView : public QWidget {
        Q_OBJECT
    public:

        struct CorrelationItem {
            CorrelationItem() {
                data1=NULL;
                data2=NULL;
                external=true;
                size=0;
                name="";
            }

            QString name;
            double* data1;
            double* data2;
            int32_t size;
            bool external;
        };

        explicit QFParameterCorrelationView(QWidget *parent = 0);
        ~QFParameterCorrelationView();
        /** \brief read the settings */
        void readSettings(QSettings& settings, const QString& prefix=QString("histogram/"));
        /** \brief write the settings */
        void writeSettings(QSettings& settings, const QString& prefix=QString("histogram/"));

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
        /** \brief add a histogram for the given dataset, copies the data to this widget */
        int addCopiedCorrelation(QString name, const double* data1, const double* data2, int32_t size);
        /** \brief set the given dataset in the given histogram \a i, copies the data to this widget, if \c external=false */
        void setCorrelation(int i, QString name, double* data1, double* data2, int32_t size, bool external);
        void setCopiedCorrelation(int i, QString name, const double* data1, const double* data2, int32_t size);
        void removeCorrelation(int i);
        int CorrelationCount() const;

        void setCorrelation1Label(const QString label, bool update=false);
        void setCorrelation2Label(const QString label, bool update=false);

        void writeReport(QTextCursor& cursor, QTextDocument* document);

        void setSpaceSavingMode(bool enabled);

    signals:
        void settingsChanged();
    public slots:
        void dataSettingsChanged(bool update=true);
        void showPlotPosition(double x, double y);

protected:

    QList<CorrelationItem> histograms;

    void createWidgets();

    int connectParameterWidgetsCounter;

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


    QEnhancedTableView* tvHistogramParameters;
    QFTableModel* tabHistogramParameters;
    QVisibleHandleSplitter* splitterHistogram;

    double mainHistogramMax;

    QCheckBox* chkHistogramRangeAuto1;
    QFDoubleEdit* edtHistogramMin1;
    QFDoubleEdit* edtHistogramMax1;

    QCheckBox* chkHistogramRangeAuto2;
    QFDoubleEdit* edtHistogramMin2;
    QFDoubleEdit* edtHistogramMax2;

    JKQTPSymbolComboBox* cmbSymbol;
    QSpinBox* spinSymbolSize;



    QFormLayout* flHistSet;
    QString histLabelX;
    QString histLabelY;
    QVBoxLayout* laySplitterTable;
    QGridLayout* layHist;

    QLabel* labPlotPos;

    
};

#endif // QFPARAMETERCORRELATIONVIEW_H
