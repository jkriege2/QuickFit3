#ifndef QFESPIMB040OPTICSSETUPBASE_H
#define QFESPIMB040OPTICSSETUPBASE_H
#include <QWidget>
#include <QMap>
#include <QString>
#include <QVariant>
#include <QList>
#include "qfextension.h"
#include "qfextensioncamera.h"
#include "qfextensionmeasurementdevice.h"
#include "qfextensionlightsource.h"
#include "qfextensionfilterchanger.h"
#include "qfextensionlinearstage.h"
#include "qfextensioncamera.h"
#include "qfcameraconfigcombobox.h"
#include "qfcameracombobox.h"

class QFESPIMB040AcquisitionTools;//forward

typedef QList<QTriple<QIcon, QString, QString> > QFESPIMB040OpticsSetupItems;

class QFESPIMB040OpticsSetupBase: public QWidget {
        Q_OBJECT
    public:
        QFESPIMB040OpticsSetupBase(QWidget* parent=NULL);
        virtual ~QFESPIMB040OpticsSetupBase();

        void setAcquisitionTools(QFESPIMB040AcquisitionTools* acqTools);
        QFESPIMB040AcquisitionTools* getAcquisitionTools() const;


        enum Shutters  {
            ShutterMain=0,
            ShutterLaser1=1,
            ShutterLaser2=2,
            ShutterTransmission=3
        };

        enum specialStages {
            StageX=0,
            StageY=1,
            StageZ=2,
            StageR=3
        };

        enum specialBrightfieldSources {
            BrightfieldTransmission=0,
            BrightfieldEpi=1
        };

        enum specialFilterChangers {
            FilterChangerDetection=0
        };


        /** \brief returns the ID (int) of the given special shutter */
        virtual int getSpecialShutterID(Shutters shutter) const=0;

        /** \brief returns the ID (int) of the given special shutter */
        virtual int getSpecialStageID(specialStages shutter) const=0;

        /** \brief returns the ID (int) of the given special shutter */
        virtual int getSpecialBrightfieldID(specialBrightfieldSources shutter) const=0;

        /** \brief returns the ID (int) of the given special shutter */
        virtual int getSpecialFilterChangerID(specialFilterChangers shutter) const=0;





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

        /*! returns the number of available cameras */
        virtual int getCameraCount() const=0;







        /** \brief return a pointer to the axis stage class */
        virtual QFExtensionLinearStage* getStage(int stage)=0;
        /** \brief return a pointer to the axis QFExtension class */
        virtual QFExtension* getStageExtension(int stage)=0;
        virtual bool isStageConnected(int stage) const=0;
        /** \brief get the axis number of x-axis stage inside its class */
        virtual int getStageAxis(int stage)=0;
        virtual QString getStageName(int stage) const=0;















        virtual QFExtensionFilterChanger* getFilterChanger(int changer) const=0;
        virtual int getFilterChangerID(int changer) const=0;
        /*! returns the number of available filter changers */
        virtual int getFilterChangerCount() const=0;
        virtual QString getFilterChangerName(int changer) const=0;

        virtual QFExtensionLightSource* getLaser(int laser)=0;
        virtual QFExtensionLightSource* getBrightfieldLightSource(int source)=0;
        virtual int getLaserID(int laser)=0;
        virtual int getBrightfieldLightSourceID(int source)=0;
        /*! returns the number of available lasers */
        virtual int getLaserCount() const=0;
        /*! returns the number of available brightfield light sources */
        virtual int getBrightfieldLightSourceCount() const=0;
        virtual QString getLaserName(int laser) const=0;
        virtual QString getBrightfieldLightSourceName(int lightsource) const=0;


        virtual QString getAxisNameForStage(QFExtensionLinearStage* stage, int axis)=0;

        virtual bool isStageConnected(QFExtensionLinearStage* stage, int id, bool& found)=0;
        /*! returns the number of available stages */
        virtual int getStageCount() const=0;

        virtual QFCameraComboBox* cameraComboBox(int camera) const=0;

        virtual QFCameraConfigComboBoxStartResume* getStopRelease(int camera) const=0;



        /** \brief set a special shutter state */
        virtual void setSpecialShutter(int shutter, bool opened, bool blocking=false)=0;
        /** \brief set a shutter state */
        virtual void setShutter(int shutter, bool opened, bool blocking=false)=0;
        /*! returns the number of available shutters */
        virtual int getShutterCount() const=0;

        /** \brief set main illumination shutter state */
        virtual bool setMainIlluminationShutter(bool opened, bool blocking=false)=0;
        /** \brief return \c true, if the main acquisition shutter is available and functional */
        virtual bool isMainIlluminationShutterAvailable()=0;
        /** \brief get main illumination shutter state */
        virtual bool getMainIlluminationShutter()=0;

        /** \brief return a description of the laser configuration */
        virtual QString getLaserConfig()=0;

        /** \brief returns \c true if the given lightpath is loaded */
        virtual bool lightpathLoaded(const QString &filename)=0;
        /** \brief returns the currently selected lightpath name */
        virtual QString getCurrentLightpath() const=0;
        /** \brief returns the currently selected lightpath file */
        virtual QString getCurrentLightpathFilename() const=0;
        /** \brief make sure the currently selected lightpath is used */
        virtual void ensureLightpath()=0;


        /** \brief collect all available measureable values (laser powers etz.) */
        virtual measuredValues getMeasuredValues()=0;

        /** \brief returns the DualView mode for a given camera (\c "none", \c "horicontal" or \c "vertical" ) */
        virtual QString dualViewMode(int camera) const=0;
        /** \brief returns the DualView mode for a given camera (\c "none", \c "horicontal" or \c "vertical" ) */
        virtual QString dualViewMode(QFExtensionCamera* ecam, int camera) const=0;

        /** \brief returns the logical number (0,1) of the specified camera
          *
          * This function compares the given data with the two specified cameras and returns the respective ID, if a match is found. If no match is found, the function returns -1.
          */
        virtual int camNumFromExtension(QFExtensionCamera* ecam, int camera) const=0;
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

    protected:
        QFESPIMB040AcquisitionTools* acqTools;

};

#endif // QFESPIMB040OPTICSSETUPBASE_H
