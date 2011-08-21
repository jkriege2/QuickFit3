#ifndef QFEXTENSIONParameterDevice_H
#define QFEXTENSIONParameterDevice_H


#include <QtPlugin>
#include <QWidget>
#include <QSettings>
#include "qfpluginservices.h"
#include <stdint.h>
#include <QVariant>




/*! \brief QuickFit QFExtension to control any device(s) that is configured by a set of parameters (shutters, filter wheels, light sources, temp. control, ...)
    \ingroup qf3extensionplugins

    Every plugin implementing this interface may control several devices where each device is configured
    by a set of parameters (0, 1, .. getParameterDeviceParameterCount()-1 ). The parameters may have different
    types (QFExtensionParameterDevice::ParameterDeviceParameterType):
        - \b double value is a floating point number
        - \b string value is a string
        - \b integer value is an integer
        - \b integerCombo value is an integer and displayed as a QComboBox with user-selectable contents
        - \b boolean value is boolean
        - \b doubleGraph values are QList<QVariant> with \f$ 2+k+n\cdot (k+1) \f$ entries where each entry has to convert
             to a double (except the first \f$ 2+k \f$, which convert to a string). This draws a plot widget that displays
             \f$ k \f$ curves. The values in the list are in this layout: <code>[ (xlabel: string, ylabel: string), (keyEntry\\_1:string,...,keyEntry\\_k:string), (x, y\\_1, y\\_2, ..., y\\_k), (x, y\\_1, y\\_2, ..., y\\_k), ... ]</code>
    .

    Devices may be display/readonly, if getParameterDeviceParameterIsDisplay() returns \c true. Display parameters are
    updated ever getParameterDeviceDisplayInterval() milliseconds.

    As it may take the device some time to reach a new set of settings, this interface contains the method
    isParameterDeviceConnected() which is used to make sure that all parameters have reached their desired end
    value, before the controling program proceeds.

    In addition there may be parameters that should not be edited by the user all the time (e.g. connection settings).
    These may be set/altered using the showParameterDeviceSettingsDialog() method which displays a modal configuration
    dialog.

    These QFExtension s will be used together with a user-interface that allows the user to change all parameters. In
    addition this control widget displays a possibility to connect to the device and to show the configuration dialog.

    As most hardware devices need one or the other type of connection initialization (serial port, USB, ...) there are the
    methods connectParameterDevice(), disconnectParameterDevice() and isParameterDeviceConnected() which allow to connect/disconnect
    to/from a device. Note that it is possible that devices are always connected, so no call to connectParameterDevice() is
    necessary. So the using program will not expect that a device is disconnected at startup, but rather check it, using
    isParameterDeviceConnected().


 */
class QFExtensionParameterDevice {
    public:
        /** \brief parameter types */
        enum ParameterDeviceParameterType {
            sdptDouble,
            sdptString,
            sdptInteger,
            sdptIntegerCombo,
            sdptBoolean,
            sdptDoubleGraph
        };

        /** Default destructor */
        virtual ~QFExtensionParameterDevice() {};


        /** \brief return the number of devices managed by this extension */
        virtual unsigned int getParameterDeviceCount()=0;

        /** \brief return the status of a device. This should return \c false while the hardware moves to a new state */
        virtual bool isParameterDeviceReady(unsigned int device)=0;

        /** \brief return the number of parameters for a given device */
        virtual unsigned int getParameterDeviceParameterCount(unsigned int device)=0;

        /** \brief return the human-readable name of the given parameter */
        virtual QString getParameterDeviceParameterName(unsigned int device, unsigned int parameter)=0;

        /** \brief return whether the given parameter is display-only */
        virtual bool getParameterDeviceParameterIsDisplay(unsigned int device, unsigned int parameter)=0;

        /** \brief return whether the update interval for display-only parameters in milliseconds */
        virtual unsigned long getParameterDeviceDisplayInterval(unsigned int device)=0;

        /** \brief return the ID (short name) of the given parameter */
        virtual QString getParameterDeviceParameterID(unsigned int device, unsigned int parameter)=0;

        /** \brief return the type of the given parameter */
        virtual ParameterDeviceParameterType getParameterDeviceParameterType(unsigned int device, unsigned int parameter)=0;

        /** \brief return the type of the given parameter */
        virtual void getParameterDeviceParameterRange(unsigned int device, QVariant& min, QVariant& max)=0;

        /** \brief return a list of values that may appear in a QComboBox for a ParameterDeviceParameterType::sdptIntegerCombo parameter */
        virtual QStringList getParameterDeviceParameterIntNames(unsigned int device, unsigned int parameter)=0;

        /** \brief set the value of the given parameter, returns \c true on success */
        virtual bool setParameterDeviceParameter(unsigned int device, QVariant value)=0;

        /** \brief get the value of the given parameter */
        virtual QVariant getParameterDeviceParameter(unsigned int device)=0;

        /*! \brief displays a modal dialog which allows the user to set the configuration options
                   of the controled device.

            The options are stored internally and written/read using QFExtension::readSettings() and
            QFExtension::writeSettings().

            \param[in] device the device for which to display the dialog
            \param[in] parent parent widget for the returned QWidget
         */
         virtual void showParameterDeviceSettingsDialog(unsigned int device, QWidget* parent=NULL)=0;

         /** \brief returns \c true if the device is connected */
         virtual bool isParameterDeviceConnected(unsigned int device)=0;

         /** \brief connect to the device/activate it */
         virtual void connectParameterDevice(unsigned int device)=0;

         /** \brief connect to the device/activate it */
         virtual void disconnectParameterDevice(unsigned int device)=0;

         /** \brief set QFPluginLogServices to use (or \c NULL) for message/error logging */
         virtual void setParameterDeviceLogging(QFPluginLogService* logService)=0;


};


Q_DECLARE_INTERFACE( QFExtensionParameterDevice,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionParameterDevice/1.0")

#endif // QFEXTENSIONParameterDevice_H
