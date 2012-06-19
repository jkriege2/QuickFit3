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
#include "qfextensionlinearstage.h"
#include "qfextensioncamera.h"
#include "qfcameraconfigcombobox.h"
#include "qfcameracombobox.h"
#include "qtriple.h"
#include "qcheckablestringlistmodel.h"
#include "qftablemodel.h"
#include "qprogresslistwidget.h"
#include "qfextensionmeasurementdevice.h"


class QFESPIMB040MainWindow; // forward
namespace Ui {
    class QFESPIMB040OpticsSetup; // forward
}

typedef QList<QTriple<QIcon, QString, QString> > QFESPIMB040OpticsSetupItems;
/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg): instrument setup widget
    \ingroup qf3ext_spimb040
 */
class QFESPIMB040OpticsSetup : public QWidget {
        Q_OBJECT

    public:
        explicit QFESPIMB040OpticsSetup(QWidget* pluginMainWidget, QWidget* parent,  QFPluginLogService* log, QFPluginServices* pluginServices);
        ~QFESPIMB040OpticsSetup();

        void setLogging(QFPluginLogService* log);

        /** \brief load settings */
        void loadSettings(QSettings& settings, QString prefix);
        /** \brief save settings */
        void storeSettings(QSettings& settings, QString prefix);
        /*! \brief lock access to stages: stop the thread used for stage access by this widget

            \note call this, if you want to access the stage from any other method outside this widget!!! otherwise concurrent thread accesses are possible!!!
            \note You can release the lock y calling unlockStages().
          */
        void lockStages();
        /** \brief unlock access to stages: restart the thread used for stage access by this widget  */
        void unlockStages();

        void lockLightpath();
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

        /*! \rief calculate the overall system magnification for the given camera in the setup (currently 0,1) */
        double getCameraMagnification(int setup_cam) const;

        /** \brief return a pointer to the x-axis stage class */
        QFExtensionLinearStage* getXStage();

        /** \brief return a pointer to the y-axis stage class */
        QFExtensionLinearStage* getYStage();

        /** \brief return a pointer to the z-axis stage class */
        QFExtensionLinearStage* getZStage();

        /** \brief return a pointer to the x-axis QFExtension class */
        QFExtension* getXStageExtension();

        /** \brief return a pointer to the y-axis QFExtension class */
        QFExtension* getYStageExtension();

        /** \brief return a pointer to the z-axis QFExtension class */
        QFExtension* getZStageExtension();

        bool isXStageConnected() const;
        bool isYStageConnected() const;
        bool isZStageConnected() const;

        QString getAxisNameForStage(QFExtensionLinearStage* stage, int axis);

        bool isStageConnected(QFExtensionLinearStage* stage, int id, bool& found);

        QFCameraComboBox* cameraComboBox(int camera) const;

        QFCameraConfigComboBoxStartResume* getStopRelease(int camera) const;


        /** \brief get the axis number of x-axis stage inside its class */
        int getXStageAxis();

        /** \brief get the axis number of y-axis stage inside its class */
        int getYStageAxis();

        /** \brief get the axis number of z-axis stage inside its class */
        int getZStageAxis();

        /** \brief set main illumination shutter state */
        void setMainIlluminationShutter(bool opened);
        /** \brief return \c true, if the main acquisition shutter is available and functional */
        bool isMainIlluminationShutterAvailable();
        /** \brief get main illumination shutter state */
        bool getMainIlluminationShutter();

        bool lightpathLoaded(const QString &filename);
        QString getCurrentLightpath() const;
        QString getCurrentLightpathFilename() const;
        void ensureLightpath();

        struct measuredValues {
            QDateTime time;
            QMap<QString, QVariant> data;

            measuredValues() {
                time=QDateTime::currentDateTime();
                data.clear();
            }
        };

        /** \brief collect all available measureable values (laser powers etz.) */
        measuredValues getMeasuredValues();
    public slots:
        void loadLightpathConfig(const QString& filename, bool waiting=false);
        void saveLightpathConfig(const QString& filename, const QString &name, const QList<bool> &saveProp=QList<bool>(), bool saveMeasured=false);
        void saveLightpathConfig(QMap<QString, QVariant>& data, const QString &name, const QString &prefix=QString(""), const QList<bool>& saveProp=QList<bool>(), bool saveMeasured=false);
        void saveCurrentLightpatConfig();
        void deleteCurrentLightpatConfig();
        void emitLighpathesChanged();
        void userChangedLightpath(QString filename);
    signals:
        void lightpathesChanged(QFESPIMB040OpticsSetupItems lightpathes);

    protected slots:
        void updateMagnifications();

        void on_btnConnectDevices_clicked();
        void on_btnConnectCameras_clicked();
        void on_btnDisconnectDevices_clicked();
        void on_btnDisconnectCameras_clicked();
        void on_chkDetectionFilterWheel_toggled(bool checked);
        void configsChanged(QFESPIMB040OpticsSetupItems configs);

        void configShortcuts();


    protected:
        void closeEvent(QCloseEvent * event);
        void showEvent( QShowEvent * event );
    private:
        Ui::QFESPIMB040OpticsSetup *ui;
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
