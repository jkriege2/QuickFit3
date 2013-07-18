#ifndef QFESPIMB040OPTICSSETUP_H
#define QFESPIMB040OPTICSSETUP_H

#include <QWidget>
#include <QSettings>
#include "programoptions.h"
#include "qfpluginservices.h"

#include "programoptions.h"
#include "jkimage.h"
#include "jkqttools.h"
#include "qftools.h"

#include "qtlogfile.h"
#include "qfextensionmanager.h"
#include "../interfaces/qfextensioncamera.h"
#include "qfextension.h"
#include "tools.h"
#include "objectives.h"

#include "qtriple.h"
#include "qcheckablestringlistmodel.h"
#include "qftablemodel.h"
#include "qprogresslistwidget.h"
#include "../interfaces/qfextensionglobalsettingsreadwrite.h"
#include "qfextensionmeasurementdevice.h"
#include "qfextensionlightsource.h"
#include "qfextensionfilterchanger.h"
#include "qfespimb040opticssetupbase.h"
#include "qfextensionlinearstage.h"
#include "qfextensioncamera.h"
#include "qfcameraconfigcombobox.h"
#include "qfcameracombobox.h"
class QFESPIMB040MainWindow; // forward
namespace Ui {
    class QFESPIMB040OpticsSetup; // forward
}

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg): instrument setup widget
    \ingroup qf3ext_spimb040
 */
class QFESPIMB040OpticsSetup : public QFESPIMB040OpticsSetupBase {
        Q_OBJECT

    public:
        explicit QFESPIMB040OpticsSetup(QWidget* pluginMainWidget, QWidget* parent,  QFPluginLogService* log, QFPluginServices* pluginServices);
        ~QFESPIMB040OpticsSetup();

        void setLogging(QFPluginLogService* log);

        /** \brief load settings */
        void loadSettings(QSettings& settings, QString prefix);
        /** \brief save settings */
        void storeSettings(QSettings& settings, QString prefix);

        /** \brief removes the lightpath widget from it's place in the dialog and returns a pointer to it. */
        virtual QWidget* takeLightpathWidget() const;




        /*! \brief lock access to stages: stop the thread used for stage access by this widget

            \note call this, if you want to access the stage from any other method outside this widget!!! otherwise concurrent thread accesses are possible!!!
            \note You can release the lock y calling unlockStages().
          */
        void lockStages();
        /** \brief unlock access to stages: restart the thread used for stage access by this widget  */
        void unlockStages();
        /** \brief locks access to lightpath controls (shutters, laser, ...) */
        void lockLightpath();
        /** \brief unlocks access to lightpath controls (shutters, laser, ...) */
        void unlockLightpath();
        /*! \brief return a map containing a description of the optics setup, suitable for meta-data storage

            This function saves a description of the optics setup to a QMap. This method saves:
              - lenses and filters configuration
              - current illumination states (light sources, shutters, ...), if available
              - environment (temperatures, light intensities, ...)
            .

            if \a setup_cam > -1 the setup will not contain data which do not apply to specified camera (i.e. -1 is wildcard)
         */
        QMap<QString, QVariant> getSetup(int setup_cam=-1) const;

        /*! returns the number of available cameras */
        virtual int getCameraCount() const;

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
        bool lockCamera(int setup_cam, QFExtension** extension, QFExtensionCamera** ecamera, int* camera, QString* previewSettingsFilename);

        /*! \brief release a locked camera n, for more details see lockCamera() */
        void releaseCamera(int setup_cam);

        /*! \brief allows to temporarily overwrite the camera preview settings and the current lightpath.

            Use resetCameraPreview() to go back to the normal settings.
         */
        void overrideCameraPreview(int setup_cam, const QString& camera_settings, const QString& lightpath=QString(""));
        /*! \brief reset the overridden camera settings set with overrideCameraPreview() to the user defaults */
        void resetCameraPreview(int setup_cam);

        /*! \rief calculate the overall system magnification for the given camera in the setup (currently 0,1) */
        double getCameraMagnification(int setup_cam) const;




        /** \brief return a pointer to the axis stage class */
        virtual QFExtensionLinearStage* getStage(int stage);
        /** \brief return a pointer to the axis QFExtension class */
        virtual QFExtension* getStageExtension(int stage);
        virtual bool isStageConnected(int stage) const;
        /** \brief get the axis number of x-axis stage inside its class */
        virtual int getStageAxis(int stage);



        QFExtensionFilterChanger* getFilterChangerDetection() const;
        int getFilterChangerDetectionID() const;

        QFExtensionLightSource* getLaser(int laser);
        QFExtensionLightSource* getBrightfieldLightSource(int source);
        int getLaserID(int laser);
        int getBrightfieldLightSourceID(int source);

        QString getAxisNameForStage(QFExtensionLinearStage* stage, int axis);

        bool isStageConnected(QFExtensionLinearStage* stage, int id, bool& found);

        QFCameraComboBox* cameraComboBox(int camera) const;

        QFCameraConfigComboBoxStartResume* getStopRelease(int camera) const;




        /** \brief set main illumination shutter state */
        void setShutter(Shutters shutter, bool opened, bool blocking=false);


        /** \brief set main illumination shutter state */
        bool setMainIlluminationShutter(bool opened, bool blocking=false);
        /** \brief return \c true, if the main acquisition shutter is available and functional */
        bool isMainIlluminationShutterAvailable();
        /** \brief get main illumination shutter state */
        bool getMainIlluminationShutter();

        /** \brief return a description of the laser configuration */
        QString getLaserConfig();

        /** \brief returns \c true if the given lightpath is loaded */
        bool lightpathLoaded(const QString &filename);
        /** \brief returns the currently selected lightpath name */
        QString getCurrentLightpath() const;
        /** \brief returns the currently selected lightpath file */
        QString getCurrentLightpathFilename() const;
        /** \brief make sure the currently selected lightpath is used */
        void ensureLightpath();


        /** \brief collect all available measureable values (laser powers etz.) */
        measuredValues getMeasuredValues();

        /** \brief returns the DualView mode for a given camera (\c "none", \c "horicontal" or \c "vertical" ) */
        QString dualViewMode(int camera) const;
        /** \brief returns the DualView mode for a given camera (\c "none", \c "horicontal" or \c "vertical" ) */
        QString dualViewMode(QFExtensionCamera* ecam, int camera) const;

        /** \brief returns the logical number (0,1) of the specified camera
          *
          * This function compares the given data with the two specified cameras and returns the respective ID, if a match is found. If no match is found, the function returns -1.
          */
        int camNumFromExtension(QFExtensionCamera* ecam, int camera) const;
    public slots:
        void loadLightpathConfig(const QString& filename, bool waiting=false, QString *name=NULL);
        void saveLightpathConfig(const QString& filename, const QString &name, QList<bool> saveProp=QList<bool>(), bool saveMeasured=false);
        void saveLightpathConfig(QMap<QString, QVariant>& data, const QString &name, const QString &prefix=QString(""), QList<bool> saveProp=QList<bool>(), bool saveMeasured=false);
        void saveCurrentLightpatConfig();
        void deleteCurrentLightpatConfig();
        void emitLighpathesChanged();
        void userChangedLightpath(QString filename);
        void unlockLighpathCombobox();
        void lockLighpathCombobox();

    protected slots:
        void updateMagnifications();

        void on_btnConnectDevices_clicked();
        void on_btnConnectCameras_clicked();
        void on_btnDisconnectDevices_clicked();
        void on_btnDisconnectCameras_clicked();
        void on_chkDetectionFilterWheel_toggled(bool checked);
        void on_btnLoadSetup_clicked();
        void on_btnSaveSetup_clicked();
        void configsChanged(QFESPIMB040OpticsSetupItems configs);

        void configShortcuts();


    protected:
        void closeEvent(QCloseEvent * event);
        void showEvent( QShowEvent * event );

        void loadPluginGlobalSettings(QSettings& settings, QString prefix);
        void storePluginGlobalSettings(QSettings& settings, QString prefix) const;
        void loadPluginGlobalSettings(QSettings& settings, QObject *extensionObject, QString prefix);
        void storePluginGlobalSettings(QSettings& settings, QObject* extensionObject, QString prefix) const;

    private:
        Ui::QFESPIMB040OpticsSetup *ui;

    protected:
        QFPluginServices* m_pluginServices;

        QFPluginLogService* m_log;

        bool setting_lightpath;

        struct shortcutItem {
            QShortcut* shortcut;
            QString label;
            QString id;
        };

        QList<shortcutItem> shortcuts;

        QWidget* m_pluginMainWidget;

        QShortcut* addShortCut(const QString& id, const QString& label, const QKeySequence& sequence=QKeySequence());

        QFTableModel* lightpathConfigModel;
};

#endif // QFESPIMB040OPTICSSETUP_H
