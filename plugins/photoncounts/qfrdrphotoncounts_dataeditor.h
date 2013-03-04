#ifndef QFRDRPHOTONCOUNTSEDITOR_H
#define QFRDRPHOTONCOUNTSEDITOR_H

#include "qfrawdataeditor.h"
#include <QLabel>
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
#include "qfrdrphotoncounts_data.h"
#include "jkqtpoverlayelements.h"

class QFRDRPhotonCountsDataEditorChannelsModel: public QAbstractTableModel {
        Q_OBJECT
    public:
        QFRDRPhotonCountsDataEditorChannelsModel(QObject* parent=NULL);
        void setCurrent(QFRawDataRecord* current);
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    protected:
        QFRawDataRecord* current;
};



/*! \brief editor for QFRawDataRecord
    \ingroup qf3rdrdp_photoncounts
*/
class QFRDRPhotonCountsDataEditor : public QFRawDataEditor {
        Q_OBJECT

    public:
        /** Default constructor */
        QFRDRPhotonCountsDataEditor(QFPluginServices* services, QFRawDataPropertyEditor* propEditor, QWidget* parent);
        /** Default destructor */
        virtual ~QFRDRPhotonCountsDataEditor();
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
        void replotData(int dummy=0);
        void replotDetail();
        void selectionChanged(const QModelIndex & current, const QModelIndex & previous );
        void selectionChanged(const QItemSelection & current, const QItemSelection & previous );
        void runsModeChanged(int c);
        void binningChanged(int bin);
        void detailWidthChanged(double width);
        void detailPosChanged(int pos);


    protected:
        /** \brief create widgets on object creation */
        void createWidgets();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        int getDetailWidth() const;

        /** \brief plot the data of the given raw data record
         *
         * \return the contribution of this to the statistics label (i.e. a string containing
         *         HTML formatted rows for this table.
         */
        QString plotBinnedItem(QFRDRPhotonCountsData* m);
        QString plotDetailItem(QFRDRPhotonCountsData *m);
        QString addItemToPlot(double *rateT, double *rate, unsigned long rateN, long datarange_start, long datarange_end, unsigned int rateRuns, QString binned, QFRDRPhotonCountsData *m, JKQtPlotter *parent, bool showAvg=true);

        /** \brief plotter widget for the count rate curve */
        QFPlotter* plotterBinned;
        /** \brief plotter widget for the count rate curve */
        QFPlotter* plotterDetail;
        /** \brief splitter between plot and other widgets */
        QVisibleHandleSplitter* splitter;
        /** \brief splitter between plot and other widgets */
        QVisibleHandleSplitter* splitterPlots;
        /** \brief a listwidget to display only a subset of runs */
        QListView* lstRunsSelect;
        /** \brief combobox to select the run display mode */
        QComboBox* cmbRunDisplay;
        /** \brief group box for data about the count rate curves */
        QGroupBox* grpInfo;
        /** \brief label for the number of runs in the file */
        QLabel* labRuns;
        /** \brief label for the number of points in the count rate curve */
        QLabel* labRatePoints;
        /** \brief label to display the average and stddev of the current count rate curve  */
        QLabel* labRateData;
        /** \brief checkbox to enable/disable the statisctics below the plot */
        QCheckBox* chkDisplayStatistics;
        /** \brief checkbox to enable/disable the displaying of average and standard deviation of the count rates */
        QCheckBox* chkDisplayAverage;
        /** \brief checkbox to always include 0 on the count rate axis */
        QCheckBox* chkIncludeRate0;
        /** \brief combobox for the plot style */
        QComboBox* cmbPlotStyle;
        /** \brief spinbox to select the number of points in the binned curve */
        QSpinBox* spinBinning;
        /** \brief spinbox to select width of detail plot in milliseconds */
        QDoubleSpinBox* spinDetailWidth;

        QScrollBar* scrollDetail;
        QLabel* labScrollRange;
        JKQTPoverlayVerticalRange* ovlRange;

        JKQTPxyGraph* createPlot(QColor color, JKQtPlotter* parent, QColor& colorOut);


        QFRDRPhotonCountsDataEditorChannelsModel runs;
    private:
};

#endif // QFRDRPHOTONCOUNTSEDITOR_H
