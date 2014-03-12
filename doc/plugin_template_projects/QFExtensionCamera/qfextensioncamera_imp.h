#ifndef HEADER_H
#define HEADER_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include "../interfaces/qfextensioncamera.h"

/*!
    \defgroup qf3ext_GROUPNAME QFExtensionCamera implementation
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtensionCamera implementation
    \ingroup qf3ext_GROUPNAME
 */
class QFExtensionCameraImplementation : public QObject, public QFExtensionBase, public QFExtensionCamera {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFExtensionCamera)
    public:
        /** Default constructor */
        QFExtensionCameraImplementation(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFExtensionCameraImplementation();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("target_id"); };
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr(/* human readable name for plugin */); };
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr(/* human-readable description for plugin */); };
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr(/* author name */); };
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr(/* copyright information */); };
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr(/* your homepage */); };
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/target_id/qfextensioncamera_imp.png"); };
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };
        /** \copydoc QFExtension::deinit() */
        virtual void deinit();

    protected:
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
        virtual unsigned int getCameraCount() const;
        /** \copydoc QFExtensionCamera::showCameraSettingsWidget() */
        virtual void showCameraSettingsDialog(unsigned int camera, QSettings& settings, QWidget* parent=NULL);
        /** \copydoc QFExtensionCamera::useCameraSettings() */
        virtual void useCameraSettings(unsigned int camera, const QSettings& settings);
        /** \copydoc QFExtensionCamera::getCameraImageWidth() */
        virtual int getCameraImageWidth(unsigned int camera);
        /** \copydoc QFExtensionCamera::getCameraImageHeight() */
        virtual int getCameraImageHeight(unsigned int camera);
        /** \copydoc QFExtensionCamera::getPixelWidth() */
        virtual double getCameraPixelWidth(unsigned int camera);
        /** \copydoc QFExtensionCamera::getPixelHeight() */
        virtual double getCameraPixelHeight(unsigned int camera);
        /** \copydoc QFExtensionCameragetCameraName::() */
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

        /** \copydoc QFExtensionCamera::isCameraSettingChangable() */
        virtual bool isCameraSettingChangable(QFExtensionCamera::CameraSetting which) const;
        /** \copydoc QFExtensionCamera::changeCameraSetting() */
        virtual void changeCameraSetting(QSettings& settings, QFExtensionCamera::CameraSetting which, QVariant value);
        /** \copydoc QFExtensionCamera::getCameraSetting() */
        virtual QVariant getCameraSetting(QSettings& settings, QFExtensionCamera::CameraSetting which) const ;
        /** \copydoc QFExtensionCamera::getCameraSetting() */
        virtual QVariant getCameraCurrentSetting(int camera, CameraSetting which) const ;

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

    protected:
        QFPluginLogService* logService;

};

#endif // HEADER_H
