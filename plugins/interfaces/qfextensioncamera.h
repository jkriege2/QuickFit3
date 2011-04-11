#ifndef QFEXTENSIONCAMERA_H
#define QFEXTENSIONCAMERA_H

#include <QtPlugin>
#include <QWidget>
#include <QSettings>
#include "qfpluginservices.h"
#include <stdint.h>



/*! \brief QuickFit QFExtension to control black \& white cameras
    \ingroup qf3extensionplugins

    \section QFExtensionCamera_settings Camera settings
    Each camera may be configured (i.e. resolution, exposure time, modes ...). This class implements an interface to work with
    such configurations. Each set of configurations is stored in a  QSettings object. This interfaces defines some methods
    to work with configurations:
      - showSettingsDialog() displays a modal dialog where the user may enter settings
      - useCameraSettings() sets the camera settings from a given QSettings object

 */
class QFExtensionCamera {
    public:
        virtual ~QFExtensionCamera() {}

        /** \brief return the number of cameras managed by this extension */
        virtual unsigned int getCameraCount()=0;


        /*! \brief displays a modal dialog which allows the user to set the properties of a camera

            The initial settings are stored in the settings attribute. When the user clicks "OK" (or any
            equivalent action) the new settings will also be stored in this structure. See the section on
            \link QFExtensionCamera_settings Camera Settings \endlink for details on the \a settings argument.

            \param[in] camera the camera the settings widget should apply to
            \param[in,out] settings initially this contains the current settings which should be displayed in the
                                    dialog. After a successfull call this the new settings will be returned in this
                                    argument.
            \param[in] parent parent widget for the returned QWidget
         */
         virtual void showCameraSettingsDialog(unsigned int camera, QSettings& settings, QWidget* parent=NULL)=0;
         /*! \brief set camera settings from the specified QSettings object
            \param[in] camera the camera the settings widget should apply to
            \param[in] settings
          */
         virtual void useCameraSettings(unsigned int camera, const QSettings& settings)=0;
         /** \brief return the width of images */
         virtual int getImageWidth(unsigned int camera)=0;
         /** \brief return the height of images */
         virtual int getImageHeight(unsigned int camera)=0;
         /** \brief returns \c true if the device is connected */
         virtual bool isConnected(unsigned int camera)=0;
         /*! \brief acquire single frame

             \param camera which camera to use for the acquisition
             \param[out] data points to a memory array of getImageWidth()*getImageHeight() items
                         which will be filled with the newly acquired image
             \param[out] timestamp a timestamp for the acquired image in \a data
             \return \c true on success
          */
         virtual bool acquire(unsigned int camera, uint32_t* data, uint64_t* timestamp=NULL)=0;
         /** \brief connect to the device/sctivate it */
         virtual bool connectDevice(unsigned int camera)=0;
         /** \brief connect to the device/sctivate it */
         virtual void disconnectDevice(unsigned int camera)=0;
         /** \brief returns the acquisition time (time the camera is open) in seconds */
         virtual double getExposureTime(unsigned int camera)=0;
         /** \brief set QFPluginLogServices to use (or \c NULL) for message/error logging */
         virtual void setLogging(QFPluginLogService* logService)=0;


};

Q_DECLARE_INTERFACE( QFExtensionCamera,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionCamera/1.0")

#endif // QFEXTENSIONCAMERA_H
