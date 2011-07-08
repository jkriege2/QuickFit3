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

#include <unistd.h>





#ifndef __WINDOWS__
# if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#  define __WINDOWS__
# endif
#endif

#ifndef __LINUX__
# if defined(linux)
#  define __LINUX__
# endif
#endif

#ifndef __WINDOWS__
# ifndef __LINUX__
#  warning("these methods are ment to be used under windows or linux ... no other system were tested")
# endif
#endif


#ifdef __WINDOWS__
#  include "ATMCD32D.H"
#else
#  include "atmcdLXd.h"
#endif

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


    protected:
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

        /** \brief path supplied to the Initialize() function (i.e. path containing detectors.ini) */
        QString detectorsIniPath;
        QString detectorsIniPath_init;

        /** \brief stores information about a camera */
        struct CameraInfo {
            /** \brief width of image in pixel */
            int width;
            /** \brief height of image in pixel */
            int height;

            /** \brief acquisition mode */
            int AcqMode;
            /** \brief read mode   0: FVB, 1: Multi-Track, 2: Random-Track, 3: Sngle-Track, 4: Image*/
            int ReadMode;
            /** \brief shutter mode 0: auto, 1: open, 2: close */
            int shutterMode;
            /** \brief shutter closing time in milliseconds */
            int shutterClosingTime;
            /** \brief shutter opening time in milliseconds */
            int shutterOpeningTime;

            /** \brief exposure time in seconds */
            float expoTime;
            /** \brief trigger mode */
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

            /** \brief cooler on/off  */
            bool coolerOn;

            /** \brief set temperature °C */
            int setTemperature;

            /** \brief EM-GAIN on/off */
            bool emgain_on;
            /** \brief advanced EM-GAIN mode */
            bool advancedEMGain;
            /** \brief EM-GAIN factor */
            int emgain;

            /** \brief output amplifier mode */
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

            /** \brief mode of the inertanl fan 0: full, 1: low, 2: off */
            int fanMode;

            /** \brief baseline offset value */
            int baselineOffset;
            /** \brief enable/disable baseline clamp */
            bool baselineClamp;

            CameraInfo();
        };

        /** \brief a set of all connected cameras */
        QSet<int> camConnected;

        /** \brief configure the given camera with the settings from the provided CameraInfo object */
        bool setCameraSettings(int camera, CameraInfo& info);

        /** \brief this map stores infos about all connected cameras. If a camer is not in the map, it is not connected */
        QMap<int, CameraInfo> camInfos;

        /** \brief this map contains all threads that control the available cameras. Each camera has it's own thread! */
        QMap<int, CamAndorAcquisitionThread*> camThreads;


};

#endif // CAM_ANDOR
