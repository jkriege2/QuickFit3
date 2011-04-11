#ifndef QFESPIMB040MAINWINDOW_H
#define QFESPIMB040MAINWINDOW_H

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
#include <QGroupBox>

#include "../../extlibs/cimg.h"

#include "programoptions.h"
#include "jkimage.h"
#include "qfespimb040cameraview.h"
#include "jkqttools.h"


#include "qtlogfile.h"
#include "qfextensionmanager.h"
#include "../interfaces/qfextensioncamera.h"
#include "qfextension.h"
#include "tools.h"

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg) main window
    \ingroup qf3ext_spimb040

    This window allows to configure a SPIM microscope setup. Images are displayed in sub windows.

    It also implements QFPluginLogService and may thus be used for logging.
 */
class QFESPIMB040MainWindow : public QWidget, public QFPluginLogService {
        Q_OBJECT
    public:
        QFESPIMB040MainWindow(QFExtensionManager* extManager, QWidget* parent=NULL);
        virtual ~QFESPIMB040MainWindow();

        /** \brief load settings */
        void loadSettings(ProgramOptions* settings);
        /** \brief save settings */
        void storeSettings(ProgramOptions* settings);
    protected:
        /** \brief struct that holds dta about the different camera devices, including the last raw image, ... */
        struct viewDataStruct {
            /** \brief initializing constructor sets all values to default values */
            viewDataStruct();
            /** \brief reset all values to default */
            void reset();
            /** \brief last raw imge */
            JKImage<uint32_t> rawImage;
            /** \brief time stamp of rawImage */
            uint64_t timestamp;
            /** \brief exposure time of rawImage in seconds */
            double exposureTime;
            /** \brief time of last acquisition */
            QTime lastAcquisitionTime;

            /** \brief number of acquired frames for frames/second calculation (is reset every 10 seconds!!!) */
            uint64_t acqFramesQR;
            /** \brief number of acquired frames  */
            uint64_t acqFrames;


            /** \brief when has the last continuous acquisition been started, used for calculation of acquisition time */
            QTime acqStartedQR;
            /** \brief when has the last continuous acquisition been started */
            QTime acqStarted;
            /** \brief internal flag: if \c true the current acquireContinuous() call will be aborted. */
            bool abort_continuous_acquisition;
            /** \brief internal flag: marks the first image (\c true ) within a continuous acquisition series */
            bool continuous_is_first;
            /** \brief points to the QFExtensionobject this is connected to */
            QFExtension* extension;
            /** \brief typecast of extension to QFExtensionCamera */
            QFExtensionCamera* camera;
            /** \brief which cmera of the QFExtensionCamera is used? */
            int usedCamera;

        };

        /** \brief list of all available QFExtensionCamera plugins, initialized by findCameras() */
        QList<QObject*> cameras;

        /** \brief list containing data about the acquisition devices connected to the views (last read image, when did acquisition start ...) */
        QList<viewDataStruct*> viewData;


        /** \brief main log widget */
        QtLogFile* logMain;


        /*! \brief combobox to select an acquisition device 1

            The data() property of this QComboBox is used to store/specify the device (and item therein) to use.
            It contains a QPoint, where x specifies the device in cameras and y the camera inside the selected
            device.
        */
        QComboBox* cmbAcquisitionDevice1;
        /** \brief spinbox to select delay between two subsequent frames, camera 1 */
        QDoubleSpinBox* spinAcquisitionDelay1;
        /** \brief view for images from camera 1 */
        QFESPIMB040CameraView* camView1;
        /** \brief action to connect/disconnect to acquisition device, camera 1 */
        QAction* actDisConnectAcquisition1;
        /** \brief action to start/stop continuous acquisition, camera 1 */
        QAction* actStartStopAcquisition1;
        /** \brief action to acquire a single frame, camera 1 */
        QAction* actAcquireSingle1;
        /** \brief action to display a camera configuration dialog, camera 1 */
        QAction* actCameraConfig1;
        /** \brief toolbar for camera 1 control */
        QToolBar* tbCamera1;
        /** \brief button to connect/disconnect to a device, camera 1 */
        QToolButton* btnConnect1;
        /** \brief button to acquire continuously, camera 1 */
        QToolButton* btnAcquireContinuous1;
        /** \brief button to acquire a single frame, camera 1 */
        QToolButton* btnAcquireSingle1;
        /** \brief button to configure camera 1 */
        QToolButton* btnConfig1;



        /*! \brief combobox to select an acquisition device 2

            The data() property of this QComboBox is used to store/specify the device (and item therein) to use.
            It contains a QPoint, where x specifies the device in cameras and y the camera inside the selected
            device.
        */
        QComboBox* cmbAcquisitionDevice2;
        /** \brief spinbox to select delay between two subsequent frames, camera 2 */
        QDoubleSpinBox* spinAcquisitionDelay2;
        /** \brief view for images from camera 2 */
        QFESPIMB040CameraView* camView2;
        /** \brief action to connect/disconnect to acquisition device, camera 2 */
        QAction* actDisConnectAcquisition2;
        /** \brief action to start/stop continuous acquisition, camera 2 */
        QAction* actStartStopAcquisition2;
        /** \brief action to acquire a single frame, camera 2 */
        QAction* actAcquireSingle2;
        /** \brief action to display a camera configuration dialog, camera 2 */
        QAction* actCameraConfig2;
        /** \brief toolbar for camera 2 control */
        QToolBar* tbCamera2;
        /** \brief button to connect/disconnect to a device, camera 2 */
        QToolButton* btnConnect2;
        /** \brief button to acquire continuously, camera 2 */
        QToolButton* btnAcquireContinuous2;
        /** \brief button to acquire a single frame, camera 2 */
        QToolButton* btnAcquireSingle2;
        /** \brief button to configure camera 2 */
        QToolButton* btnConfig2;







        /** \brief handles the close event, also close all camera views in camViews
         *
         *  disconnect devices and close camera view
         */
        void closeEvent ( QCloseEvent * event );

        /** \brief void find all available cameras (QFExtensionCamera), fills \a cameras QList */
        void findCameras(QFExtensionManager* extManager);

        /** \brief Create all widgets on this window, called in the constructor before createActions() */
        void createWidgets(QFExtensionManager* extManager);
        /** \brief Create ll QActions for this window, clled after createWidgets() in the constructor.
         *         So you also have to connect and add (e.g. to toolbars) QAction's here! */
        void createActions();



    protected slots:

        /** \brief connect/disconnect to acquisition device 1
         *
         * This method mainly sets the enabled properties of the QActions and widgets that play a role in device control.
         *
         * \note this function is a wrapper slot function and calls the respective protected function connectDevice()
                 or disconnectDevice().
         */
        void disConnectAcquisition1();
        /** \brief start/stop image continuous acquisition (depending on actStartStopAcquisition), camera 1
         *
         * \note this function is a wrapper slot function and calls the respective protected function stopAcquisition()
         */
        void startStopAcquisition1();
        /** \brief acquire a single frame from continuous series, camera 1
         *
         *  This function is called by a QTimer. The continuous series is started by startStopAcquisition1()
         *  This function actually reads one image from the connected camera and displays it.
         *
         */
        void acquireContinuous1();
        /** \brief acquire single frame, camera 1 */
        void acquireSingle1();
        /** \brief display a configuration dialog for camera 1 */
        void configCamera1();





        /** \brief connect/disconnect to acquisition device 2
         *
         * This method mainly sets the enabled properties of the QActions and widgets that play a role in device control.
         *
         * \note this function is a wrapper slot function and calls the respective protected function connectDevice()
         */
        void disConnectAcquisition2();
        /** \brief start/stop image continuous acquisition (depending on actStartStopAcquisition), camera 1 */
        void startStopAcquisition2();
        /** \brief acquire a single frame from continuous series, camera 1
         *
         *  This function is called by a QTimer. The continuous series is started by startStopAcquisition1()
         *  This function actually reads one image from the connected camera and displays it.
         */
        void acquireContinuous2();
        /** \brief acquire single frame, camera 1 */
        void acquireSingle2();
        /** \brief display a configuration dialog for camera 1 */
        void configCamera2();


    protected:
        /*! \brief connect camera view \a camView + camera  \a camView widgets to a given acquisition device and camera therein

            \param camView work with the given camera view
            \param device the device to connect to
            \param camera the camera inside \a device to connect to
            \return \c true on success
        */
        bool connectDevice(int camView, int device, int camera);
        /** \brief disconnect camera device \a camView

            \param camView work with the given camera view
        */
        void disconnectDevice(int camView);
        /*! \brief acquire a single image from the device connected to the fiven \a camView and store it in rawImages

            \param camView work with the given camera view
            \return \c true on success

            This function will be called in continuous and single acquisition modes!
         */
        bool acquireSingle(int camView);

    public:

        /** \brief log project text message
         *  \param message the message to log
         */
        virtual void log_text(QString message);
        /** \brief log project warning message
         *  \param message the warning message to log
         */
        virtual void log_warning(QString message);
        /** \brief log project error message
         *  \param message the error message to log
         */
        virtual void log_error(QString message);
};

#endif // QFESPIMB040MAINWINDOW_H
