#ifndef QFEXTENSIONMEASUREMENTDEVICE_H
#define QFEXTENSIONMEASUREMENTDEVICE_H



/*! \brief QuickFit QFExtension to read arbitrary measured values from a device
    \ingroup qf3extensionplugins

    \note <b>Your implementation has to be thread-safe!!! So it can be called simultaneously from multiple threads, even when the invocations use shared data, because all references to the shared data are serialized</b>


 */
class QFExtensionMeasurementDevice {
    public:


        /** Default destructor */
        virtual ~QFExtensionMeasurementDevice() {}


        /** \brief return the number of measurement devices */
        virtual unsigned int getMeasuremenDeviceCount()=0;


        /*! \brief displays a modal dialog which allows the user to set the configuration options

            The options are stored internally and written/read using QFExtension::readSettings() and
            QFExtension::writeSettings().

            \param[in] measuremenDevice the measuremenDevice for which to display the dialog
            \param[in] parent parent widget for the returned QWidget
         */
         virtual void showMeasuremenDeviceSettingsDialog(unsigned int measuremenDevice, QWidget* parent=NULL)=0;
         /** \brief returns \c true if the measuremenDevice is connected */
         virtual bool isMeasuremenDeviceConnected(unsigned int measuremenDevice)=0;
         /** \brief connect to the device/activate it */
         virtual void connectMeasuremenDevice(unsigned int measuremenDevice)=0;
         /** \brief disconnect from the device/activate it */
         virtual void disconnectMeasuremenDevice(unsigned int measuremenDevice)=0;
         /** \brief set QFPluginLogServices to use (or \c NULL) for message/error logging */
         virtual void setMeasuremenDeviceLogging(QFPluginLogService* logService)=0;


        /** \brief return the number of measured values in a measurement devices */
        virtual unsigned int getMeasuremenDeviceValueCount(unsigned int measuremenDevice)=0;

        /** \brief return a measured value from a specified measuremen devices */
        virtual QVariant getMeasuremenDeviceValue(unsigned int measuremenDevice, unsigned int value)=0;

        /** \brief return a name for the specified measuremen devices (human-readable)  */
        virtual QString getMeasuremenDeviceValueName(unsigned int measuremenDevice, unsigned int value)=0;

        /** \brief return a short name for the specified measuremen devices (for config-files etc. ... i.e. no space etc.)  */
        virtual QString getMeasuremenDeviceValueShortName(unsigned int measuremenDevice, unsigned int value)=0;


};


Q_DECLARE_INTERFACE( QFExtensionMeasurementDevice,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionMeasurementDevice/1.0")



#endif // QFEXTENSIONMEASUREMENTDEVICE_H
