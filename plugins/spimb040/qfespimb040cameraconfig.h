#ifndef QFESPIMB040CAMERACONFIG_H
#define QFESPIMB040CAMERACONFIG_H

#include <QGroupBox>

class QFESPIMB040MainWindow; // forward

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

#include "programoptions.h"
#include "jkimage.h"
#include "qfespimb040cameraview.h"
#include "jkqttools.h"


#include "qtlogfile.h"
#include "qfextensionmanager.h"
#include "../interfaces/qfextensioncamera.h"
#include "qfextension.h"
#include "tools.h"
#include "qfcameracombobox.h"
#include "qfcameraconfigcombobox.h"

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg) QGropBox with a set of controls that allow to control a camera
    \ingroup qf3ext_spimb040


 */
class QFESPIMB040CameraConfig : public QGroupBox {
        Q_OBJECT
    protected:
        /** \brief states used in conjunction with displayStates() */
        enum States {Disconnected, Connected, Previewing,Inactive,Locked};
    public:
        /** Default constructor */
        QFESPIMB040CameraConfig(QFESPIMB040MainWindow* parent, int camViewID, QFExtensionServices* pluginServices);
        /** Default destructor */
        virtual ~QFESPIMB040CameraConfig();

        /** \brief load settings */
        void loadSettings(ProgramOptions* settings, QString prefix);
        /** \brief save settings */
        void storeSettings(ProgramOptions* settings, QString prefix);

        /*! \brief returns a pointer to the QFExtensionCamera and ensures exclusive access to one camera therein.

            This method first stops any preview or use of the <QFExtensionCamera, camera> pair managed by this class.
            Then it returns a pointer to QFExtensionCamera and which camera to use inside it. This object is then
            locked for further access until releaseCamera() is called.
            While the camera is locked, a second call to lockCamera() will always return \c false and NULL-pointers!

            \b Note that the camera has to be connected already! Else this function will return \c false.

            \return \c true if camera is available \c false else
            \param[out] extension pointer to the extension representing the camera
            \param[out] ecamera pointer to the QFExtensionCamera representing the camera (basically a typecast from \a extension
            \param[out] camera which camera inside \a ecamera to use.
            \param[out] acquisitionSettingsFilename filename of the acquisition settings selected by the suer
            \param[out] previewSettingsFilename filename of the preview settings selected by the suer
        */
        bool lockCamera(QFExtension** extension, QFExtensionCamera** ecamera, int* camera, QString* acquisitionSettingsFilename, QString* previewSettingsFilename);

        /*! \brief release a locked camera, for more details see lockCamera() */
        void releaseCamera();

    signals:
        /** \brief emitted when the set of configuration files changes */
        void configFilesChanged();


    protected:
        QFESPIMB040MainWindow* m_parent;
        int m_camViewID;
        QFExtensionManager* m_extManager;
        QFExtensionServices* m_pluginServices;

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

        /** \brief list containing data about the acquisition devices connected to the views (last read image, when did acquisition start ...) */
        viewDataStruct viewData;

        /** \brief indicates whether the camera is locked */
        bool locked;

        /*! \brief combobox to select an acquisition device

            The data() property of this QComboBox is used to store/specify the device (and item therein) to use.
            It contains a QPoint, where x specifies the device in cameras and y the camera inside the selected
            device.
        */
        QFCameraComboBox* cmbAcquisitionDevice;
        /** \brief spinbox to select delay between two subsequent frames */
        QDoubleSpinBox* spinAcquisitionDelay;
        /** \brief view for images from camera */
        QFESPIMB040CameraView* camView;
        /** \brief combobox for the selected camera preview configuration */
        QFCameraConfigEditorWidget* cmbPreviewConfiguration;
        /** \brief combobox for the selected camera acquisition configuration */
        QFCameraConfigEditorWidget* cmbAcquisitionConfiguration;


        /** \brief button to connect/disconnect to a device */
        QToolButton* btnConnect;
        /** \brief button to acquire continuously */
        QToolButton* btnPreviewContinuous;
        /** \brief button to acquire a single frame */
        QToolButton* btnPreviewSingle;


        /** \brief action to connect/disconnect to acquisition device */
        QAction* actDisConnect;
        /** \brief action to start/stop continuous acquisition */
        QAction* actStartStopPreview;
        /** \brief action to acquire a single frame */
        QAction* actPreviewSingle;


        /** \brief handles the close event, also close all camera views in camViews
         *
         *  disconnect devices and close camera view
         */
        void closeEvent ( QCloseEvent * event );

        /** \brief Create all widgets on this window, called in the constructor before createActions() */
        void createWidgets(QFExtensionManager* extManager);
        /** \brief Create ll QActions for this window, clled after createWidgets() in the constructor.
         *         So you also have to connect and add (e.g. to toolbars) QAction's here! */
        void createActions();

        /** \brief disconnect camera device \a camView

            \param camView work with the given camera view
        */
        void disconnectDevice();


        /*! \brief connect camera view \a camView + camera  \a camView widgets to a given acquisition device and camera therein

            \param extension the extension objectof thecamera to connect to
            \param cam same as extension, but mapped to QFExtensionCamera
            \param camera the camera inside \a device to connect to
            \return \c true on success
        */
        bool connectDevice(QFExtension* extension, QFExtensionCamera* cam, int camera);
        /*! \brief acquire a single image from the device connected to the fiven \a camView and store it in rawImages

            \param camView work with the given camera view
            \return \c true on success

            This function will be called in continuous and single acquisition modes!
         */
        bool acquireSingle();
    protected slots:

        /** \brief connect/disconnect to acquisition device
         *
         * This method mainly sets the enabled properties of the QActions and widgets that play a role in device control.
         *
         * \note this function is a wrapper slot function and calls the respective protected function connectDevice()
                 or disconnectDevice().
         */
        void disConnectAcquisitionDevice();
        /** \brief start/stop image continuous acquisition (depending on actStartStopAcquisition)
         *
         * \note this function is a wrapper slot function and calls the respective protected function stopAcquisition()
         */
        void startStopPreview();
        /** \brief acquire a single frame from continuous series
         *
         *  This function is called by a QTimer. The continuous series is started by startStopAcquisition1()
         *  This function actually reads one image from the connected camera and displays it.
         *
         */
        void previewContinuous();
        /** \brief acquire single preview frame */
        void previewSingle();

        /** \brief set enabled/disabled states of the actions and widgets according to the given parameter */
        void displayStates(States state);




    private:
};

#endif // QFESPIMB040CAMERACONFIG_H
