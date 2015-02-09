/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



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
    
        \note <b>Your implementation has to be thread-safe!!! So it can be called simultaneously from multiple threads, even when the invocations use shared data, because all references to the shared data are serialized</b>


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

        /** \brief stage informations */
        struct StageInfo {
                StageInfo() {
                    positionUnit=QLatin1String("\xB5m");
                    speedUnit=QLatin1String("\xB5m/s");
                    minPos=-1e37;
                    maxPos=1e37;
                    minSpeed=0;
                    maxSpeed=1e37;
                    resolution=1e-3;
                }

            QString positionUnit;
            QString speedUnit;
            double minPos;
            double maxPos;
            double minSpeed;
            double maxSpeed;
            double resolution;
        };


        /** Default destructor */
        virtual ~QFExtensionLinearStage() {};


        /** \brief return the number of axes managed by this extension */
        virtual unsigned int getAxisCount()=0;


        /*! \brief displays a modal dialog which allows the user to set the configuration options
                   of the controled axes.

            The options are stored internally and written/read using QFExtension::readSettings() and
            QFExtension::writeSettings().

            \param[in] axis the axis for which to display the dialog
            \param[in] parent parent widget for the returned QWidget
         */
         virtual void showSettingsDialog(unsigned int axis, QWidget* parent=NULL)=0;
         /** \brief returns \c true if the axis is connected */
         virtual bool isConnected(unsigned int axis)=0;
         /** \brief connect to the device/activate it */
         virtual void connectDevice(unsigned int axis)=0;
         /** \brief connect to the device/activate it */
         virtual void disconnectDevice(unsigned int axis)=0;
         /** \brief set QFPluginLogServices to use (or \c NULL) for message/error logging */
         virtual void setLogging(QFPluginLogService* logService)=0;

         /*! \brief enable/disable joystick control for the specified axis, \a maxVelocity is the maximum velocity for movements in microns/sec

             \param axis the axis for which to activate the joystick
             \param enabled whether to enable or disable the joystick
             \param maxVelocity the maximum velocity reachable with the joystick set to full in micron/second (if applicable!)

             \note If this is called with \c enabled=true multiple times, only the maxVelocity will be updated.
          */
         virtual void setJoystickActive(unsigned int axis, bool enabled, double maxVelocity=100000)=0;
         /** \brief returns \c true if the manual joystick control for the specified axis is activated */
         virtual bool isJoystickActive(unsigned int axis)=0;

         /** \brief return the current state of a given axis */
         virtual AxisState getAxisState(unsigned int axis)=0;

         /** \brief emergency stop an axis */
         virtual void stop(unsigned int axis)=0;

         /** \brief return the absolute position of an axis in microns */
         virtual double getPosition(unsigned int axis)=0;

         /** \brief return the current speed of an axis in microns/sec */
         virtual double getSpeed(unsigned int axis)=0;

         /*! \brief move one axis to a given absolute position in microns

             Immediately after a call to this function getAxisState() should return Moving until
             the destination is reached. If this function blocks until the destinaion is reached
             getAxisPosition() may immediately return Ready.
         */
         virtual void move(unsigned int axis, double newPosition)=0;

        /*! \brief returns the human readable name of the given stage axis */
        virtual QString getStageName(unsigned int axis) const=0;

        /*! \brief returns some information of the given stage axis */
        virtual StageInfo getStageInfo(unsigned int axis) const=0;
};


Q_DECLARE_INTERFACE( QFExtensionLinearStage,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionLinearStage/1.0")

#endif // QFEXTENSIONLINEARSTAGE_H
