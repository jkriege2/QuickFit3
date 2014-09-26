/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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



#ifndef QFEXTENSIONMEASUREMENTDEVICE_H
#define QFEXTENSIONMEASUREMENTDEVICE_H

#include <QVariant>
#include <QWidget>
#include <QString>
#include "qfpluginservices.h"

/*! \brief QuickFit QFExtension to read arbitrary measured values from a device
    \ingroup qf3extensionplugins

    \note <b>Your implementation has to be thread-safe!!! So it can be called simultaneously from multiple threads, even when the invocations use shared data, because all references to the shared data are serialized</b>


 */
class QFExtensionMeasurementAndControlDevice {
    public:


        /** Default destructor */
        virtual ~QFExtensionMeasurementAndControlDevice() {}


        /** \brief return the number of measurement devices */
        virtual unsigned int getMeasurementDeviceCount() =0;


        /*! \brief displays a modal dialog which allows the user to set the configuration options

            The options are stored internally and written/read using QFExtension::readSettings() and
            QFExtension::writeSettings().

            \param[in] measuremenDevice the measuremenDevice for which to display the dialog
            \param[in] parent parent widget for the returned QWidget
         */
         virtual void showMeasurementDeviceSettingsDialog(unsigned int measuremenDevice, QWidget* parent=NULL)=0;
         /** \brief returns \c true if the measuremenDevice is connected */
         virtual bool isMeasurementDeviceConnected(unsigned int measuremenDevice) =0;
         /** \brief connect to the device/activate it */
         virtual void connectMeasurementDevice(unsigned int measuremenDevice)=0;
         /** \brief disconnect from the device/activate it */
         virtual void disconnectMeasurementDevice(unsigned int measuremenDevice)=0;
         /** \brief set QFPluginLogServices to use (or \c NULL) for message/error logging */
         virtual void setMeasurementDeviceLogging(QFPluginLogService* logService)=0;


        /** \brief return the number of measured values in a measurement devices */
        virtual unsigned int getMeasurementDeviceValueCount(unsigned int measuremenDevice)=0;
        /** \brief returns the name (or given label) of a measurement devices */
        virtual QString getMeasurementDeviceName(unsigned int measuremenDevice) =0;

        /** \brief return a measured value from a specified measurement devices */
        virtual QVariant getMeasurementDeviceValue(unsigned int measuremenDevice, unsigned int value) =0;

        /** \brief return a name for the specified measuremen devices (human-readable)  */
        virtual QString getMeasurementDeviceValueName(unsigned int measuremenDevice, unsigned int value) =0;

        /** \brief return a short name for the specified measuremen devices (for config-files etc. ... i.e. no space etc.)  */
        virtual QString getMeasurementDeviceValueShortName(unsigned int measuremenDevice, unsigned int value) =0;

        /** \brief returns \c true, if the value can be changed, using setMeasurementDeviceValue()  */
        virtual bool isMeasurementDeviceValueEditable(unsigned int measuremenDevice, unsigned int value) =0;

        /** \brief set a value in a specified measurement devices. This method should block, until the value is set/transmitted */
        virtual void setMeasurementDeviceValue(unsigned int measuremenDevice, unsigned int value, const QVariant& data)=0;

        /** \brief returns the datatype of a measurement device editable value */
        virtual QVariant::Type getMeasurementDeviceEditableValueType(unsigned int measuremenDevice, unsigned int value) =0;

        enum WidgetTypes {
            mdDoubleEdit=0,
            mdIntEdit,
            mdCheckBox,
            mdIntCombobBox,
            mdDoubleSlider,
            mdIntSlider,
            mdLabel,
            mdLineEdit,


            mdDefault=mdDoubleEdit
        };

        /** \brief returns the widget to be used for a measurement device' editable's value value */
        virtual WidgetTypes getMeasurementDeviceValueWidget(unsigned int measuremenDevice, unsigned int value, QStringList* comboboxEntries=NULL) =0;

        /** \brief returns the range of a measurement device editable value */
        virtual void getMeasurementDeviceEditableValueRange(unsigned int measuremenDevice, unsigned int value, double& minimum, double& maximum) =0;

};


Q_DECLARE_INTERFACE( QFExtensionMeasurementAndControlDevice,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionMeasurementAndControlDevice/1.0")



#endif // QFEXTENSIONMEASUREMENTDEVICE_H
