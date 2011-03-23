#ifndef QFESPIMB040CAMERAVIEW_H
#define QFESPIMB040CAMERAVIEW_H

#include <QWidget>
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
#include "qtlogfile.h"
#include "qfextensionmanager.h"
#include "../interfaces/qfextensioncamera.h"
#include "qfextension.h"
#include "tools.h"

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg) camera widget
    \ingroup qf3ext_spimb040
 */
class QFESPIMB040CameraView : public QWidget, public QFPluginLogService {
        Q_OBJECT
    public:
        QFESPIMB040CameraView(const QString& logfile, QFExtensionManager* extManager, QWidget* parent=NULL);
        virtual ~QFESPIMB040CameraView();
        /** \brief load settings */
        void loadSettings(ProgramOptions* settings, QString prefix);
        /** \brief save settings */
        void storeSettings(ProgramOptions* settings, QString prefix);
	/** \brief disconnect camera */
	void diconnectCurrentCamra();
    protected:
        void closeEvent ( QCloseEvent * event );

        /** \brief create main widgets */
        void createMainWidgets(const QString& logfile);
        /** \brief create actions and register them to toolbar */
        void createActions();

        /** \brief list of all available QFExtensionCamera plugins */
        QList<QObject*> cameras;

        QSplitter* splitHor;
        QSplitter* splitVert;
        QToolBar* toolbar;
        QTabWidget* tabSettings;
        QTabWidget* tabResults;

        /*! \brief combobox to select an acquisition device

            The data() property of this QComboBox is used to store/specify the device (and item therein) to use.
            It contains a QPoint, where x specifies the device in cameras and y the camera inside the selected
            device.
        */
        QComboBox* cmbAcquisitionDevice;
        /** \brief spinbox to select delay between two subsequent frames */
        QDoubleSpinBox* spinAcquisitionDelay;

        /** \brief main log widget */
        QtLogFile* logMain;

        /** \brief main image plotter widget */
        JKQTFastPlotter* pltMain;
        /** \brief label for current position and value below it on the image plot */
        QLabel* labCurrentPos;
        /** \brief plot used to display the current frame */
        JKQTFPimagePlot* plteFrame;
        /** \brief plot used to display an overlay with the broken pixels saved in mask */
        JKQTFPimageOverlayPlot* plteMask;
        /** \brief plot used to display the range in the histogram */
        JKQTFPXRangePlot* plteHistogramRange;
        /** \brief plot used to display the histogram */
        JKQTFPVBarPlot* plteHistogram;

        /** \brief main image plotter widget */
        JKQTFastPlotter* pltCountsHistogram;
        /** \brief spin edit for lower count range bound */
        QDoubleSpinBox* spinCountsLower;
        /** \brief spin edit for upper count range bound */
        QDoubleSpinBox* spinCountsUpper;
        /** \brief check box to use automatic count range determination */
        QCheckBox* chkCountsRangeAuto;
        /** \brief label for the image statistics output */
        QLabel* labImageStatistics;
        /** \brief spin edit for number of histogram bins */
        QSpinBox* spinHistogramBins;
        /** \brief check box to use automatic histogram bin determination */
        QCheckBox* chkHistogramBinsAuto;
        /** \brief check box to use logarithmic histogram y-scale */
        QCheckBox* chkHistogramLog;


        /** \brief combobox to select color bar */
        QComboBox* cmbColorscale;
        /** \brief color combobox for the overlay color */
        ColorComboBox* cmbMaskColor;
        /** \brief combobox for rotation of the image */
        QComboBox* cmbRotation;

        /** \brief label for image sie and framerate */
        QLabel* labVideoSettings;




        /** \brief action to connect/disconnect to acquisition device */
        QAction* actDisConnectAcquisition;
        /** \brief action to start/stop continuous acquisition */
        QAction* actStartStopAcquisition;
        /** \brief action to acquire a single frame */
        QAction* actAcquireSingle;
        /** \brief action to display a camera configuration dialog */
        QAction* actCameraConfig;
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


        /** \brief the camera extension this class is connected to.
         *
         * This is \c NULL if the class is not connected to any device.
         */
        QFExtensionCamera* cam;





        /** \brief image data
         *
         * This is the image all calculations are done with, while rawImage is the data taken directly from the
         * sensor/camera. So if e.g. a background image should be removed, you will have to do something like
         * <code>image = rawImage-backgroundImage</code>
         *
         */
        JKImage<double> image;
        /** \brief the raw image data */
        JKImage<uint32_t> rawImage;
        /** \brief the mask image */
        JKImage<bool> mask;

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
        double imageImin;
        /** \brief image statistics: maximum pixel value */
        double imageImax;
        /** \brief time index of the last frame */
        uint64_t imageTimeindex;


        /** \brief histogram x-values */
        double* histogram_x;
        /** \brief histogram y-values */
        double* histogram_y;
        /** \brief number of entries in the histogram */
        uint32_t histogram_n;
        /** \brief when was the size of the count rate histogram updated the last time */
        QTime histogramUpdateTime;

        /** \brief path last used to save/load masks (*.msk) */
        QString lastMaskpath;
        /** \brief path last used to save images */
        QString lastImagepath;
        /** \brief filter last used to save images */
        QString lastImagefilter;

        /** \brief number of acquired frames for frames/second calculation (is reset every 10 seconds!!!) */
        uint64_t acqFramesQR;
        /** \brief number of acquired frames  */
        uint64_t acqFrames;
        /** \brief length of the last acquisition time in seconds */
        double acqTime;


        /** \brief when has the last continuous acquisition been started */
        QTime acqStarted;
        /** \brief internal flag: if \c true the current acquireContinuous() call will be aborted. */
        bool abort_continuous_acquisition;
        /** \brief internal flag: marks the first image (\c true ) within a continuous acquisition series */
        bool continuous_is_first;

    protected slots:
        /** \brief called when the mouse moves over the image plot window */
        void imageMouseMoved(double x, double y);
        /** \brief called when the mouse is clicked on the image plot window. This is used together with actMaskEdit to control editing of image masks */
        void imageMouseClicked(double x, double y);
        /** \brief en/disables autoscaling of coloring
         *
         * This method en/disables the input widgets for the upper/lower counts. It does not trigger a redraw, as
         * the input fields are only en-/disabled and the values are not changed, so the display should be up to date!
         */
        void setCountsAutoscale(bool autoscale);
        /** \brief en/disables autoscaling of histogram bins
         *
         * This method en/disables the input widget for the histogram bins. It does not trigger a redraw, as
         * the input fields are only en-/disabled and the values are not changed, so the display should be up to date!
         */
        void setHistogramBinsAutoscale(bool autoscale);
        /** \brief redraws the frame with the current settings for color scale ...
         *  \note this does NOT recalculate the statistics. For a full recalc/redraw, call displayImageStatistics() first, or
         *        call redrawFrameRecalc().
         */
        void redrawFrame();
        /** \brief this function prepares the image property, i.e. it copies the contents of rawImage
         *         and possibly applies some math to it (background removal ...) */
        void prepareImage();
        /** \brief redraws the frame with the current settings for color scale ... and recalculate statistics and transforms */
        void redrawFrameRecalc();
        /** \brief calculate image statistics */
        void displayImageStatistics();
        /** \brief clear mask */
        void clearMask();
        /** \brief save mask */
        void saveMask();
        /** \brief load mask */
        void loadMask();
        /** \brief save the current image */
        void saveRaw();


        /** \brief connect/disconnect to acquisition device
         *
         * This method mainly sets the enabled properties of the QActions and widgets that play a role in device control.
         */
        void disConnectAcquisition();
        /** \brief start/stop image continuous acquisition (depending on actStartStopAcquisition) */
        void startStopAcquisition();
        /** \brief acquire single frame */
        void acquireSingle();
        /** \brief acquire single frame from a continuous series */
        void acquireContinuous();
        /** \brief connect to a given acquisition device and camera therein */
        void connectDevice(int device, int camera);
        /** \brief make sure all images/arrays have the same/right size */
        void ensureImageSizes();
        /** \brief display a configuration dialog for the current camera */
        void configCamera();

        /** \brief indent all following lines in the logging pane */
        virtual void log_indent() { logMain->inc_indent(); };
        /** \brief undo former log_indent() */
        virtual void log_unindent() { logMain->dec_indent(); };

        /** \brief log project text message
         *  \param message the message to log
         */
        virtual void log_text(QString message) { logMain->log_text(message); };
        /** \brief log project warning message
         *  \param message the warning message to log
         */
        virtual void log_warning(QString message) { logMain->log_warning(message); };
        /** \brief log project error message
         *  \param message the error message to log
         */
        virtual void log_error(QString message) { logMain->log_error(message); };
};

#endif // QFESPIMB040CAMERAVIEW_H
