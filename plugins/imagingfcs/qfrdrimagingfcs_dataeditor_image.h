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
#include <QSpinBox>
#include <QAbstractTableModel>
#include <QTabWidget>
#include "datacutslider.h"
#include "qfrdrimagingfcsrunsmodel.h"
#include <QSet>
#include "jkdoubleedit.h"
#include "qtriple.h"
#include "qfhtmldelegate.h"
#include <QTableView>
#include "qffitfunction.h"
#include "qenhancedtableview.h"
#include "../base_classes/qftablemodel.h"
#include "qfhistogramview.h"
#include "jkqtptools.h"
#include <QFrame>
#include <QFile>
#include <QToolBar>

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
protected:
        enum ImageTransforms {
            itNone=0,
            itAbs=1,
            itLog=2,
            itReciprocal=3,
            itSqrt=4
        };

        /** \brief create widgets on object creation */
        void createWidgets();
        /** \brief read the settings */
        virtual void readSettings();
        /** \brief write the settings */
        virtual void writeSettings();

        /** \brief return the currently selected evalGroup (or an empty string, if none selected) */
        QString currentEvalGroup() const;
        /** \brief return the currently selected fit parameter (or an empty string, if none selected, or no evalGroup selected) */
        QString currentFitParameter() const;
        /** \brief return the currently selected goodnes of fit parameter (or an empty string, if none selected, or no evalGroup selected) */
        QString currentGofParameter() const;
        /** \brief return the currently selected fit parameter transformation */
        ImageTransforms currentFitParameterTransfrom() const;
        /** \brief return the currently selected goodnes of fit parameter transformation */
        ImageTransforms currentGofParameterTransfrom() const;

    protected slots:

        /** \brief switch plots on and off according to selected visible plots */
        void showHidePlots();
        /** \brief move color bars to appropriate position */
        void moveColorbarsAuto();

        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged();
        /** \brief update selection etz. after click on image */
        void updateAfterClick();
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old);


        /** \brief replot histogram */
        void replotHistogram();

        /** \brief recalculate histogram over all pixels */
        void updateHistogram();

        /** \brief recalculate histogram over selected pixels */
        void updateSelectionHistogram(bool replot=false);


        /** \brief displays the data from the current data element in the plotter widget
         *
         * the parameter dummy has no function it is only there so this function may be used
         * as a slot for a signal with an integer parameter.
         */
        void replotData();

        /** \brief draw overview plot */
        void replotImage();

        /** \brief replot the selection displays only */
        void replotSelection(bool replot=true);

        /** \brief fill the selection/expluded arrays for the overlays, does not plot */
        void updateSelectionArrays();

        /** \brief draw overview plot */
        void replotOverview();
        /** \brief draw mask plot */
        void replotMask();
        /** \brief excludes the currently selected items from the evaluation and recalculates the average */
        void excludeRuns();
        /** \brief includes the currently selected items from the evaluation and recalculates the average */
        void includeRuns();
        /** \brief exclude runs by intensity in the overview image */
        void excludeByIntensity();
        /** \brief exclude runs by intensity in the parameter image */
        void excludeByParamIntensity();
        /** \brief exclude runs by intensity in the gof image */
        void excludeByGOFIntensity();

        /** \brief load a mask file as a selection from hard disk */
        void loadMask();
        /** \brief save mask to harddisk */
        void saveMask();
        /** \brief save selection to harddisk */
        void saveSelection();
        /** \brief load a selection from harddisk */
        void loadSelection();

        /** \brief delete mask */
        void includeAll();
        /** \brief invert mask */
        void invertMask();
        void slidersChanged(int userMin, int userMax, int min, int max);
        void imageClicked(double x, double y, Qt::KeyboardModifiers modifiers);
        void imageScribbled(double x, double y, Qt::KeyboardModifiers modifiers, bool first, bool last);
        void imageMouseMoved(double x, double y);
        void imageRectangleFinished(double x, double y, double width, double height, Qt::KeyboardModifiers modifiers);
        void imageLineFinished(double x1, double y1, double x2, double y2, Qt::KeyboardModifiers modifiers);
        void imageCircleFinished(double x, double y, double radius, Qt::KeyboardModifiers modifiers);
        void imageEllipseFinished(double x, double y, double radiusX, double radiusY, Qt::KeyboardModifiers modifiers);
        void setImageEditMode();

        /** \brief activated when the user selects a new parameter set/evaluation group,  fills cmbParameters with all available parameters in the selected result group */
        void parameterSetChanged();
        /** \brief activated when the user selects a new parameter*/
        void parameterChanged();
        /** \brief called when the user selects a new transformation */
        void transformChanged();
        /** \brief clear the parameter image */
        void clearImage();
        /** \brief fills cmbResultGroup with all available result groups */
        void fillParameterSet();
        /** \brief connect/disconnect cmbResultGroups, cmbParameters, ... to their slots */
        void connectParameterWidgets(bool connectTo=true);
        /** \brief connect/disconnect widgets for image settings to their slots */
        void connectImageWidgets(bool connectTo=true);
        /** \brief called when the user selects a new palette */
        void paletteChanged();
        void histogramSettingsChanged();
        /** \brief called when the user selects to display overlays or not */
        void updateOverlaySettings();

        /** \brief save the image plot settings to the raw data record properties */
        void saveImageSettings();
        /** \brief load the image plot settings from the raw data record properties */
        void loadImageSettings();

        void imageZoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter* sender);

        void acfZoomChangedLocally(double newxmin, double newxmax, double newymin, double newymax, JKQtPlotter* sender);

        /*! \brief called when the current's record's fit results change

            This function updates the contents of the image setting comboboxes, but tries to preserve the current selection!
         */
        void resultsChanged();

        void debugInfo();

        /** \brief save a report of the evaluation results */
        void saveReport();
        /** \brief save the displayed images as data files (TIFF, CSV, ...) */
        void saveData();
        /** \brief copy displayed images to Matlab */
        void copyToMatlab();
        /** \brief print a report of the evaluation results */
        void printReport();
    protected:
        /** \brief map with all available fit functions */
        QMap<QString, QFFitFunction*> m_fitFunctions;

        int connectParameterWidgetsCounter;
        int connectImageWidgetsCounter;

        /** \brief indicates whether the overview plot is visible */
        QCheckBox* chkOverviewVisible;
        /** \brief indicates whether the goodnes of fit plot is visible */
        QCheckBox* chkGofVisible;
        /** \brief indicates whether the mask plot is visible */
        QCheckBox* chkMaskVisible;


        /** \brief plotter widget for the correlation curve */
        JKQtPlotter* plotter;
        /** \brief plotter widget for the correlation curve residuals */
        JKQtPlotter* plotterResid;
        /** \brief sliders to set the cut of the correlation curve */
        DataCutSliders* sliders;
        /** \brief splitter between top plots */
        QVisibleHandleSplitter* splitterTop;
        /** \brief splitter between bottom plot and parameter table */
        QVisibleHandleSplitter* splitterBot;
        /** \brief splitter between bottom plot and parameter table */
        //QVisibleHandleSplitter* splitterBotPlots;
        /** \brief splitter between top plots and bottom plots */
        QVisibleHandleSplitter* splitterTopBot;
        /** \brief checkbox to select whether to display errors or not */
        QCheckBox* chkDisplayAverage;
        /** \brief checkbox to select whether to display residulas or not */
        QCheckBox* chkDisplayResiduals;
        /** \brief checkbox to select whether to display selected/excluded pixels overlay on image plot */
        QCheckBox* chkDisplayImageOverlay;
        /** \brief a combobox to select how the average run are displayed */
        QComboBox* cmbAverageStyle;
        /** \brief a combobox to select how the error of the average run are displayed */
        QComboBox* cmbAverageErrorStyle;

        /** \brief a combobox to select how to display the selected pixel */
        QComboBox* cmbSelectionStyle;

        /** \brief a combobox to select how the runs are displayed */
        QComboBox* cmbRunStyle;
        /** \brief a combobox to select how the error of the runs are displayed */
        QComboBox* cmbRunErrorStyle;
        /** \brief a button which excludes the selected runs from the evaluation */
        QPushButton* btnDontUse;
        /** \brief a button which includes the selected runs to the evaluation */
        QPushButton* btnUse;
        /** \brief a button which includes all runs to the evaluation */
        QPushButton* btnUseAll;
        /** \brief a button which inverts the mask */
        QPushButton* btnInvertMask;
        /** \brief button to mask pixels by intensity */
        QPushButton* btnMaskByIntensity;
        /** \brief button to mask pixels by gof image */
        QPushButton* btnMaskByGofIntensity;
        /** \brief button to mask pixels by parameter image */
        QPushButton* btnMaskByParamIntensity;
        /** \brief checkbox to select log tau-axis */
        QCheckBox* chkLogTauAxis;
        /** \brief label for the run options */
        QLabel* labRunOptions;
        /** \brief checkbox to select whether to display key in the graphs */
        QCheckBox* chkKeys;
        QPushButton* btnLoadSelection;
        QPushButton* btnLoadMask;
        QPushButton* btnSaveSelection;
        QPushButton* btnSaveMask;

        /** \brief label over the parameter image plot */
        QLabel* labParamImage;

        /** \brief  plotter for overview image */
        JKQtPlotter* pltOverview;
        /** \brief plot for the overview image in pltOverview */
        JKQTPMathImage* plteOverview;
        /** \brief plot for the selected runs in pltOverview, plot plteOverviewSelectedData */
        JKQTPOverlayImageEnhanced* plteOverviewSelected;
        /** \brief plot for the excluded runs in pltOverview, plot plteOverviewSelectedData */
        JKQTPOverlayImageEnhanced* plteOverviewExcluded;
        /** \brief data in plteOverviewSelected */
        bool* plteOverviewSelectedData;
        /** \brief data in plteOverviewExcluded */
        bool* plteOverviewExcludedData;
        /** \brief size of plteOverviewSelectedData */
        int plteOverviewSize;

        /** \brief  plotter for mask image */
        JKQtPlotter* pltMask;
        /** \brief plot for the mask image in pltMask */
        JKQTPOverlayImage* plteMask;
        /** \brief plot for the selected runs in pltOverview, plot plteOverviewSelectedData */
        JKQTPOverlayImageEnhanced* plteMaskSelected;

        /** \brief  plotter for parameter image */
        JKQtPlotter* pltImage;
        /** \brief plot for the parameter image in pltImage */
        JKQTPMathImage* plteImage;
        double* plteImageData;
        int32_t plteImageSize;

        /** \brief plot for the selected runs in pltImage, plot plteImageSelectedData */
        JKQTPOverlayImageEnhanced* plteImageSelected;
        /** \brief plot for the excluded runs in pltImage, plot plteImageSelectedData */
        JKQTPOverlayImageEnhanced* plteImageExcluded;


        /** \brief  plotter for goodnes of fit image */
        JKQtPlotter* pltGofImage;
        /** \brief plot for the  goodnes of fit image in pltOverview */
        JKQTPMathImage* plteGofImage;
        double* plteGofImageData;


        /** \brief plot for the selected runs in pltGofImage, plot plteImageSelectedData */
        JKQTPOverlayImageEnhanced* plteGofImageSelected;
        /** \brief plot for the excluded runs in pltGofImage, plot plteImageSelectedData */
        JKQTPOverlayImageEnhanced* plteGofImageExcluded;


        /** \brief combobox for the color bar of overview image */
        QComboBox* cmbColorbarOverview;

        /** \brief combobox for the color bar of gof image */
        QComboBox* cmbColorbarGof;

        /** \brief combobox for the color bar of plteImage */
        QComboBox* cmbColorbar;
        /** \brief combobox for the display style of plteImage */
        QComboBox* cmbImageStyle;
        /** \brief cobobox to select what should happen to the out-of-range pixels */
        QComboBox* cmbOutOfRangeMode;

        ///** \brief combobox for the color bar of plteOverview */
        //QComboBox* cmbOverviewColorbar;
        ///** \brief combobox for the color bar of plteGofImage */
        //QComboBox* cmbGofColorbar;

        /** \brief checkbox to en-/disable automatic color bar scaling */
        QCheckBox* chkImageAutoScale;

        JKDoubleEdit* edtColMin;
        JKDoubleEdit* edtColMax;

        /** \brief toolbar to work with parameter images */
        QToolBar* tbParameterImage;
        QLabel* labImagePositionDisplay;
        QLabel* labImageAvg;
        QActionGroup* agImageSelectionActions;

        QAction* actImagesZoom;
        QAction* actImagesDrawPoints;
        QAction* actImagesDrawRectangle;
        QAction* actImagesDrawCircle;
        QAction* actImagesDrawEllipse;
        QAction* actImagesDrawLine;
        QAction* actImagesScribble;
        QComboBox* cmbMaskEditMode;

        /** \brief combobox to select a result group */
        QComboBox* cmbResultGroup;


        /** \brief combobox to select a parameter from the result group */
        QComboBox* cmbParameter;
        QLabel* labParameter;
        QLabel* labParameterTransform;
        QComboBox* cmbParameterTransform;
        /** \brief combobox to select a goodnes of fit parameter from the result group */
        QComboBox* cmbGofParameter;
        QLabel* labGofParameter;
        QLabel* labGofParameterTransform;
        QComboBox* cmbGofParameterTransform;

        /** \brief table for the fit params */
        QEnhancedTableView* tvParams;

        QFTableModel* tabFitvals;

        /** \brief action to print a report */
        QPushButton* btnPrintReport;

        /** \brief action to save a report */
        QPushButton* btnSaveReport;
        /** \brief action to save image data */
        QPushButton* btnSaveData;
        /** \brief action to copy image data to Matlab */
        QPushButton* btnCopyDataToMatlab;


        /** \brief plotter widget for the parameter histogram */

        QFHistogramView* histogram;
        QCheckBox* chkExcludeExcludedRunsFromHistogram;
        double* datahist;
        double* datahistsel;
        int32_t datasize;
        int32_t datasizesel;

        QColor selectionColor;
        QColor excludedColor;

        void setSelectedOverlayStyle(JKQTPOverlayImageEnhanced* plot);




        /** \brief tab widget to select whether to display ACF, histogram, ... */
        QTabWidget* tabDisplay;



        /** \brief set which contains all currently selected runs */
        QSet<int32_t> selected;

        QString lastSavePath;

        QTimer* timUpdateAfterClick;

        QString lastMaskDir;


        void excludeByImage(double *imageIn);


        /** \brief create a parameter image with the given evalGroup and fitParam */
        void readParameterImage(double* image, double* gof_image, uint16_t width, uint16_t height, QString evalGroup, QString fitParam, ImageTransforms tranFitParam, QString gofParam, ImageTransforms tranGofParam);

        /** \brief apply the given transformation to the image */
        void transformImage(double *image, uint16_t width, uint16_t height, QFRDRImagingFCSImageEditor::ImageTransforms tranFitParam);

        /*! \brief evaluate the fit function (with parameters) as defined by the pair evalGroup and evaluation

            \param tau lag times where to evaluate the fit function
            \param[out] fit the evaluated wit function is written here, this is only filled if both \a tau and \a fit are non-NULL
            \param N size of tau and fit
            \param[out] names is filled with the names of the parameters
            \param[out] namelabels is filled with the name labels of the parameters
            \param[out] values is filled with the values of the parameters
            \param[out] errors is filled with the errors of the parameters
            \param[out] fix is filled with the fix-states of the parameters
            \param[out] units is filled with the units of the parameters
            \param[out] unitlabels is filled with the unit labels of the parameters
            \param evaluation specifies the parameters to use
            \param index for which run should we print
            \return If not all fit needed parameters are available, this function retuns \c false and does
                    not output data in fit. If the function is well defined it is evaluated for every given
                    tau. The result is saved in fit.
         */
        bool evaluateFitFunction(const double* tau, double* fit, uint32_t N, QStringList& names, QStringList& namelabels, QList<double>& values, QList<double>& errors, QList<bool>& fix, QStringList& units, QStringList& unitlabels, QString evaluation, int index);

        void createReportDoc(QTextDocument* document);
};

#endif // QFRDRIMAGINGFCSEDITORIMAGE_H

