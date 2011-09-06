#ifndef QFRDRIMAGINGFCSEDITORIMAGE_H
#define QFRDRIMAGINGFCSEDITORIMAGE_H

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
#include <QSet>


/*! \brief editor for FCS fit parameter images, created from QFRDRImagingFCSData
    \ingroup qf3rdrdp_imaging_fcs


    This class uses the evalGroups of QFRawDataRecord to let the user select a set of parameters
    from which to generatethe plot. It assumes that the evaluation writes the run as evalGroup Index,
    so this index is used to get the parameters for each run.

    Only fit results from the result group <code>"fit results"</code> can be used for plotting.
*/
class QFRDRImagingFCSImageEditor : public QFRawDataEditor {
        Q_OBJECT
    public:
        /** Default constructor */
        QFRDRImagingFCSImageEditor(QFPluginServices* services, QWidget* parent);
        /** Default destructor */
        virtual ~QFRDRImagingFCSImageEditor();
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
        void replotImage();

        /** \brief draw overview plot */
        void replotOverview();
        /** \brief excludes the currently selected items from the evaluation and recalculates the average */
        void excludeRuns();
        /** \brief includes the currently selected items from the evaluation and recalculates the average */
        void includeRuns();
        void slidersChanged(int userMin, int userMax, int min, int max);
        void previewClicked(double x, double y, Qt::KeyboardModifiers modifiers);

        /** \brief activated when the suer selects a new parameter set/evaluation group,  fills cmbParameters with all available parameters in the selected result group */
        void parameterSetChanged();
        /** \brief activated when the suer selects a new parameter*/
        void parameterChanged();
        /** \brief clear the parameter image */
        void clearImage();
        /** \brief fills cmbResultGroup with all available result groups */
        void fillParameterSet();
        /** \brief connect/disconnect cmbResultGroups, cmbParameters, ... to their slots */
        void connectParameterWidgets(bool connectTo=true);
    protected:


        /** \brief plotter widget for the correlation curve */
        JKQtPlotter* plotter;
        /** \brief plotter widget for the correlation curve residuals */
        JKQtPlotter* plotterResid;
        /** \brief sliders to set the cut of the correlation curve */
        DataCutSliders* sliders;
        /** \brief splitter between top plots */
        QVisibleHandleSplitter* splitterTop;
        /** \brief splitter between top plots and bottom plots */
        QVisibleHandleSplitter* splitterTopBot;
        /** \brief checkbox to select whether to display errors or not */
        QCheckBox* chkDisplayAverage;
        /** \brief checkbox to select whether to display residulas or not */
        QCheckBox* chkDisplayResiduals;
        /** \brief a combobox to select how the average run are displayed */
        QComboBox* cmbAverageStyle;
        /** \brief a combobox to select how the error of the average run are displayed */
        QComboBox* cmbAverageErrorStyle;

        /** \brief a combobox to select how the runs are displayed */
        QComboBox* cmbRunStyle;
        /** \brief a combobox to select how the error of the runs are displayed */
        QComboBox* cmbRunErrorStyle;
        /** \brief a button which excludes the selected runs from the evaluation */
        QPushButton* btnDontUse;
        /** \brief a button which includes the selected runs to the evaluation */
        QPushButton* btnUse;
        /** \brief checkbox to select log tau-axis */
        QCheckBox* chkLogTauAxis;
        /** \brief label for the run options */
        QLabel* labRunOptions;

        /** \brief label over the parameter image plot */
        QLabel* labParamImage;

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
        int plteOverviewSize;


        /** \brief fast plotter for overview image */
        JKQtPlotter* pltImage;
        /** \brief plot for the overview image in pltOverview */
        JKQTPMathImage* plteImage;
        double* plteImageData;
        int plteImageSize;

        /** \brief plot for the selected runs in pltImage, plot plteImageSelectedData */
        JKQTPImage* plteImageSelected;
        /** \brief plot for the excluded runs in pltImage, plot plteImageSelectedData */
        JKQTPImage* plteImageExcluded;
        QImage imgOverlay;

        /** \brief combobox for the color bar of plteImage */
        QComboBox* cmbColorbar;

        /** \brief combobox to select a result group */
        QComboBox* cmbResultGroup;

        /** \brief combobox to select a parameter from the result group */
        QComboBox* cmbParameter;
        QLabel* labParameter;


        /** \brief set which contains all currently selected runs */
        QSet<int32_t> selected;

};

#endif // QFRDRIMAGINGFCSEDITORIMAGE_H
