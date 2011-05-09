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
    .
    So usually you will use a camera in this manner for preview:
      -# First load settings from some location (file, registry, ...) and then use them in the camera by caling useCameraSettings().
      -# Then take single images using acquire()
    .
    And in this manner for timed acquisitions:
      -# First load settings from some location (file, registry, ...)
      -# call prepareAcquisition()
      -# call startAcquisition()
      -# loop until isAcquisitionRunning() returns \c false. During the loop the application MAY request
         preview images using getAcquisitionPreview() (if this returns \c false no previews will be
         shown)
      -# After the acquisition ended a call to getAcquisitionDescription() should return information about the last
         acquisition.
    .
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
         /*! \brief set camera settings from the specified QSettings object for subsequent acquire()
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
         /** \brief connect to the device/activate it */
         virtual bool connectDevice(unsigned int camera)=0;
         /** \brief connect to the device/activate it */
         virtual void disconnectDevice(unsigned int camera)=0;
         /** \brief returns the acquisition time (time the camera is open) in seconds */
         virtual double getExposureTime(unsigned int camera)=0;
         /** \brief set QFPluginLogServices to use (or \c NULL) for message/error logging */
         virtual void setLogging(QFPluginLogService* logService)=0;

         /*! \brief set camera settings from the specified QSettings object for a subsequent startAcquisition()
             \param[in] camera the camera the settings widget should apply to
             \param[in] settings the acquisition settings to use (may be created using showCameraSettingsDialog() )
             \param[in] filenamePrefix the acquisition result is stored in file(s) based on this baseFilename
          */
         virtual bool prepareAcquisition(unsigned int camera, const QSettings& settings, QString filenamePrefix=QString(""))=0;

         /*! \brief start an acquisition

             \param camera start an acquisition for this camera
             \retunr \c true on success
          */
         virtual bool startAcquisition(unsigned int camera)=0;
         /*! \brief cancel an acquisition started with startAcquisition()

             \param camera start an acquisition for this camera
          */
         virtual void cancelAcquisition(unsigned int camera)=0;
         /*! \brief check whether an acquisition started with startAcquisition() is running

             \param camera acquisition for this camera
             \param[out] percentageDone may be used (if \c !=NULL ) to return the percentage [0..100] of
                                        acquisition completion
         */
         virtual bool isAcquisitionRunning(unsigned int camera, double* percentageDone=NULL)=0;
         /*! \brief after completion of acquisition started with startAcquisition() this returns information about the acquistion

             \param camera acquisition for this camera
             \param[out] files list of files generated by the acquisition
             \param[out] parameters describing the acquisition and the files
          */
         virtual void getAcquisitionDescription(unsigned int camera, QStringList* files, QMap<QString, QVariant>* parameters)=0;
         /*! \brief read an intermediate preview image from the currently running acquisition

             \param camera which camera to use for the acquisition
             \param[out] data points to a memory array of getImageWidth()*getImageHeight() items
                         which will be filled with the newly acquired image
             \param[out] timestamp a timestamp for the acquired image in \a data
             \return \c true on success, or \c false on error or if the camera driver does not provide previews
         */
         virtual bool getAcquisitionPreview(unsigned int camera, uint32_t* data)=0;
         /*! \brief return the progress (0..100) of the current acquisition

             \param camera which camera to use for the acquisition
             \return the progress as an integer between 0 and 100
         */
         virtual int getAcquisitionProgress(unsigned int camera)=0;


};

Q_DECLARE_INTERFACE( QFExtensionCamera,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionCamera/1.0")

#endif // QFEXTENSIONCAMERA_H
