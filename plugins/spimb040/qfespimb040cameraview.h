#ifndef QFESPIMB040CAMERAVIEW_H
#define QFESPIMB040CAMERAVIEW_H

#include <QWidget>
#include "qvisiblehandlesplitter.h"
#include <QSplitter>
#include <QWidget>
#include <QComboBox>
#include <QSettings>
#include <QMenu>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <cmath>
#include <QLabel>
#include <QPushButton>
#include <QToolBar>
#include <QTime>
#include <QTabWidget>

#include "programoptions.h"
#include "jkimage.h"
#include "jkqtfastplotter.h"
#include "jkqttools.h"
#include "colorcombobox.h"
#include "tools.h"
#include "qfextensionmanager.h"
#include "qfcameracombobox.h"
#include "qfcameraconfigcombobox.h"



/** \brief defines the type of the internal image representation of QFESPIMB040CameraView::image, may be uint32_t OR double OR float (nothing else!!!)  */
#define QFESPIMB040CameraView_internalImageType uint32_t

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg) camera view widget
    \ingroup qf3ext_spimb040

    This widget displays a single image + some image statistics (histograms ...). To display a new image, call
    displayImage(). This triggers the display of a new image.
 */
class QFESPIMB040CameraView : public QWidget {
        Q_OBJECT
    public:
        /*! \brief class contructor

            \param cameraID ID of the camera to use
            \param pluginServices  QFPluginServices object used to access QuickFit basic services
            \param stopresume (if supplied) allows to stop/resume a preview in order to save an image
            \param parent the parent widget
         */
        QFESPIMB040CameraView(int cameraID, QFPluginServices* pluginServices, QFCameraConfigComboBoxStartResume* stopresume, QWidget* parent=NULL);
        virtual ~QFESPIMB040CameraView();
        /** \brief load settings */
        void loadSettings(ProgramOptions* settings, QString prefix);
        /** \brief save settings */
        void storeSettings(ProgramOptions* settings, QString prefix);

        /** \brief set size of pixels */
        void setPixelSize(double pixelWidth, double pixelHeight);

    public slots:
        /** \brief display a new image in the widget, this does not gurantee an update of the statistics

            \param image points to the data of the image to be displayed This points to a 1D array with the size \c width*height*sizeof(unit32_t)
            \param width width of \a image
            \param height height of \a image
            \param timeindex time index of image \a image in seconds
            \param exposuretime exposure time of the image in seconds (used to calculate count rates in units of kHz).

            \note This does NOT guarantee that the statistics are calculated for every frame (to increase frame rate!). If you need
                  the statistics, call displayImageComplete() instead.
        */
        void displayImage(JKImage<uint32_t>& image, double timeindex, double exposuretime);
        /** \brief display a new image in the widget, in comparison to displayImage() this guarantees an update of the statistics!

            \param image points to the data of the image to be displayed This points to a 1D array with the size \c width*height*sizeof(unit32_t)
            \param width width of \a image
            \param height height of \a image
            \param timeindex time index of image \a image in seconds
            \param exposuretime exposure time of the image in seconds (used to calculate count rates in units of kHz).
        */
        void displayImageComplete(JKImage<uint32_t>& image, double timeindex, double exposuretime);
        /** \brief clear the display */
        void clearImage();
        /*! \brief display the current camera data/config
            \param camera name of the current camera
            \param framerate current framerate in frames per second
         */
        void displayCameraConfig(QString camera, double framerate);

        /** \brief add a QAction to the main toolbar */
        void addUserAction(QAction* action);
        /** \brief delete a QAction from the main toolbar */
        void deleteUserAction(QAction* action);
    protected slots:
        /** \brief save a report of the evaluation results */
        void saveReport();
        /** \brief save the marginals and histogram data */
        void saveData();
        /** \brief print a report of the evaluation results */
        void printReport();
    protected:

        /*! \brief create an evaluation report for the current record */
        void createReportDoc(QTextDocument* document);
        /** \brief create main widgets */
        void createMainWidgets();
        /** \brief create actions and register them to toolbar */
        void createActions();


        QVisibleHandleSplitter* splitHor;
        QVisibleHandleSplitter* splitVert;
        QToolBar* toolbar;
        QTabWidget* tabSettings;
        QTabWidget* tabResults;

        QFPluginServices* m_pluginServices;
        QFCameraConfigComboBoxStartResume* m_stopresume;




        /** \brief main image plotter widget */
        JKQTFastPlotter* pltMain;
        /** \brief label for current position and value below it on the image plot */
        QLabel* labCurrentPos;
        /** \brief plot used to display the current frame */
        JKQTFPimagePlot* plteFrame;
        /** \brief plot used to display an overlay with the broken pixels saved in mask */
        JKQTFPimageOverlayPlot* plteMask;
        /** \brief plot used to display the position where the marginal plots are taken */
        JKQTFPVCrossPlot* plteMarginalPos;
        /** \brief plot used to display the range in the histogram */
        JKQTFPXRangePlot* plteHistogramRange;
        /** \brief plot used to display the histogram */
        JKQTFPVBarPlot* plteHistogram;

        /** \brief distance measure plot */
        JKQTFPLinePlot* plteMainDistance;
        /** \brief bottom marginal plotter widget */
        JKQTFastPlotter* pltMarginalBottom;
        /** \brief bottom marginal plot */
        JKQTFPLinePlot* plteMarginalBottom;
        /** \brief bottom marginal plot x data */
        double* pltDataMarginalBottomX;
        /** \brief position where to take marginal plot (if single line is selected) */
        uint32_t pltDataMarginalXPixel;
        double pltDataMarginalXPixelF;
        /** \brief bottom marginal plot y data */
        double* pltDataMarginalBottomY;
        /** \brief bottom marginal plot number of points */
        uint32_t pltDataMarginalBottomN;
        /** \brief left marginal plotter widget */
        JKQTFastPlotter* pltMarginalLeft;
        /** \brief left marginal plot */
        JKQTFPLinePlot* plteMarginalLeft;
        /** \brief bottom marginal plot x data */
        double* pltDataMarginalLeftX;
        /** \brief bottom marginal plot y data */
        double* pltDataMarginalLeftY;
        /** \brief bottom marginal plot number of points */
        uint32_t pltDataMarginalLeftN;
        /** \brief position where to take marginal plot (if single line is selected) */
        uint32_t pltDataMarginalYPixel;
        double pltDataMarginalYPixelF;
        /** \brief combobox to select the type of marginal plot */
        QComboBox* cmbMarginalPlots;

        double pltDataMarginalLeftYMin;
        double pltDataMarginalLeftYMax;
        double pltDataMarginalBottomYMin;
        double pltDataMarginalBottomYMax;



        /** \brief main image plotter widget */
        JKQTFastPlotter* pltCountsHistogram;
        /** \brief spin edit for lower count range bound */
        QDoubleSpinBox* spinCountsLower;
        /** \brief spin edit for upper count range bound */
        QDoubleSpinBox* spinCountsUpper;
        /** \brief check box to use automatic count range determination */
        QCheckBox* chkCountsRangeAutoLow;
        /** \brief check box to use automatic count range determination */
        QCheckBox* chkCountsRangeAutoHigh;
        /** \brief label for the image statistics output */
        QLabel* labImageStatistics;
        /** \brief spin edit for number of histogram bins */
        QSpinBox* spinHistogramBins;
        /** \brief check box to use automatic histogram bin determination */
        QCheckBox* chkHistogramBinsAuto;
        /** \brief check box to use logarithmic histogram y-scale */
        QCheckBox* chkHistogramLog;

        /** \brief combobox to select fit function */
        QComboBox* cmbMarginalFitFunction;
        /** \brief label for marginal fit results */
        QLabel* labMarginalFitResults;
        /** \brief left marginal fit plot */
        JKQTFPLinePlot* plteMarginalFitLeft;
        /** \brief bottom marginal fit plot */
        JKQTFPLinePlot* plteMarginalFitBottom;
        /** \brief bottom marginal fit plot y data */
        double* pltDataMarginalFitBottomY;
        /** \brief left marginal fit plot y data */
        double* pltDataMarginalFitLeftY;
        /** \brief bottom marginal fit plot x data */
        double* pltDataMarginalFitBottomX;
        /** \brief left marginal fit plot x data */
        double* pltDataMarginalFitLeftX;
        /** \brief marginal fit results as string */
        QString marginalResults;


        /** \brief combobox to select color bar */
        QComboBox* cmbColorscale;
        /** \brief color combobox for the overlay color */
        ColorComboBox* cmbMaskColor;
        /** \brief combobox for rotation of the image */
        QComboBox* cmbRotation;

        /** \brief label for image sie and framerate */
        QLabel* labVideoSettings;

        /** \brief checkbox to switch image statistics with histogram on/off */
        QCheckBox* chkImageStatisticsHistogram;




        /** \brief action to save the current raw image */
        QAction* actSaveRaw;
        /** \brief action to activate/disactivate mask editing mode */
        QAction* actMaskEdit;
        /** \brief action to clear mask */
        QAction* actMaskClear;
        /** \brief action to save mask */
        QAction* actMaskSave;
        /** \brief action to load mask */
        QAction* actMaskLoad;

        /** \brief action to activate/disactivate measuring mode */
        QAction* actMeasure;
        /** \brief action to activate/disactivate normal cursor mode mode */
        QAction* actCursor;

        /** \brief action to print a report */
        QAction* actPrintReport;

        /** \brief action to save a report */
        QAction* actSaveReport;
        /** \brief action to save marginal+histogram data */
        QAction* actSaveData;

        bool measureFirst;


        /*! \brief image data

            The image in this data member is drawn by redrawFrame(). The function prepareImage() copies the contents of
            \a rawImage (which represents the raw data read from the camera) into \a image and may possibly apply some
            filtering or data transformations!
         */
        JKImage<QFESPIMB040CameraView_internalImageType> image;
        /*! \brief the raw image data

            The image in this data member is NOT directly drawn by redrawFrame(). The function prepareImage() copies the contents of
            \a rawImage (which represents the raw data read from the camera) into \a image and may possibly apply some
            filtering or data transformations!
        */
        JKImage<uint32_t> rawImage;
        /*! \brief the mask image

            The pixels set to \c true in this image (which has to have the same size, as image and rawImage are used to mask
            broken pixels etc. when calculating image statistics, color scaling ... Note that when a call to prepareImage()
            implies a different image size, this image will be set to all \c false!
        */
        JKImage<bool> mask;

        bool maskEmpty;

        /** \brief indicates whether the statistics of the raw image have already been calculated */
        bool imageStatisticsCalculated;
        /** \brief image statistics: number of broken pixels */
        uint32_t imageBrokenPixels;
        /** \brief image statistics: sum of pixel values */
        double imageSum;
        /** \brief image statistics: average pixel value */
        double imageMean;
        /** \brief image statistics: standard deviation of pixel values */
        double imageStddev;
        /** \brief image statistics: minimum pixel value */
        QFESPIMB040CameraView_internalImageType imageImin;
        /** \brief image statistics: maximum pixel value */
        QFESPIMB040CameraView_internalImageType imageImax;
        /** \brief time index of the last frame in seconds */
        double imageTimeindex;
        /** \brief exposure time of the last frame in seconds */
        double imageExposureTime;


        /** \brief histogram x-values */
        double* histogram_x;
        /** \brief histogram y-values */
        double* histogram_y;
        /** \brief number of entries in the histogram */
        uint32_t histogram_n;
        /** \brief when was the count rate histogram updated the last time */
        QTime histogramUpdateTime;
        QTime labelUpdateTime;

        /** \brief path last used to save/load masks (*.msk) */
        QString lastMaskpath;
        /** \brief path last used to save images */
        QString lastImagepath;
        /** \brief filter last used to save images */
        QString lastImagefilter;

        /** \brief flag to indicate that we are currently redrawing */
        bool currentlyRedrawing;

        /** \brief width of a pixel in microns */
        double pixelWidth;
        /** \brief height of a pixel in microns */
        double pixelHeight;


        double measureX[2];
        double measureY[2];

    protected slots:
        /** \brief called when the mouse moves over the image plot window */
        void imageMouseMoved(double x, double y);
        /** \brief called when the mouse is clicked on the image plot window. This is used together with actMaskEdit to control editing of image masks */
        void imageMouseClicked(double x, double y);
        /** \brief en/disables autoscaling of coloring
         *
         * This method en/disables the input widgets for the upper/lower counts. It does not trigger a redraw, as
         * the input fields are only en-/disabled and the values are not changed, so the display should be up to date!
         * \note parameter is just dummy
         */
        void setCountsAutoscale(bool autoscale=true);
        /** \brief en/disables autoscaling of histogram bins
         *
         * This method en/disables the input widget for the histogram bins. It does not trigger a redraw, as
         * the input fields are only en-/disabled and the values are not changed, so the display should be up to date!
         */
        void setHistogramBinsAutoscale(bool autoscale);
        /** \brief redraws the frame with the current settings for color scale ... This reads data from the data member \a image
         *  \note this does NOT recalculate the statistics. For a full recalc/redraw, call displayImageStatistics() first, or
         *        call redrawFrameRecalc().
         */
        void redrawFrame();
        /** \brief this function prepares the \a image data memeber, i.e. it copies the contents of rawImage
         *         and possibly applies some math to it (background removal ...)
         */
        void prepareImage();
        /** \brief redraws the frame with the current settings for color scale ... and recalculate statistics and transforms */
        void redrawFrameRecalc(bool forceHisto=false);
        /** \brief calculate image statistics */
        void displayImageStatistics(bool withHistogram=true, bool forceHistogram=false);
        /** \brief clear mask */
        void clearMask();
        /** \brief save mask */
        void saveMask();
        /** \brief load mask */
        void loadMask();
        /** \brief save the current image */
        void saveRaw();

        void histogramChecked(bool checked);

};

#endif // QFESPIMB040CAMERAVIEW_H
