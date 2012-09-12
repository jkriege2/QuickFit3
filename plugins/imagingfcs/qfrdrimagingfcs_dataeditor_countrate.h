#ifndef QFRDRIMAGINGFCSEDITORCOUNTRATE_H
#define QFRDRIMAGINGFCSEDITORCOUNTRATE_H

#include "qfrawdataeditor.h"
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
#include <QAbstractTableModel>
#include "datacutslider.h"
#include "qt/jkqtfastplotter.h"
#include "qfrdrimagingfcsrunsmodel.h"


/*! \brief editor for ACFs in QFRDRImagingFCSData
    \ingroup qf3rdrdp_imaging_fcs
*/
class QFRDRImagingFCSDataEditorCountrate : public QFRawDataEditor {
        Q_OBJECT
    protected:
        QFRDRImagingFCSRunsModel runs;
    public:
        /** Default constructor */
        QFRDRImagingFCSDataEditorCountrate(QFPluginServices* services, QFRawDataPropertyEditor* propEditor, QWidget* parent);
        /** Default destructor */
        virtual ~QFRDRImagingFCSDataEditorCountrate();
    protected slots:
        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged();
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old);
    protected:
        /** \brief create widgets on object creation */
        void createWidgets();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();


    protected slots:
        /** \brief displays the data from the current data element in the plotter widget
         *
         * the parameter dummy has no function it is only there so this function may be used
         * as a slot for a signal with an integer parameter.
         */
        void replotData(int dummy=0);

        /** \brief draw overview plot */
        void replotOverview();
        /** \brief excludes the currently selected items from the evaluation and recalculates the average */
        void excludeRuns();
        /** \brief includes the currently selected items from the evaluation and recalculates the average */
        void includeRuns();
        void selectionChanged(const QModelIndex & current, const QModelIndex & previous );
        void selectionChanged(const QItemSelection & current, const QItemSelection & previous );
        void runsModeChanged(int c);
        void previewClicked(double x, double y, Qt::KeyboardModifiers modifiers);
    protected:

        /** \brief plotter widget for the correlation curve */
        JKQtPlotter* plotter;
        /** \brief splitter between plot and rhs widgets */
        QVisibleHandleSplitter* splitter;
        /** \brief splitter between plot and bottom widgets */
        QVisibleHandleSplitter* splitterBot;
        /** \brief a listwidget to display only a subset of runs */
        QListView* lstRunsSelect;

        QComboBox* cmbVideo;

        /** \brief a combobox to select how the runs are displayed */
        QComboBox* cmbRunStyle;
        /** \brief a combobox to select how the error of the runs are displayed */
        QComboBox* cmbRunErrorStyle;
        /** \brief a button which excludes the selected runs from the evaluation */
        QPushButton* btnDontUse;
        /** \brief a button which includes the selected runs to the evaluation */
        QPushButton* btnUse;
        /** \brief group box for data about the correlation functions */
        QGroupBox* grpInfo;
        /** \brief label for the number of runs in the file */
        QLabel* labRuns;
        /** \brief label for the number of points in the correlation function */
        QLabel* labCorrelationPoints;
        /** \brief label for the run options */
        QLabel* labRunOptions;

        /** \brief fast plotter for overview image */
        JKQTFastPlotter* pltOverview;
        /** \brief plot for the overview image in pltOverview */
        JKQTFPimagePlot* plteOverview;
        /** \brief plot for the selected runs in pltOverview, plot plteOverviewSelectedData */
        JKQTFPimageOverlayPlot* plteOverviewSelected;
        /** \brief plot for the excluded runs in pltOverview, plot plteOverviewSelectedData */
        JKQTFPimageOverlayPlot* plteOverviewExcluded;
        /** \brief data in plteOverviewSelected */
        bool* plteOverviewSelectedData;
        /** \brief data in plteOverviewExcluded */
        bool* plteOverviewExcludedData;
        /** \brief size of plteOverviewSelectedData */
        int plteOverviewSelectedSize;
};

#endif // QFRDRIMAGINGFCSEDITORCOUNTRATE_H
