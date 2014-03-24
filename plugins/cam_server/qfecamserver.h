#ifndef QFECAMSERVER_H
#define QFECAMSERVER_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include "../interfaces/qfextensioncamera.h"
#include <QTcpSocket>
#include <QHostAddress>
#include "qftools.h"
#include "qfextensionmeasurementdevice.h"
#include "qfmeasurementdeviceconfigwidget.h"
/*!
    \defgroup qf3ext_cam_server QFExtensionCamera implementation
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtensionCamera implementation
    \ingroup qf3ext_cam_server
 */
class QFECamServer : public QObject, public QFExtensionBase, public QFExtensionCamera, public QFExtensionMeasurementAndControlDevice {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFExtensionCamera QFExtensionMeasurementAndControlDevice)
    public:
        /** Default constructor */
        QFECamServer(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFECamServer();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("cam_server"); };
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("camera TCPIP server plugin"); };
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("camera driver that polls a TCPIP server for frames"); };
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan W. Krieger"); };
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2014 by Jan W. Krieger"); };
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); };
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/cam_server/qfecamserver.png"); };
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
    // QFExtensionMeasurementDevice routines
    /////////////////////////////////////////////////////////////////////////////
    public:
        /** \copydoc QFExtensionMeasurementDevice::getmeasurementDeviceCount() */
        virtual unsigned int getMeasurementDeviceCount();
        /** \copydoc QFExtensionMeasurementDevice::showmeasurementDeviceSettingsDialog() */
        virtual void showMeasurementDeviceSettingsDialog(unsigned int measurementDevice, QWidget* parent=NULL);
        /** \copydoc QFExtensionMeasurementDevice::ismeasurementDeviceConnected() */
        virtual bool isMeasurementDeviceConnected(unsigned int measurementDevice);
        /** \copydoc QFExtensionMeasurementDevice::connectmeasurementDevice() */
        virtual void connectMeasurementDevice(unsigned int measurementDevice);
        /** \copydoc QFExtensionMeasurementDevice::disconnectmeasurementDevice() */
        virtual void disconnectMeasurementDevice(unsigned int measurementDevice);
        /** \copydoc QFExtensionMeasurementDevice::getMeasurementDeviceName() */
        virtual QString getMeasurementDeviceName(unsigned int measurementDevice);
        /** \copydoc QFExtensionMeasurementDevice::setmeasurementDeviceLogging() */
        virtual void setMeasurementDeviceLogging(QFPluginLogService* logService);
        /** \copydoc QFExtensionMeasurementDevice::getmeasurementDeviceValueCount() */
        virtual unsigned int getMeasurementDeviceValueCount(unsigned int measurementDevice);
        /** \copydoc QFExtensionMeasurementDevice::getmeasurementDeviceValue() */
        virtual QVariant getMeasurementDeviceValue(unsigned int measurementDevice, unsigned int value);
        /** \copydoc QFExtensionMeasurementDevice::getmeasurementDeviceValueName() */
        virtual QString getMeasurementDeviceValueName(unsigned int measurementDevice, unsigned int value);
        /** \copydoc QFExtensionMeasurementDevice::getmeasurementDeviceValueShortName() */
        virtual QString getMeasurementDeviceValueShortName(unsigned int measurementDevice, unsigned int value);
        /** \copydoc QFExtensionMeasurementDevice::isMeasurementDeviceValueEditable() */
        virtual bool isMeasurementDeviceValueEditable(unsigned int measurementDevice, unsigned int value);
        /** \copydoc QFExtensionMeasurementDevice::getMeasurementDeviceValue() */
        virtual void setMeasurementDeviceValue(unsigned int measurementDevice, unsigned int value, const QVariant& data);
        /** \copydoc QFExtensionMeasurementDevice::getMeasurementDeviceEditableValueType() */
        virtual QVariant::Type getMeasurementDeviceEditableValueType(unsigned int measurementDevice, unsigned int value);
        /** \copydoc QFExtensionMeasurementDevice::getMeasurementDeviceValueWidget() */
        virtual WidgetTypes getMeasurementDeviceValueWidget(unsigned int measurementDevice, unsigned int value, QStringList* comboboxEntries=NULL);
        /** \copydoc QFExtensionMeasurementDevice::getMeasurementDeviceEditableValueRange() */
        virtual void getMeasurementDeviceEditableValueRange(unsigned int measurementDevice, unsigned int value, double& minimum, double& maximum);


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

        struct CAM_PARAM {
            QString description;
            QString id;
            bool editable;
            QVariant::Type type;
            QFExtensionMeasurementAndControlDevice::WidgetTypes widget;
            double range_min;
            double range_max;
        };

        struct DEVICE_CONFIG {
            QString host;
            quint16 port;
            QString camera_name;
            QString sensor_name;
            QTime lastAction;
            QElapsedTimer connected_timer;
            /** \brief width of the frames, sent by the server */
            int width;
            /** \brief height of the frames, sent by the server */
            int height;
            /** \brief bit depth of the frames, sent by the server */
            int answer_bits;
            /** \brief timeout when connecting */
            int timeout_connection;
            /** \brief timeout for instructions */
            int timeout_instruction;
            /** \brief width of a single pixel in micrometer */
            int pixel_width;
            /** \brief height of a single pixel in micrometer */
            int pixel_height;
            /** \brief current exposure time */
            double exposure;

            QByteArray last_filenameprefix;
            bool acquiring;

            QByteArray lastfiles;

            QList<CAM_PARAM> params;


            /** \brief Server object to access the device */
            QTcpSocket* server;
            QMutex* mutex;
        };

        QList<DEVICE_CONFIG> sources;

        /** \brief flush output buffers .... and discard all bytes in input buffer, if read_all=true */
        static bool flushBuffers(const DEVICE_CONFIG& device, bool read_all=true);
        /** \brief send a command to the server */
        static bool sendCommand(const DEVICE_CONFIG& device, const QByteArray& command);
        /** \brief read from server until the given end_string is encountered is encountered, if cleanresult=true, the returned string will NOT contain end_string */
        static QByteArray readString(const DEVICE_CONFIG& device, const QByteArray &end_string=QByteArray("\n"), bool* ok=NULL, bool cleanResult=true);
        /** \brief read the given number of bytes from server */
        static QByteArray readData(const DEVICE_CONFIG& device, int bytes_to_read, bool* ok=NULL);
        /** \brief wait for given string from server */
        static bool waitForString(const DEVICE_CONFIG& device, const QByteArray &end_string=QByteArray("\n"));
        /** \brief read the given number of bytes from server, if cleanresult=true, the returned string will NOT contain end_string */
        static QList<QByteArray> queryData(const DEVICE_CONFIG& device, const QByteArray& command, int line_to_read=1, const QByteArray &end_string=QByteArray("\n"), bool* ok=NULL, bool cleanResult=true);



};


#endif // QFECAMSERVER_H
