#ifndef QFEXTENSIONCAMERA_H
#define QFEXTENSIONCAMERA_H

#include <QtPlugin>
#include <QWidget>
#include <QSettings>


/*! \brief QuickFit QFExtension to control black \& white cameras
    \ingroup qf3extensionplugins
 */
class QFExtensionCamera {
    public:
        virtual ~QFExtensionCamera() {}

        /** \brief return the number of cameras managed by this extension */
        virtual unsigned int getCameraCount()=0;


        /*! \brief return a QWidget which contains controls to set the device parameters

            This QWidget may e.g. be placed as a QDockWidget on the application screen.
            It should contain widgets to set parameters like exposure time, shutter,
            color channel, ...

            \param camera the camera the settings widget should apply to
            \param parent parent widget for the returned QWidget
         */
         virtual QWidget* createSettingsWidget(unsigned int camera, QWidget* parent=NULL)=0;
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
         virtual double getAcquisitionTime(unsigned int camera)=0;

};

Q_DECLARE_INTERFACE( QFExtensionCamera,
                     "www.dkfz.de.b040.quickfit3.extensions.QFExtensionCamera/1.0")

#endif // QFEXTENSIONCAMERA_H
