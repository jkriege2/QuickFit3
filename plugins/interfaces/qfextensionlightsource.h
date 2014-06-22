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



#ifndef QFEXTENSIONLIGHTSOURCE_H
#define QFEXTENSIONLIGHTSOURCE_H


#include <QtPlugin>
#include <QWidget>
#include <QSettings>
#include "qfpluginservices.h"
#include <stdint.h>



/*! \brief QuickFit QFExtension to control light sources and comparable devices
    \ingroup qf3extensionplugins


    This class is an abstraction for lightsources of different types:
      - lasers
      - lights/lamps
      - AOTFs
    .

    Each light source has (possibly) several lines (number is returned by  getLightSourceLineCount(), getLightSourceLineDescription() returns a description
    of the given line). Each line may be enabled or disabled using  setLightSourceLineEnabled(). Also it is possible to set the power of every line using
    setLightSourcePower() (out of the range, returned by getLightSourceLinePowerRange() ). The unit in which the power is set, is returned by getLightSourceLinePowerUnit().
    
        \note <b>Your implementation has to be thread-safe!!! So it can be called simultaneously from multiple threads, even when the invocations use shared data, because all references to the shared data are serialized</b>

 */
class QFExtensionLightSource {
    public:
        virtual ~QFExtensionLightSource() {}

        /** \brief returns the number of light sources controlable by this plugin */
        virtual unsigned int  getLightSourceCount()  =0;

        /** \brief connect to a light source controller/driver/.... */
        virtual void lightSourceConnect(unsigned int lightSource)=0;

        /** \brief disconnect from a light source controller/driver/.... */
        virtual void lightSourceDisonnect(unsigned int lightSource)=0;

        /** \brief set QFPluginLogServices to use (or \c NULL) for message/error logging */
        virtual void setLightSourceLogging(QFPluginLogService* logService)=0;

        /** \brief returns \c true if the device is connected */
        virtual bool isLightSourceConnected(unsigned int lightSource) =0;

        /** \brief return the number of wavelength lines selctable by the light source */
        virtual unsigned int getLightSourceLineCount(unsigned int lightSource) =0;

        /** \brief get a description for the given line */
        virtual QString getLightSourceLineDescription(unsigned int lightSource, unsigned int wavelengthLine) =0;

        /** \brief get a description for the given line */
        virtual void getLightSourceLinePowerRange(unsigned int lightSource, unsigned int wavelengthLine, double& minimum, double& maximum) =0;

        /** \brief get a unit for the power setting of the given line */
        virtual QString getLightSourceLinePowerUnit(unsigned int lightSource, unsigned int wavelengthLine) =0;

        /** \brief set wavelength power in the given light source */
        virtual void setLightSourcePower(unsigned int lightSource, unsigned int wavelengthLine, double power) =0;

        /** \brief return the currently selected light source set power */
        virtual double getLightSourceCurrentSetPower(unsigned int lightSource, unsigned int wavelengthLine) =0;

        /** \brief return the currently selected light source measured power */
        virtual double getLightSourceCurrentMeasuredPower(unsigned int lightSource, unsigned int wavelengthLine) =0;

        /** \brief en-/disable the wavelength line in the given light source */
        virtual void setLightSourceLineEnabled(unsigned int lightSource, unsigned int wavelengthLine, bool enabled) =0;

        /** \brief return whether the given line is en- or disabled the selected light source line  */
        virtual bool getLightSourceLineEnabled(unsigned int lightSource, unsigned int wavelengthLine) =0;

        /** \brief return \c true if the last command, sent to the given light source was executet, i.e. the light source has settled to its new position */
        virtual bool isLastLightSourceActionFinished(unsigned int lightSource)=0;

        /** \brief return a human-readable description for the given light source */
        virtual QString getLightSourceDescription(unsigned int lightSource) =0;

        /** \brief return a human-readable short name for the given light source */
        virtual QString getLightSourceShortName(unsigned int lightSource) =0;

        /*! \brief displays a modal dialog which allows the user to set the configuration options
                   of the controled light source.

            The options are stored internally and written/read using QFExtension::readSettings() and
            QFExtension::writeSettings().

            \param[in] lightSource the light source for which to display the dialog
            \param[in] parent parent widget for the returned QWidget
         */
         virtual void showLightSourceSettingsDialog(unsigned int lightSource, QWidget* parent=NULL)=0;
};



Q_DECLARE_INTERFACE( QFExtensionLightSource,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionLightSource/1.0")

#endif // QFEXTENSIONLIGHTSOURCE_H
