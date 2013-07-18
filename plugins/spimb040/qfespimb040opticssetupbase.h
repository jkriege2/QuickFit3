#ifndef QFESPIMB040OPTICSSETUPBASE_H
#define QFESPIMB040OPTICSSETUPBASE_H
#include <QWidget>
#include <QMap>
#include <QString>
#include <QVariant>
#include <QList>
#include "qfextension.h"
#include "qfextensioncamera.h"

typedef QList<QTriple<QIcon, QString, QString> > QFESPIMB040OpticsSetupItems;

class QFESPIMB040OpticsSetupBase: public QWidget {
        Q_OBJECT
    public:
        QFESPIMB040OpticsSetupBase(QWidget* parent=NULL);
        virtual ~QFESPIMB040OpticsSetupBase();


        enum Shutters  {
            ShutterMain,
            ShutterLaser1,
            ShutterLaser2,
            ShutterTransmission
        };

        struct measuredValues {
            QDateTime time;
            QMap<QString, QVariant> data;

            measuredValues() {
                time=QDateTime::currentDateTime();
                data.clear();
            }
        };






        /** \brief removes the lightpath widget from it's place in the dialog and returns a pointer to it. */
        virtual QWidget* takeLightpathWidget() const=0;

        /*! \brief lock access to stages: stop the thread used for stage access by this widget

            \note call this, if you want to access the stage from any other method outside this widget!!! otherwise concurrent thread accesses are possible!!!
            \note You can release the lock y calling unlockStages().
          */
        virtual void lockStages()=0;
        /** \brief unlock access to stages: restart the thread used for stage access by this widget  */
        virtual void unlockStages()=0;
        /** \brief locks access to lightpath controls (shutters, laser, ...) */
        virtual void lockLightpath()=0;
        /** \brief unlocks access to lightpath controls (shutters, laser, ...) */
        virtual void unlockLightpath()=0;

        /*! \brief return a map containing a description of the optics setup, suitable for meta-data storage

            This function saves a description of the optics setup to a QMap. This method saves:
              - lenses and filters configuration
              - current illumination states (light sources, shutters, ...), if available
              - environment (temperatures, light intensities, ...)
            .

            if \a setup_cam > -1 the setup will not contain data which do not apply to specified camera (i.e. -1 is wildcard)
         */
        virtual QMap<QString, QVariant> getSetup(int setup_cam=-1) const=0;


        /*! \brief returns a pointer to the QFExtensionCamera and ensures exclusive access to one camera  therein.

            This method first stops any preview or use of the <QFExtensionCamera, camera> pair managed by this class.
            Then it returns a pointer to QFExtensionCamera and which camera to use inside it. This object is then
            locked for further access until releaseCamera() is called.
            While the camera is locked, a second call to lockCamera() will always return \c false and NULL-pointers!

            \b Note that the camera has to be connected already! Else this function will return \c false.

            \return \c true if camera is available \c false else
            \param[in] setup_cam which cam to lock (currently either 0 or 1)
            \param[out] extension pointer to the extension representing the camera
            \param[out] ecamera pointer to the QFExtensionCamera representing the camera (basically a typecast from \a extension
            \param[out] camera which camera inside \a ecamera to use.
            \param[out] acquisitionSettingsFilename filename of the acquisition settings selected by the suer
            \param[out] previewSettingsFilename filename of the preview settings selected by the suer
        */
        virtual bool lockCamera(int setup_cam, QFExtension** extension, QFExtensionCamera** ecamera, int* camera, QString* previewSettingsFilename)=0;

        /*! \brief release a locked camera n, for more details see lockCamera() */
        virtual void releaseCamera(int setup_cam)=0;

        /*! \brief allows to temporarily overwrite the camera preview settings and the current lightpath.

            Use resetCameraPreview() to go back to the normal settings.
         */
        virtual void overrideCameraPreview(int setup_cam, const QString& camera_settings, const QString& lightpath=QString(""))=0;
        /*! \brief reset the overridden camera settings set with overrideCameraPreview() to the user defaults */
        virtual void resetCameraPreview(int setup_cam)=0;

        /*! \rief calculate the overall system magnification for the given camera in the setup (currently 0,1) */
        virtual double getCameraMagnification(int setup_cam) const=0;
    signals:
        void lightpathesChanged(QFESPIMB040OpticsSetupItems lightpathes);

    public slots:
        virtual void loadLightpathConfig(const QString& filename, bool waiting=false, QString *name=NULL)=0;
        virtual void saveLightpathConfig(const QString& filename, const QString &name, QList<bool> saveProp=QList<bool>(), bool saveMeasured=false)=0;
        virtual void saveLightpathConfig(QMap<QString, QVariant>& data, const QString &name, const QString &prefix=QString(""), QList<bool> saveProp=QList<bool>(), bool saveMeasured=false)=0;
        virtual void saveCurrentLightpatConfig()=0;
        virtual void deleteCurrentLightpatConfig()=0;
        virtual void emitLighpathesChanged()=0;
        virtual void userChangedLightpath(QString filename)=0;
        virtual void unlockLighpathCombobox()=0;
        virtual void lockLighpathCombobox()=0;

};

#endif // QFESPIMB040OPTICSSETUPBASE_H
