#ifndef QFEXTENSIONLINEARSTAGE_H
#define QFEXTENSIONLINEARSTAGE_H

#include <QtPlugin>
#include <QWidget>
#include <QSettings>
#include "qfpluginservices.h"
#include <stdint.h>



/*! \brief QuickFit QFExtension to control 1 axis (linear) motion stages
    \ingroup qf3extensionplugins

    Each of these plugins may control one stage with \f$ n \f$ axes. There are functions to move
    each axis separately and also read each axis's state. Configurations are edited by the user in a
    a dialog displayed with showSettingsDialog(). They are stored in calls to QFExtension::writeSettings()
    and read using QFExtension::readSettings().

    As this is useful some features common to many stages are implemented already in this class:
      - one may activate/deactivate a joystick for manual control: setJoystickActive()
    .

    There is one speciality about the connectDevice(), disconnectDevice() methods: For many stages there
    is only one controller which connects to several axes, so whenever connectDevice() for one axis is called
    the QFExtension using these stages should ask all other stages whether they are also connected, so you don't
    have to connect to each axis via a separate click. The same applies to the setJoystickActive() function.

 */
class QFExtensionLinearStage {
    public:
        /** \brief possible states of an axis */
        enum AxisState {
            Disconnected=0,
            Ready=1,
            Moving=2,
            Error=3
        };


        /** Default destructor */
        virtual ~QFExtensionLinearStage() {};


        /** \brief return the number of axes managed by this extension */
        virtual unsigned int getAxisCount()=0;


        /*! \brief displays a modal dialog which allows the user to set the configuration options
                   of the controled axes.

            The options are stored internally and written/read using QFExtension::readSettings() and
            QFExtension::writeSettings().
            \param[in] parent parent widget for the returned QWidget
         */
         virtual void showSettingsDialog(QWidget* parent=NULL)=0;
         /** \brief returns \c true if the axis is connected */
         virtual bool isConnected(unsigned int axis)=0;
         /** \brief connect to the device/activate it */
         virtual void connectDevice(unsigned int axis)=0;
         /** \brief connect to the device/activate it */
         virtual void disconnectDevice(unsigned int axis)=0;
         /** \brief set QFPluginLogServices to use (or \c NULL) for message/error logging */
         virtual void setLogging(QFPluginLogService* logService)=0;

         /** \brief enable/disable joystick control for the specified axis, \a maxVelocity is the maximum velocity for movements in microns/sec */
         virtual void setJoystickActive(unsigned int axis, bool enabled, double maxVelocity=100000)=0;
         /** \brief returns \c true if the manual joystick control for the specified axis is activated */
         virtual bool isJoystickActive(unsigned int axis)=0;

         /** \brief return the current state of a given axis */
         virtual AxisState getAxisState(unsigned int axis)=0;

         /** \brief emergency stop an axis */
         virtual void stop(unsigned int axis)=0;

         /** \brief return the absolute position of an axis in microns */
         virtual double getPosition(unsigned int axis)=0;

         /*! \brief move one axis to a given absolute position in microns

             Immediately after a call to this function getAxisState() should return Moving until
             the destination is reached. If this function blocks until the destinaion is reached
             getAxisPosition() may immediately return Ready.
         */
         virtual void move(unsigned int axis, double newPosition)=0;

};


Q_DECLARE_INTERFACE( QFExtensionLinearStage,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionLinearStage/1.0")

#endif // QFEXTENSIONLINEARSTAGE_H
