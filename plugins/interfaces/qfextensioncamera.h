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
      -# Then take single images using acquire(), This function will always use those settings given in the last call of useCameraSettings().
         Do not call functions for timed acquisitions in between useCameraSettings() and acquire(), as then the object is not guaranteed
         do use the correct settings!
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
    There is also the possibility to alter the acquisition settings in a controlled manner. This may be done,
    using the functions isCameraSettingChangable() and changeCameraSetting() which are applied to the QSettings
    object, used to configure in prepareAcquisition().
 */
class QFExtensionCamera {
    public:
        virtual ~QFExtensionCamera() {}

        /** \brief return the number of cameras managed by this extension */
        virtual unsigned int getCameraCount() const=0;


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
         virtual int getCameraImageWidth(unsigned int camera)=0;
         /** \brief return the height of images */
         virtual int getCameraImageHeight(unsigned int camera)=0;
         /** \brief return width of a sensor pixel in microns */
         virtual double getCameraPixelWidth(unsigned int camera)=0;
         /** \brief return hieght of a sensor pixel in microns */
         virtual double getCameraPixelHeight(unsigned int camera)=0;
         /** \brief returns the name of the camera */
         virtual QString getCameraName(unsigned int camera)=0;
         /** \brief returns the name of the camera sensor */
         virtual QString getCameraSensorName(unsigned int camera)=0;
         /** \brief returns \c true if the device is connected */
         virtual bool isCameraConnected(unsigned int camera)=0;
         /*! \brief acquire single frame

             \param camera which camera to use for the acquisition
             \param[out] data points to a memory array of getImageWidth()*getImageHeight() items
                         which will be filled with the newly acquired image
             \param[out] timestamp a timestamp for the acquired image in \a data
             \param[out] parameters this map is filled with the acquisition parameters, if \c !=NULL
             \return \c true on success
          */
         virtual bool acquireOnCamera(unsigned int camera, uint32_t* data, uint64_t* timestamp=NULL, QMap<QString, QVariant>* parameters=NULL)=0;
         /** \brief connect to the device/activate it */
         virtual bool connectCameraDevice(unsigned int camera)=0;
         /** \brief connect to the device/activate it */
         virtual void disconnectCameraDevice(unsigned int camera)=0;
         /** \brief returns the acquisition time (time the camera is open) in seconds */
         virtual double getCameraExposureTime(unsigned int camera)=0;
         /** \brief set QFPluginLogServices to use (or \c NULL) for message/error logging */
         virtual void setCameraLogging(QFPluginLogService* logService)=0;

         /*! \brief set camera settings from the specified QSettings object for a subsequent startAcquisition()
             \param[in] camera the camera the settings widget should apply to
             \param[in] settings the acquisition settings to use (may be created using showCameraSettingsDialog() )
             \param[in] filenamePrefix the acquisition result is stored in file(s) based on this baseFilename
          */
         virtual bool prepareCameraAcquisition(unsigned int camera, const QSettings& settings, QString filenamePrefix=QString(""))=0;

         /*! \brief start an acquisition

             \param camera start an acquisition for this camera
             \retunr \c true on success
          */
         virtual bool startCameraAcquisition(unsigned int camera)=0;
         /*! \brief cancel an acquisition started with startAcquisition()

             \param camera start an acquisition for this camera
          */
         virtual void cancelCameraAcquisition(unsigned int camera)=0;
         /*! \brief check whether an acquisition started with startAcquisition() is running

             \param camera acquisition for this camera
             \param[out] percentageDone may be used (if \c !=NULL ) to return the percentage [0..100] of
                                        acquisition completion
         */
         virtual bool isCameraAcquisitionRunning(unsigned int camera, double* percentageDone=NULL)=0;


         /** \brief used by getAcquisitionDescription() to return a description of the output files*/
         struct CameraAcquititonFileDescription {
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
               - \c sequence_length number of images in the sequence
               - \c image_width / \c image_height width/height of the image in pixel
               - \c pixel_width / \c pixel_height width/height of each pixel in microns (<b>with magnification optics, and caring for binning!</b>)
               - \c camera_pixel_width / \c camera_pixel_height width/height of each pixel in microns (<b>without magnification optic but caring for binning!</b>)
               - \c start_time QTime/QDate/QDateTime secribing when the acquisition started
               - \c duration duration of the acquisition in seconds (\c double )
               - \c exposure_time exposure time in seconds (double)
               - \c frame_time (>= \c exposure_time ) the time between two consecutive exposures
               - \c pixel_units (\c photons | \c arbitrary | ... ) in which units are the pixel grey values measured (set to \c photons for photon counting devices!)
               - \c shutter_state (\c open | \c closed | \c auto | ... ) shutter settings used for the acquisition
               - \c sensor_temperature (double) temperature [°C] of the sensor during acquisition
               - \c stack_positions (QList<double>) position (in microns) where images have been taken in image stacks
               - \c sensor_temperature_series / \c sensor_temperature_series_times (QList<double>) temperature [°C] of the sensor
                 during acquisition at different times (times stored in sensor_temperature_series_times in seconds).
               - \c camera_model model name of the camera
               - \c camera_manufacturer manufacturer of the camera
               - \c camera_serial_number serial number of the camera
               - \c camera_controler_card frame grabber card
               - \c sensor_model name of the sensor head
               - \c quantum_efficiency / \c quantum_efficiency_wavelength (QList<double>) quantum efficiency [0..1] of the sensor
                 given at several wavelengths [nanometers]
               - \c filter_name description of a possible filter
               - \c filter_center_wavelength center wavelength of filter [nanometers]
               - \c filter_width  width of the filter [nanometers]
               - \c illumination_filter_name description of a possible illumination filter
               - \c illumination_filter_center_wavelength center wavelength of illumination filter [nanometers]
               - \c illumination_filter_width  width of the illumination filter [nanometers]
               - \c beamsplitter_name description of a possible beamsplitter
               - \c beamsplitter_center_wavelength cut wavelength of beamsplitter [nanometers]
               - \c roi_xstart \c roi_xend \c roi_ystart \c roi_yend size of the region of interest
               - \c emgain EMGain factor on EMCCD cameras (\c double )
             .
          */
         virtual void getCameraAcquisitionDescription(unsigned int camera, QList<QFExtensionCamera::CameraAcquititonFileDescription>* files, QMap<QString, QVariant>* parameters)=0;
         /*! \brief read an intermediate preview image from the currently running acquisition

             \param camera which camera to use for the acquisition
             \param[out] data points to a memory array of getImageWidth()*getImageHeight() items
                         which will be filled with the newly acquired image
             \param[out] timestamp a timestamp for the acquired image in \a data
             \return \c true on success, or \c false on error or if the camera driver does not provide previews
         */
         virtual bool getCameraAcquisitionPreview(unsigned int camera, uint32_t* data)=0;
         /*! \brief return the progress (0..100) of the current acquisition

             \param camera which camera to use for the acquisition
             \return the progress as an integer between 0 and 100
         */
         virtual int getCameraAcquisitionProgress(unsigned int camera)=0;

         enum CameraSetting {
             CamSetExposureTime=0,
             CamSetNumberFrames=1,
             CamSetGain=2,
             CamSetFrametime=3,
             CamSetEMGAIN=4,
             CamSetHorizontalBinning=5,
             CamSetVerticalBinning=6,
             CamSetTemporalBinning=7,
             CamSetMaxParam=CamSetTemporalBinning,

             CamSetUnknown=CamSetMaxParam+1
         };

         /** \brief returns \c true if the given CameraSetting is changable by changeCameraSetting() */
         virtual bool isCameraSettingChangable(CameraSetting which) const =0;
         /*! \brief change the given CameraSetting in the given QSettings object

             \note <b>this will change the contents of your QSettings object, so possibly this changed version is also written back to the harddisk!!!</b>
          */
         virtual void changeCameraSetting(QSettings& settings, CameraSetting which, QVariant value) =0;
         /** \brief extract the given CameraSetting from the given QSettings object */
         virtual QVariant getCameraSetting(QSettings& settings, CameraSetting which) const =0;
         /** \brief extract the given CameraSetting from the given camera */
         virtual QVariant getCameraCurrentSetting(int camera, CameraSetting which) const =0;


};

Q_DECLARE_INTERFACE( QFExtensionCamera,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionCamera/1.1")

#endif // QFEXTENSIONCAMERA_H
