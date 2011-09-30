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

        struct cameraSettings{
          QSettings *settings_pc;
          processing_chain *pc;
          QString *prefix;
          unsigned int xRes,yRes;
          float pixelWidth,pixelHeight;
          float exposureTime;
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

    /////////////////////////////////////////////////////////////////////////////
    // QFExtensionCamera routines
    /////////////////////////////////////////////////////////////////////////////
    public:
        /** \copydoc QFExtensionCamera::getCameraCount() */
        virtual unsigned int getCameraCount();
        /** \copydoc QFExtensionCamera::showCameraSettingsWidget() */
        virtual void showCameraSettingsDialog(unsigned int camera, QSettings& settings, QWidget* parent=NULL);
        /** \copydoc QFExtensionCamera::useCameraSettings() */
        virtual void useCameraSettings(unsigned int camera, const QSettings& settings);
        /** \copydoc QFExtensionCamera::getImageWidth() */
        virtual int getImageWidth(unsigned int camera);
        /** \copydoc QFExtensionCamera::getImageHeight() */
        virtual int getImageHeight(unsigned int camera);
				/** \copydoc QFExtensionCamera::getCameraNme() */
				virtual QString getCameraName(unsigned int camera);
        /** \copydoc QFExtensionCamera::getCameraSensorName() */
        virtual QString getCameraSensorName(unsigned int camera);
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
        virtual void getAcquisitionDescription(unsigned int camera, QList<QFExtensionCamera::AcquititonFileDescription>* files, QMap<QString, QVariant>* parameters);
        /** \copydoc QFExtensionCamera::getAcquisitionPreview() */
        virtual bool getAcquisitionPreview(unsigned int camera, uint32_t* data);
        /** \copydoc QFExtensionCamera::getAcquisitionProgress() */
        virtual int getAcquisitionProgress(unsigned int camera);
        /** \copydoc QFExtensionCamera::getPixelWidth() */
        virtual double getPixelWidth(unsigned int camera);
        /** \copydoc QFExtensionCamera::getPixelHeight() */
        virtual double getPixelHeight(unsigned int camera);


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
        QString& findGroupByType(const QString &t, const unsigned int camera);
        struct cameraSettings* cameraSetting;
        QString bitfileMaster;
        QString bitfileSlave;
        QString autoflashbitfileMaster;
        QString autoflashbitfileSlave;
        bool autoflash;
        int retries;
        int retryDelay;
				
	public:
        bool reconfigure(unsigned int camera, const QSettings& settings, unsigned int set);
        bool reconfigure(unsigned int camera, const QSettings& settings, const QString& setName);
        void reconfigure2(unsigned int camera, const QSettings& settings, const QString& postfix);

	protected slots:
		void logger_txt(QString message){log_text(LOG_PREFIX+message);}
		void logger_wrn(QString message){log_warning(LOG_PREFIX+message);}
		void logger_err(QString message){log_error(LOG_PREFIX+message);}
        void programFPGA();

};

#endif // QFEXTENSIONCAMERA_H
