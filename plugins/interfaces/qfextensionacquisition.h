/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
	
	last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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

#ifndef QFEXTENSIONACQUISITION_H
#define QFEXTENSIONACQUISITION_H



#include "qfplugin.h"
#include <QWidget>
#include <QSettings>
#include "qfpluginservices.h"
#include <stdint.h>



/*! \brief QuickFit QFExtension to control general acquisition devices
    \ingroup qf3extensionplugins

 */
class QFExtensionAcquisition {
    public:
        virtual ~QFExtensionAcquisition() {}

        /** \brief return the number of cameras managed by this extension */
        virtual unsigned int getAcquisitionDeviceCount()=0;


        /*! \brief displays a modal dialog which allows the user to set the properties of a camera

            The initial settings are stored in the settings attribute. When the user clicks "OK" (or any
            equivalent action) the new settings will also be stored in this structure. See the section on
            \link QFExtensionAcquisition_settings Camera Settings \endlink for details on the \a settings argument.

            \param[in] camera the camera the settings widget should apply to
            \param[in,out] settings initially this contains the current settings which should be displayed in the
                                    dialog. After a successfull call this the new settings will be returned in this
                                    argument.
            \param[in] parent parent widget for the returned QWidget
         */
         virtual void showAcquisitionDeviceSettingsDialog(unsigned int device, QSettings& settings, QWidget* parent=NULL)=0;
         /** \brief returns \c true if the device is connected */
         virtual bool isAcquisitionDeviceConnected(unsigned int device)=0;
         /** \brief connect to the device/activate it */
         virtual bool connectAcquisitionDevice(unsigned int device)=0;
         /** \brief connect to the device/activate it */
         virtual void disconnectAcquisitionDevice(unsigned int device)=0;
         /** \brief set QFPluginLogServices to use (or \c NULL) for message/error logging */
         virtual void setLogging(QFPluginLogService* logService)=0;

         /*! \brief set camera settings from the specified QSettings object for a subsequent startAcquisition()
             \param[in] camera the camera the settings widget should apply to
             \param[in] settings the acquisition settings to use (may be created using showCameraSettingsDialog() )
             \param[in] filenamePrefix the acquisition result is stored in file(s) based on this baseFilename
          */
         virtual bool prepareAcquisition(unsigned int device, const QSettings& settings, QString filenamePrefix=QString(""))=0;

         /*! \brief start an acquisition

             \param camera start an acquisition for this camera
             \retunr \c true on success
          */
         virtual bool startAcquisition(unsigned int device)=0;
         /*! \brief cancel an acquisition started with startAcquisition()

             \param camera start an acquisition for this camera
          */
         virtual void cancelAcquisition(unsigned int device)=0;
         /*! \brief check whether an acquisition started with startAcquisition() is running

             \param camera acquisition for this camera
             \param[out] percentageDone may be used (if \c !=NULL ) to return the percentage [0..100] of
                                        acquisition completion
         */
         virtual bool isAcquisitionRunning(unsigned int device, double* percentageDone=NULL)=0;


         /** \brief used by getAcquisitionDescription() to return a description of the output files*/
         struct AcquititonFileDescription {
            /** \brief name of the output file */
            QString name;
            /** \brief type of the output file (e.g. RAW16, RAW32, TIFF8, TIFF16, autocorrelations, PNG, JPEG, ...) */
            QString type;
            /** \brief a description of the file contents (human readable) */
            QString description;
         };

         /*! \brief after completion of acquisition started with startAcquisition() this returns information about the acquistion

             \param camera acquisition for this camera
             \param[out] files list of files generated by the acquisition (for details: see AcquititonFileDescription )
             \param[out] parameters describing the acquisition and the files

             For \a parameters there are some default key values that you should use to guarantee a certain degree of
             standardization, as these may be used by other plugins/applications. Note that none of these key/value pairs
             is in any way required and you can create any other key/value pairs, you like to describe your acquisition)!
             .
          */
         virtual void getAcquisitionDescription(unsigned int device, QList<QFExtensionAcquisition::AcquititonFileDescription>* files, QMap<QString, QVariant>* parameters)=0;
         /*! \brief return the progress (0..100) of the current acquisition

             \param camera which camera to use for the acquisition
             \return the progress as an integer between 0 and 100
         */
         virtual int getAcquisitionProgress(unsigned int device)=0;



};

Q_DECLARE_INTERFACE( QFExtensionAcquisition,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionAcquisition/1.0")

#endif // QFEXTENSIONACQUISITION_H
