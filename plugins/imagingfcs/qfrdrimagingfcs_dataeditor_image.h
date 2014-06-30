#ifndef QFRDRIMAGINGFCSEDITORIMAGE_H
#define QFRDRIMAGINGFCSEDITORIMAGE_H

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
#include <QSpinBox>
#include <QAbstractTableModel>
#include <QTabWidget>
#include "datacutslider.h"
#include "qfrdrimagingfcsrunsmodel.h"
#include <QSet>
#include "qfdoubleedit.h"
#include "qtriple.h"
#include "qfhtmldelegate.h"
#include <QTableView>
#include "qffitfunction.h"
#include "qenhancedtableview.h"
#include "qftablemodel.h"
#include "qfhistogramview.h"
#include "jkqtptools.h"
#include <QFrame>
#include <QFile>
#include <QToolBar>
#include "jkverticalscrollarea.h"
#include "qfcorrelationmasktools.h"
#include "qfrdrtableinterface.h"
#include "qfrdrcolumngraphsinterface.h"
#include "qfplottercopytotabledialog.h"
#include "qfimageplotter.h"
#include "qfimageparametergroupbox.h"
#include "qfoverlaystylecombobox.h"
#include "qfparametercorrelationview.h"
#include "frawdataimageeditor.h"
#include "qfselectrdrdialog.h"
#include "datatools.h"

class QFRDRImagingFCSData; // forward


/*! \brief editor for FCS fit parameter images, created from QFRDRImagingFCSData
    \ingroup qf3rdrdp_imaging_fcs


    This class uses the evalGroups of QFRawDataRecord to let the user select a set of parameters
    from which to generatethe plot. It assumes that the evaluation writes the run as evalGroup Index,
    so this index is used to get the parameters for each run.

    Only fit results from the result group <code>"fit results"</code> can be used for plotting.
*/
class QFRDRImagingFCSImageEditor : public QFRawDataEditor {//FRawDataImageEditor {
        Q_OBJECT
    public:

        /** Default constructor */
        QFRDRImagingFCSImageEditor(QFPluginServices* services, QFRawDataPropertyEditor* propEditor, QWidget* parent);
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
        QString currentFit2Parameter() const;
        /** \brief return the currently selected fit parameter transformation */
        ImageTransforms currentFitParameterTransfrom() const;
        /** \brief return the currently selected goodnes of fit parameter transformation */
        ImageTransforms currentFit2ParameterTransfrom() const;

        QString formatTransformAndParameter(QComboBox* cmbParameter, QComboBox* cmbTransform);

    protected slots:

        /** \brief switch plots on and off according to selected visible plots */
        void showHidePlots();
        /** \brief move color bars to appropriate position */
        void moveColorbarsAuto();

        /** \brief connected to the rawDataChanged() signal of the current record */
        virtual void rawDataChanged();

        virtual void maskChanged();
        /** \brief update selection etz. after click on image */
        void updateAfterClick();
        /** \brief connect widgets to current data record */
        virtual void connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old);


        /** \brief replot histogram */
        void replotHistogram();

        /** \brief recalculate histogram over all pixels */
        void updateHistogram();

        /** \brief fill the given histogram widget with data */
        void updateHistogram(QFHistogramView *histogram, QFRDRImagingFCSData *m, double *plteImageData, int32_t plteImageSize, bool excludeExcluded, bool dv, bool selHistogram);
        /** \brief fill the given correlation widget with data */
        void updateCorrelation(QFParameterCorrelationView *corrView, QFRDRImagingFCSData *m, double *data1, double *data2, int32_t plteImageSize, bool excludeExcluded, bool dv, bool selHistogram, int mode, int channel=0, const QString& label1=QString(), const QString label2=QString(), int width=0, int height=0);


        /** \brief recalculate histogram over selected pixels */
        void updateSelectionHistogram(bool replot=false);


        /** \brief displays the data from the current data element in the plotter widget
         *
         * the parameter dummy has no function it is only there so this function may be used
         * as a slot for a signal with an integer parameter.
         */
        void replotData();

        void plotAverage(QFRDRImagingFCSData *m, QFPlotter* plotter, int c_tau_in, int* c_tau_out=NULL, QVector<double>* tau_out=NULL, QVector<double>* corr_out=NULL, QVector<double>* errcorr_out=NULL, QColor overrideColor=QColor());
        void plotRun(QFRDRImagingFCSData *m, int i, bool plotFit, QFPlotter* plotter, QFPlotter* plotterResid, QFTableModel *tabFitvals=NULL, int c_tau=-1, QVector<double>* tau_out=NULL, QVector<double>* corr_out=NULL, QVector<double>* errcorr_out=NULL, QColor overrideColor=QColor(), const QString& overrideTitle=QString());
        void plotRunsAvg(QFRDRImagingFCSData *m, QSet<int32_t> selected, bool plotFit, QFPlotter* plotter, QFPlotter* plotterResid, QFTableModel *tabFitvals=NULL, int c_tau=-1, QVector<double>* tau_out=NULL, QVector<double>* corr_out=NULL, QVector<double>* errcorr_out=NULL, QColor overrideColor=QColor(), const QString& overrideTitle=QString());


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
        /** \brief exclude runs by intensity in the overview image 2 */
        void excludeByIntensity2();
        /** \brief exclude runs by intensity in the parameter image */
        void excludeByParamIntensity();
        /** \brief exclude runs by intensity in the parameter image 2 */
        void excludeByParan2Intensity();
        /** \brief select pixel by intensity in the overview image */
        void selectByIntensity();
        /** \brief select pixel by intensity in the overview image */
        void selectByIntensity2();
        /** \brief select pixel by intensity in the parameter image */
        void selectByParamIntensity();
        /** \brief select pixel by intensity in the parameter image 2 */
        void selectByParam2Intensity();
        /** \brief run selection builder */
        void buildSelection(bool select_notmask=true);
        /** \brief run mask builder */
        void buildMask();
        /** \brief deselect masked pixels */
        void deselectMask();

        /** \brief save selection to harddisk */
        void saveSelection();
        /** \brief load a selection from harddisk */
        void loadSelection();
        /** \brief copy selection to clipboard */
        void copySelection();
        /** \brief paste a selection from clipboard */
        void pasteSelection();

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
        void parameterSetChanged(bool replot=true);
        /** \brief activated when the user selects a new parameter*/
        void parameterChanged(bool replot=true);
        /** \brief called when the user selects a new transformation */
        void transformChanged(bool replot=true);
        /** \brief clear the parameter image */
        void clearImage();
        /** \brief fills cmbResultGroup with all available result groups */
        void fillParameterSet(bool replot=true);
        /** \brief connect/disconnect cmbResultGroups, cmbParameters, ... to their slots */
        void connectParameterWidgets(bool connectTo=true);
        /** \brief connect/disconnect widgets for image settings to their slots */
        void connectImageWidgets(bool connectTo=true);
        /** \brief called when the user selects a new palette */
        void ovrPaletteChanged();
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
        void resultsChanged(const QString &evalName=QString(""), const QString &resultName=QString(""), bool deleted=false);

        void debugInfo();

        /** \brief save the displayed images as data files (TIFF, CSV, ...) */
        void saveData();
        /** \brief copy displayed images to Matlab */
        void copyToMatlab();
        /** \brief copy the shown images as columns of values */
        void copyDataAsColumns();

        /** \brief copy image data into datastores of the image plots, so they can be copied to other applications. */
        void setCopyableData();

        void updateSelectionCombobox();

        void selectedSelectionInCombo(int index);
        void storeSelection();
        void deleteSelection();
        void selectionEdited();

        void dualviewChanged(int mode);

        void insertSelectedCorrelationsAsFCSRDR();
        void copyFitResultStatistics();
        void copyMaskToAll();

        void copyGroupACFsToTable();

        void recorrelate();

        void setBackgroundFromSelection();
        void setBackgroundFromMasked();
        void setBackground(bool* mask, bool alsoSetOtherACF);

        void aspectRatioChanged();
        void copyMeanCFFromAll();
        void copySelCFFromAll();
        void copySelAvgCFFromAll();

        void postBin();
    protected:
        void copyCFFromFilesToTable(QList<QFRawDataRecord*>& recs, bool copyAvg=true, bool avgSelected=true, bool copySingle=false, bool nameFromRole=true, QList<int> sel=QList<int>());

        /** \brief map with all available fit functions */
        QMap<QString, QFFitFunction*> m_fitFunctions;

        int connectParameterWidgetsCounter;
        int connectImageWidgetsCounter;
        bool reactOnRedrawCalls;

        /** \brief indicates whether the overview plot is visible */
        QCheckBox* chkOverviewVisible;
        /** \brief indicates whether the goodnes of fit plot is visible */
        QCheckBox* chkParamImage2Visible;
        /** \brief indicates whether the mask plot is visible */
        QCheckBox* chkMaskVisible;

        QCheckBox* chkKeepAspect;



        /** \brief plotter widget for the correlation curve */
        QFPlotter* plotter;
        /** \brief plotter widget for the correlation curve residuals */
        QFPlotter* plotterResid;
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
        JKQTPLinePlotStyleComboBox* cmbAverageStyle;
        /** \brief a combobox to select how the error of the average run are displayed */
        JKQTPerrorPlotstyleComboBox* cmbAverageErrorStyle;

        /** \brief a combobox to select how to display the selected pixel */
        QFOverlayStyleCombobox* cmbSelectionStyle;

        /** \brief a combobox to select how the runs are displayed */
        JKQTPLinePlotStyleComboBox* cmbRunStyle;
        /** \brief a combobox to select how the error of the runs are displayed */
        JKQTPerrorPlotstyleComboBox* cmbRunErrorStyle;
        /** \brief checkbox to select log tau-axis */
        QCheckBox* chkLogTauAxis;
        /** \brief label for the run options */
        QLabel* labRunOptions;
        /** \brief checkbox to select whether to display key in the graphs */
        QCheckBox* chkKeys;
        /** \brief combobox to select selection display options for the correlation /table plot */
        QComboBox* cmbSeletionCorrDisplayMode;
        QLabel* labBackground1;
        QLabel* labBackground2;
        QLabel* labCnt1;
        QLabel* labCnt2;
        QCheckBox* chkFCCSUseBackground;
        QComboBox* cmbCrosstalkDirection;
        QDoubleSpinBox* spinCrosstalk;
        QSpinBox* spinCrosstalkAvg;
        QComboBox* cmbCrosstalkMode;
        QLabel* labRelCCF;


        QFImageParameterGroupBox* grpImage;
        QFImageParameterGroupBox* grpImage2;


        /** \brief label over the parameter image plot */
        QLabel* labParamImage;

        /** \brief  plotter for overview image */
        QFPlotter* pltOverview;
        /** \brief plot for the overview image in pltOverview */
        JKQTPMathImage* plteOverview;
        JKQTPRGBMathImage* plteOverviewRGB;
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
        QFPlotter* pltMask;
        /** \brief plot for the mask image in pltMask */
        JKQTPOverlayImage* plteMask;
        /** \brief plot for the selected runs in pltOverview, plot plteOverviewSelectedData */
        JKQTPOverlayImageEnhanced* plteMaskSelected;

        /** \brief  plotter for parameter image */
        QFImagePlotter* pltImage;

        int32_t plteImageSize;



        /** \brief  plotter for goodnes of fit image */
        QFImagePlotter* pltParamImage2;



        /** \brief combobox for the color bar of overview image */
        JKQTPMathImageColorPaletteComboBox* cmbColorbarOverview;
        QCheckBox* chkAutorangeOverview;
        QFDoubleEdit* edtOvrMin;
        QFDoubleEdit* edtOvrMax;
        QFDoubleEdit* edtOvr2Min;
        QFDoubleEdit* edtOvr2Max;




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
        QAction* actInsertSelectedCorrelationsAsFCSRDR;
        QAction* actCopyFitResultStatistics;
        QComboBox* cmbMaskEditMode;
        QAction* actCopyMaskToAll;
        QAction* actSelectionBuilder;
        QToolButton* btnSelectionBuilder;
        QAction* actMaskBuilder;
        QAction* actSetBackgroundFromSelection;
        QToolButton* btnSetBackgroundFromSelection;
        QAction* actSetBackgroundFromMasked;
        QAction* actPostProcessBinning;

        /** \brief combobox to select a result group */
        QComboBox* cmbResultGroup;


        /** \brief combobox to select a parameter from the result group */
        QComboBox* cmbParameter;
        QLabel* labParameter;
        QLabel* labParameterTransform;
        QComboBox* cmbParameterTransform;
        /** \brief combobox to select a goodnes of fit parameter from the result group */
        QComboBox* cmbParameter2;
        QLabel* labParameter2;
        QLabel* labParameter2Transform;
        QComboBox* cmbParameter2Transform;

        /** \brief table for the fit params */
        QEnhancedTableView* tvParams;

        QFTableModel* tabFitvals;


        /** \brief combobox to select a stored selection */
        QComboBox* cmbStoredSelections;
        QToolButton* btnSaveSelectionToStored;
        QToolButton* btnDeleteStoredSelection;
        QAction* actSaveSelectionToStored;
        QAction* actDeleteStoredSelection;

        QToolButton* btnCopySelection;
        QToolButton* btnPasteSelection;
        QToolButton* btnCopyMask;
        QToolButton* btnPasteMask;
        QAction* actCopySelection;
        QAction* actPasteSelection;



        /** \brief plotter widget for the parameter histogram */
        QGridLayout* histLay;
        QFHistogramView* histogram;
        QFHistogramView* histogram_2;
        QCheckBox* chkExcludeExcludedRunsFromHistogram;
        QCheckBox* chkExcludeExcludedRunsFromHistogram_2;
        QFHistogramView* histogram2;
        QFHistogramView* histogram2_2;
        QCheckBox* chkExcludeExcludedRunsFromHistogram2;
        QCheckBox* chkExcludeExcludedRunsFromHistogram2_2;

        QComboBox* cmbCorrelationDisplayMode;
        QSpinBox* spinCorrelationChannel;
        QFParameterCorrelationView* corrView;

        QColor selectionColor;
        QColor excludedColor;


        QCheckBox* chkOtherFileP2;
        QComboBox* cmbOtherFilesResultGroup;
        QComboBox* cmbOtherFileRole;




        /** \brief tab widget to select whether to display ACF, histogram, ... */
        QTabWidget* tabDisplay;



        /** \brief set which contains all currently selected runs */
        QSet<int32_t> selected;

        /** \brief returns true, if a given run/pixel is inside the second DualView channel */
        bool indexIsDualView2(int32_t sel);

        void selectedInsert(int idx);
        void selectedRemove(int idx);

        QString lastSavePath;

        QTimer* timUpdateAfterClick;

        QString lastMaskDir;


        void excludeByImage(double *imageIn);
        void selectByImage(double *imageIn);

        void getCurrentResultNamesAndLabels(QStringList& names, QStringList& labels);


        /** \brief create a parameter image with the given evalGroup and fitParam */
        void readParameterImage(double* image, uint32_t width, uint32_t height, QString evalGroup, QString fitParam, QFRDRImagingFCSImageEditor::ImageTransforms tranFitParam, bool thisRDR=true, const QString& otherRDRRole=QString(""), const QString& otherRDRevalGroup=QString(""));

        /** \brief apply the given transformation to the image */
        void transformImage(double *image, uint32_t width, uint32_t height, QFRDRImagingFCSImageEditor::ImageTransforms tranFitParam);

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
        bool evaluateFitFunction(QFRawDataRecord* current, const double* tau, double* fit, uint32_t N, QStringList& names, QStringList& namelabels, QList<double>& values, QList<double>& errors, QList<bool>& fix, QStringList& units, QStringList& unitlabels, QString evaluation, int index, QStringList *ids=NULL);

        /*! \brief fill a QComboBox with the names (and IDs as data) of all plotable fit parameters */
        void fillParameterComboBox(QComboBox* cmbParameter, QFRDRImagingFCSData* m, const QString& egroup, const QStringList &param1Default, const QString& otherParameter=QString(), const QString& indexPropertyName=QString(""), const QString& parameterDefault=QString("fitparam_n_particle"), int idxDefault=0);

        /*! \brief returns the RDR from which to read fit parameter 2 */
        QFRDRImagingFCSData* getRDRForParameter2(const QString& otherRDRRole) const;

        virtual void createReportDoc(QTextDocument* document);


        QString selectionToString();
        void loadSelectionFromString(QString sel);






        /** \brief a button which excludes the selected runs from the evaluation */
        QToolButton* btnDontUse;
        /** \brief a button which includes the selected runs to the evaluation */
        QToolButton* btnUse;
        /** \brief a button which includes all runs to the evaluation */
        QToolButton* btnUseAll;
        /** \brief a button which inverts the mask */
        QToolButton* btnInvertMask;
        /** \brief button to mask pixels by intensity */
        QToolButton* btnMaskByIntensity;
        /** \brief button to mask pixels by intensity */
        QToolButton* btnMaskByIntensity2;
         /** \brief button to mask pixels by parameter image 2 */
        QToolButton* btnMaskByParam2Intensity;
        /** \brief button to mask pixels by parameter image */
        QToolButton* btnMaskByParamIntensity;

        QToolButton* btnLoadSelection;
        QToolButton* btnLoadMask;
        QToolButton* btnSaveSelection;
        QToolButton* btnSaveMask;

        QToolButton* btnSetBackgroundFromMasked;
        QToolButton* btnCopyMaskToGroup;
        QToolButton* btnCopyFitResults;
        /** \brief action to print a report */
        QToolButton* btnPrintReport;

        /** \brief action to save a report */
        QToolButton* btnSaveReport;
        /** \brief action to save image data */
        QToolButton* btnSaveData;
        /** \brief action to copy image data to Matlab */
        QToolButton* btnCopyDataToMatlab;
        /** \brief action to copy image data as column table (CSV) */
        QToolButton* btnCopyDataAsColumns;

        QComboBox* cmbDualView;





        /** \brief a button which excludes the selected runs from the evaluation */
        QAction* actDontUse;
        /** \brief a button which includes the selected runs to the evaluation */
        QAction* actUse;
        /** \brief a button which includes all runs to the evaluation */
        QAction* actClearMask;
        /** \brief a button which inverts the mask */
        QAction* actInvertMask;
        /** \brief button to mask pixels by intensity */
        QAction* actMaskByIntensity;
        /** \brief button to mask pixels by intensity, channel 2 */
        QAction* actMaskByIntensity2;
        /** \brief button to mask pixels by parameter image 2 */
        QAction* actMaskByParam2Intensity;
        /** \brief button to mask pixels by parameter image */
        QAction* actMaskByParamIntensity;
        /** \brief action to select pixels by intensity */
        QAction* actSelectionByIntensity;
        /** \brief action to select pixels by intensity, channel 2 */
        QAction* actSelectionByIntensity2;
        /** \brief action to select pixels by parameter image 2 */
        QAction* actSelectByParam2Intensity;
        /** \brief action to select pixels by parameter image */
        QAction* actSelectByParamIntensity;
        /** \brief action to deselect masked pixels */
        QAction* actDeselctMasked;

        QAction* actLoadSelection;
        QAction* actSaveSelection;

        /** \brief action to save image data */
        QAction* actSaveData;
        /** \brief action to copy image data to Matlab */
        QAction* actCopyDataToMatlab;
        /** \brief action to copy image data to Matlab */
        QAction* actCopyDataAsColumns;

        QAction* actCopyGroupACFsToTable;
        QAction* actRecorrelate;
        QAction* actCopyMeanCFFromAll;
        QAction* actCopyPixelCFFromAll;
        QAction* actCopyPixelAvgCFFromAll;

        QMenu* menuData;
        QMenu* menuMask;
        QMenu* menuSelection;
        QMenu* menuImagingFCSTools;

        QFCorrelationMaskTools* correlationMaskTools;

        QStringList param1Default;
        QStringList param2Default;
};

#endif // QFRDRIMAGINGFCSEDITORIMAGE_H

