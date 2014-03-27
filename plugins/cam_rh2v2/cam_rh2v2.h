#ifndef __CAM_RH2V2_H__
#define __CAM_RH2V2_H__

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include "../interfaces/qfextensioncamera.h"
#include "processing_chain.h"
#include "qenhancedlineedit.h"
#include "jkstyledbutton.h"
#include "../base_classes/radhard2flash.h"
#include "qfradhard2flashtool.h"
#include "cam_rh2v2_cordlg.h"
#include "programoptions.h"
#include "qfpluginservices.h"

#define LOG_PREFIX "[RH2v2]: "


/*!
    \defgroup qf3ext_rh2v2 QFExtensionCamera implementation
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtensionCamera implementation
    \ingroup qf3ext_rh2v2
 */
class QFExtensionCameraRh2v2 : public QObject, public QFExtensionBase, public QFExtensionCamera {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFExtensionCamera)
    public:
        /** Default constructor */
        QFExtensionCameraRh2v2(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFExtensionCameraRh2v2();

        struct processingChainSettings{
            QSettings *settings;
            processing_chain *pc;
            bool enabled;
        };

        struct cameraSettings{
          struct processingChainSettings raw,cor;
          QString *prefix;
          unsigned int xRes,yRes;
          float pixelWidth,pixelHeight;
          float exposureTime;
          cam_rh2v2_cordlg *cordlg;
          QMap<QString, QVariant>* params;
        };


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("cam_rh2v2"); };
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("HW YAID USB driver"); };
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("Interface to sensors featuring streaming YAID format"); };
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan Buchholz"); };
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2011 by Jan Buchholz"); };
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/macromol"); };
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/cam_rh2v2.logo.png");};
        /** \copydoc QFExtension::deinit() */
        virtual void deinit();
        /** \copydoc QFPlugin::getVersion() */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };

    protected:
        /** \copydoc QFExtensionBase::projectChanged() */
        virtual void projectChanged(QFProject* oldProject, QFProject* project);
        /** \copydoc QFExtensionBase::initExtension() */
        virtual void initExtension();
        /** \copydoc QFExtensionBase::loadSettings() */
        virtual void loadSettings(ProgramOptions* settings);
        /** \copydoc QFExtensionBase::storeSettings() */
        virtual void storeSettings(ProgramOptions* settings); 

        /** \brief QAction used to program the FPGA */
        QAction* actProgramFPGA;

        bool setConfiguration(unsigned int camera, const QSettings &settings, const QString &setName);

    /////////////////////////////////////////////////////////////////////////////
    // QFExtensionCamera routines
    /////////////////////////////////////////////////////////////////////////////
    public:
        /** \copydoc QFExtensionCamera::getCameraCount() */
        virtual unsigned int getCameraCount() const;
        /** \copydoc QFExtensionCamera::showCameraSettingsWidget() */
        virtual void showCameraSettingsDialog(unsigned int camera, QSettings& settings, QWidget* parent=NULL);
        /** \copydoc QFExtensionCamera::useCameraSettings() */
        virtual void useCameraSettings(unsigned int camera, const QSettings& settings);
        /** \copydoc QFExtensionCamera::getImageWidth() */
        virtual int getCameraImageWidth(unsigned int camera);
        /** \copydoc QFExtensionCamera::getImageHeight() */
        virtual int getCameraImageHeight(unsigned int camera);
				/** \copydoc QFExtensionCamera::getCameraNme() */
				virtual QString getCameraName(unsigned int camera);
        /** \copydoc QFExtensionCamera::getCameraSensorName() */
        virtual QString getCameraSensorName(unsigned int camera);
        /** \copydoc QFExtensionCamera::isConnected() */
        virtual bool isCameraConnected(unsigned int camera);
        /** \copydoc QFExtensionCamera::acquire() */
        virtual bool acquireOnCamera(unsigned int camera, uint32_t* data, uint64_t* timestamp=NULL, QMap<QString, QVariant>* parameters=NULL);
        /** \copydoc QFExtensionCamera::connectDevice() */
        virtual bool connectCameraDevice(unsigned int camera);
        /** \copydoc QFExtensionCamera::disconnectDevice() */
        virtual void disconnectCameraDevice(unsigned int camera);
        /** \copydoc QFExtensionCamera::getExposureTime() */
        virtual double getCameraExposureTime(unsigned int camera);
        /** \copydoc QFExtensionCamera::setLogging() */
        virtual void setCameraLogging(QFPluginLogService* logService) { this->logService=logService; };

        /** \copydoc QFExtensionCamera::prepareAcquisition() */
        virtual bool prepareCameraAcquisition(unsigned int camera, const QSettings& settings, QString filenamePrefix=QString(""));
        /** \copydoc QFExtensionCamera::startAcquisition() */
        virtual bool startCameraAcquisition(unsigned int camera);
				/** \copydoc QFExtensionCamera::cancelAcquisition() */
        virtual void cancelCameraAcquisition(unsigned int camera);
        /** \copydoc QFExtensionCamera::isAcquisitionRunning() */
        virtual bool isCameraAcquisitionRunning(unsigned int camera, double* percentageDone=NULL);
        /** \copydoc QFExtensionCamera::getAcquisitionDescription() */
        virtual void getCameraAcquisitionDescription(unsigned int camera, QList<QFExtensionCamera::CameraAcquititonFileDescription>* files, QMap<QString, QVariant>* parameters);
        /** \copydoc QFExtensionCamera::getAcquisitionPreview() */
        virtual bool getCameraAcquisitionPreview(unsigned int camera, uint32_t* data);
        /** \copydoc QFExtensionCamera::getAcquisitionProgress() */
        virtual int getCameraAcquisitionProgress(unsigned int camera);
        /** \copydoc QFExtensionCamera::getPixelWidth() */
        virtual double getCameraPixelWidth(unsigned int camera);
        /** \copydoc QFExtensionCamera::getPixelHeight() */
        virtual double getCameraPixelHeight(unsigned int camera);



        /** \copydoc QFExtensionCamera::isCameraSettingChangable() */
        virtual bool isCameraSettingChangable(QFExtensionCamera::CameraSetting which) ;
        /** \copydoc QFExtensionCamera::changeCameraSetting() */
        virtual void changeCameraSetting(QSettings& settings, QFExtensionCamera::CameraSetting which, QVariant value);
        /** \copydoc QFExtensionCamera::getCameraSetting() */
        virtual QVariant getCameraSetting(QSettings& settings, QFExtensionCamera::CameraSetting which) ;
        /** \copydoc QFExtensionCamera::getCurrentCameraSetting() */
        virtual QVariant getCameraCurrentSetting(int camera, CameraSetting which) ;

        /** \brief returns \c true if the given CameraSetting is changable by changeCameraSetting() */
        inline virtual bool isCameraSettingChangable(const QString& which)  { return isCameraSettingChangable(QStringToSetting(which)); }
        /*! \brief change the given CameraSetting in the given QSettings object

            \note <b>this will change the contents of your QSettings object, so possibly this changed version is also written back to the harddisk!!!</b>
         */
        inline virtual void changeCameraSetting(QSettings& settings, const QString& which, QVariant value)  {  changeCameraSetting(settings, QStringToSetting(which), value); }
        /** \brief extract the given CameraSetting from the given QSettings object */
        inline virtual QVariant getCameraSetting(QSettings& settings, const QString& which)  { return getCameraSetting(settings, QStringToSetting(which)); }
        /** \brief extract the given CameraSetting from the given camera */
        inline virtual QVariant getCameraCurrentSetting(int camera, const QString& which)  { return getCameraCurrentSetting(camera, QStringToSetting(which)); }


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



        /*! \brief program the given \a bitfile into the FPGA (m for master or s for slave)

            \param bitfile which file to flash
            \param fpga \c 'm' to program master FPGA or \c 's' to program slave FPGA
            \param[out] messageOut contains the messages output by the flashing procedure
            \param retries number of retries when programming was not successfull
            \param retryDelayMS delay between two retries in milliseconds
            \return \c true on successfull programming
        */
        bool flashFPGA(QString autobitfile, char fpga, QString& messageOut, int retries=10, int retryDelayMS=500);

	protected:
        QFPluginLogService* logService;
        bool findGroupByType(const QString &t, const unsigned int camera, QString &group);
        bool findGroupByType(const QString &t, QSettings &settings, QString &group);
        struct cameraSettings* cameraSetting;
        QString bitfileMaster;
        QString bitfileSlave;
        QString autoflashbitfileMaster;
        QString autoflashbitfileSlave;
        bool autoflash;
        int retries;
        int retryDelay;
        bool connected;
				
	public:
        bool reconfigure(unsigned int camera, const QSettings &settings, const QString& setName);
        void reconfigure2(unsigned int camera, const QSettings &settings, const QString& postfix);
        bool flashFPGA(unsigned int camera);
protected slots:
        void reconfigureDialog(int camera);
        void logger_txt(QString message){log_text(LOG_PREFIX+message);}
		void logger_wrn(QString message){log_warning(LOG_PREFIX+message);}
		void logger_err(QString message){log_error(LOG_PREFIX+message);}
        void programFPGA();
        void acquisitionFinished(int camera);

};

#endif // QFEXTENSIONCAMERA_H
