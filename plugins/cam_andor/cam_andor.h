#ifndef CAM_ANDOR_H
#define CAM_ANDOR_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include "../interfaces/qfextensioncamera.h"
#include "highrestimer.h"
#include "qenhancedlineedit.h"
#include "jkstyledbutton.h"
#include "camandoracquisitionthread.h"
#include <QMap>
#include <QSet>
#include "andorglobalcamerasettingswidget.h"
#include "flowlayout.h"
#include <unistd.h>
#include <QtPlugin>




/*!
    \defgroup qf3ext_andor QFExtensionCamera implementation for Andor Cameras
    \ingroup qf3extensionplugins
*/



/*! \brief QFExtensionCamera implementation for Andor cameras
    \ingroup qf3ext_andor

 */
class QFExtensionCameraAndor : public QObject, public QFExtensionBase, public QFExtensionCamera {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFExtensionCamera)
    public:
        /** Default constructor */
        QFExtensionCameraAndor(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFExtensionCameraAndor();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("cam_andor"); };
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("Andor camera driver"); };
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("reads images from andor cameras."); };
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan Krieger"); };
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2011 by J. Krieger"); };
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr(""); };
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/cam_andor_logo.png"); };
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };
        /** \copydoc QFExtension::deinit() */
        virtual void deinit();

    protected:
        QFPluginLogService* logService;

        /** \copydoc QFExtensionBase::projectChanged() */
        virtual void projectChanged(QFProject* oldProject, QFProject* project);
        /** \copydoc QFExtensionBase::initExtension() */
        virtual void initExtension();
        /** \copydoc QFExtensionBase::loadSettings() */
        virtual void loadSettings(ProgramOptions* settings);
        /** \copydoc QFExtensionBase::storeSettings() */
        virtual void storeSettings(ProgramOptions* settings);


    /////////////////////////////////////////////////////////////////////////////
    // QFExtensionCamera routines
    /////////////////////////////////////////////////////////////////////////////
    public:
        /** \copydoc QFExtensionCamera::getCameraCount() */
        virtual unsigned int getCameraCount();
        /** \copydoc QFExtensionCamera::showCameraSettingsDialog() */
        virtual void showCameraSettingsDialog(unsigned int camera, QSettings& settings, QWidget* parent=NULL);
        /** \copydoc QFExtensionCamera::useCameraSettings() */
        virtual void useCameraSettings(unsigned int camera, const QSettings& settings);
        /** \copydoc QFExtensionCamera::getImageWidth() */
        virtual int getImageWidth(unsigned int camera);
        /** \copydoc QFExtensionCamera::getImageHeight() */
        virtual int getImageHeight(unsigned int camera);
        /** \copydoc QFExtensionCamera::isConnected() */
        virtual bool isConnected(unsigned int camera);
        /** \copydoc QFExtensionCamera::acquire() */
        virtual bool acquire(unsigned int camera, uint32_t* data, uint64_t* timestamp=NULL);
        /** \copydoc QFExtensionCamera::connectDevice() */
        virtual bool connectDevice(unsigned int camera);
        /** \copydoc QFExtensionCamera::disconnectDevice() */
        virtual void disconnectDevice(unsigned int camera);
        /** \copydoc QFExtensionCamera::getExposureTime() */
        virtual double getExposureTime(unsigned int camera);
        /** \copydoc QFExtensionCamera::setLogging() */
        virtual void setLogging(QFPluginLogService* logService) { this->logService=logService; };


        /** \copydoc QFExtensionCamera::prepareAcquisition() */
        virtual bool prepareAcquisition(unsigned int camera, const QSettings& settings, QString filenamePrefix=QString(""));
        /** \copydoc QFExtensionCamera::startAcquisition() */
        virtual bool startAcquisition(unsigned int camera);
        /** \copydoc QFExtensionCamera::cancelAcquisition() */
        virtual void cancelAcquisition(unsigned int camera);
        /** \copydoc QFExtensionCamera::isAcquisitionRunning() */
        virtual bool isAcquisitionRunning(unsigned int camera, double* percentageDone=NULL);
        /** \copydoc QFExtensionCamera::getAcquisitionDescription() */
        virtual void getAcquisitionDescription(unsigned int camera, QStringList* files, QMap<QString, QVariant>* parameters);
        /** \copydoc QFExtensionCamera::getAcquisitionPreview() */
        virtual bool getAcquisitionPreview(unsigned int camera, uint32_t* data);
        /** \copydoc QFExtensionCamera::getAcquisitionProgress() */
        virtual int getAcquisitionProgress(unsigned int camera);

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

        static QString andorErrorToString(unsigned int error);

    signals:
        /** \brief signal that may be used to display the current exposure time as a string */
        void displayAcquisitionTime(const QString& time);

    protected slots:
        /** \brief update the current temperature of the sensors ... calls itself again using a QTimer::singleShot() */
        void updateTemperatures();


        /** \brief called when global settings have changed */
        void globalSettingsChanged(int camera, int fan_mode, bool cooling_on, int temperature, int shutterMode);
    protected:
        /*! \brief stores global settings for a camera

             stores global settings for a camera, which are not included in CameraInfo and therefore should not be set on a
             settings basis, as used by useCameraSettings() or prepareAcquisition(). These settings can be set using menu
             commands provided by the extension plugin.

             Currently these global settings mostly include fan and temperature control!

             When connectDevice() is called, the global settings for the specified camera are applied. They will then only
             be applied again, if the user changes them.

             The functions initExtension() and connectDevice() will load the global settings from a global ini file as needed.
             The function deinit() will write the current settings back. A writeback may also occur in intermediate steps, but
             is not guaranteed to!
        */
        struct CameraGlobalSettings {
            /** \brief cooler on/off  */
            bool coolerOn;

            /** \brief set temperature °C */
            int setTemperature;

            /** \brief mode of the inertanl fan 0: full, 1: low, 2: off */
            int fanMode;

            /** \brief shutter mode 0: auto, 1: open, 2: close */
            int shutterMode;
            /** \brief shutter closing time in milliseconds */
            int shutterClosingTime;
            /** \brief shutter opening time in milliseconds */
            int shutterOpeningTime;


            /** \brief set default values */
            CameraGlobalSettings();

            /** \brief read these settings from QSettings object for given camera ID */
            void readSettings(QSettings& settings, int camera);
            /** \brief write these settings to QSettings object for given camera ID */
            void writeSettings(QSettings& settings, int camera) const;

        };

        /** \brief stores information about a camera */
        struct CameraInfo {
            /** \brief width of sensor in pixel  (read by readCameraProperties() from camera, so not change) */
            int width;
            /** \brief height of sensor in pixel  (read by readCameraProperties() from camera, so not change) */
            int height;

            /** \brief acquisition mode: 1: single scan, 2: accumulate, 3: kinetics, 4: fast kinetics, 5: run till abort */
            int AcqMode;
            /** \brief read mode   0: FVB, 1: Multi-Track, 2: Random-Track, 3: Sngle-Track, 4: Image*/
            int ReadMode;

            /** \brief exposure time in seconds */
            float expoTime;
            /** \brief trigger mode 0:internal, 1: external, 6: external start, 7: external exposure (bulb) 9: external FVB EM, 10: software trigger  */
            int trigMode;
            /** \brief number of frames in kinetic series */
            int numKins;
            /** \brief number of accumulations */
            int numAccs;
            /** \brief kinetic cycle time in seconds */
            float kinTime;
            /** \brief accumulation cycle time in seconds */
            float accTime;
            /** \brief defines the subregion to read out */
            QRect subImage;
            /** \brief horicontal binning */
            int hbin;
            /** \brief vertical binning */
            int vbin;
            /** \brief spooling mode? */
            int spool;


            /** \brief advanced EM-GAIN mode */
            bool advancedEMGain;
            /** \brief EM-GAIN factor */
            int emgain;
            /** \brief output amplifier mode: 0: EMCCD (default) 1: CCD (switch EM gain off) */
            int outputAmplifier;

            /** \brief preamplifier gain */
            float preamp_gain;

            /** \brief vertical shift amplitude (0, +1..+4)*/
            int vsAmplitude;

            /** \brief vertical shift speed */
            int vsSpeed;

            /** \brief horicontal shift speed */
            int hsSpeed;

            /** \brief crop mode on/off */
            bool cropMode;

            /** \brief frame transfer on/off */
            bool frameTransfer;

            /** \brief baseline offset value */
            int baselineOffset;
            /** \brief enable/disable baseline clamp */
            bool baselineClamp;

            /** \brief camera head model (read by readCameraProperties() from camera, so not change ) */
            QString headModel;
            /** \brief controller card (read by readCameraProperties() from camera, so not change ) */
            QString controllerCard;
            /** \brief camera serial number (read by readCameraProperties() from camera, so not change ) */
            int serialNumber;

            /** \brief the AD channel to use for reading */
            int ADchannel;
            /** \brief bit depth of the AD channel (read by readCameraProperties() from camera, so not change) */
            int bitDepth;
            /** \brief preamp gain (read by readCameraProperties() from camera, so not change) */
            float preampGainF;

            /** \brief width of the pixels in microns (read by readCameraProperties() from camera, so not change) */
            float pixelWidth;
            /** \brief height of the pixels in microns (read by readCameraProperties() from camera, so not change) */
            float pixelHeight;
            /** \brief speed of vertical shift in microns/pixelshift (read by readCameraProperties() from camera, so not change) */
            int verticalSpeed;
            /** \brief speed of horizontal shift in MHz (read by readCameraProperties() from camera, so not change) */
            float horizontalSpeed;

            /** \brief readout time of sensor */
            float readoutTime;


            CameraInfo();
        };







        /** \brief are we connected? */
        bool conn;
        /** \brief timer */
        HighResTimer timer;



        /** \brief select the given camera */
        bool selectCamera (int iSelectedCamera);

        /** \brief get temperature from camera i */
        int getTemperature(int cam);

        /** \brief set the temperature and cooler of the given camera,
         *
         * fan mode of the inertanl fan 0: full, 1: low, 0: off
         */
        bool setTemperature(int camera, bool coolerOn, int temperature, int fanMode=0);

        /** \brief set the shutter mode */
        bool setShutter(int camera, int mode, int closingtime=50, int openingtime=50);

        /** \brief get camera info as HTML string for the given \A camera. The other options switch on/off parts of the report*/
        QString getCameraInfo(int camera, bool showHeadModel=true, bool showSensorSize=true, bool extendedInfo=false, bool currentSettings=false);

        /** \brief path supplied to the Initialize() function (i.e. path containing detectors.ini) */
        QString detectorsIniPath;
        QString detectorsIniPath_init;



        /** \brief a set of all connected cameras */
        QSet<int> camConnected;

        /** \brief configure the given camera with the settings from the provided CameraInfo object */
        bool setCameraSettings(int camera, CameraInfo& info);

        /** \brief read the settings of the current camera (fill some of the fields in CameraInfo) */
        void readCameraProperties(int camera, CameraInfo& info);

        /** \brief this map stores infos about all connected cameras. If a camer is not in the map, it is not connected */
        QMap<int, CameraInfo> camInfos;

        /** \brief this map stores global settings (temperature, fan mode ...) about all connected cameras. If a camer is not in the map, it is not connected */
        QMap<int, CameraGlobalSettings> camGlobalSettings;

        /** \brief this map conatins all widgets used to display/edit the global settings */
        QMap<int, AndorGlobalCameraSettingsWidget*> camGlobalSettingsWidgets;

        /** \brief window, used to display the current global settings */
        QWidget* dlgGlobalSettings;

        /** \brief flow layout on dlgGlobalSettings */
        FlowLayout* dlgGlobalSettings_layout;

        /** \brief write contents of camGlobalSettings to global ini file */
        void storeGlobalSettings();

        /*! \brief set the global settings of the given camera, or all cameras (if \a cam \c < \c 0, default)

            \return \ true on success and \c false if an error occured (also if an error occured only on some of the cameras, for \c cam==-1 )

            This function internally calls setTemperature()!
        */
        bool setGlobalSettings(int cam=-1);

        /** \brief this map contains all threads that control the available cameras. Each camera has it's own thread! */
        QMap<int, CamAndorAcquisitionThread*> camThreads;

        /** \brief the SDK version */
        QString SDKVersion;
        /** \brief device driver version */
        QString deviceDriverVersion;

        /** \brief acquire a single frame with current settings, but before set the image size to full sensor! */
        bool acquireFullFrame(unsigned int camera, uint32_t* data, uint64_t* timestamp=NULL);
};

#endif // CAM_ANDOR
