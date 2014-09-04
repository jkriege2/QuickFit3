/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#ifndef QFESPIMB040OPTICSSETUP2_H
#define QFESPIMB040OPTICSSETUP2_H

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
#include "qfextensioncamera.h"
#include "qfextension.h"
#include "tools.h"
#include "objectives.h"

#include "qtriple.h"
#include "qcheckablestringlistmodel.h"
#include "qftablemodel.h"
#include "qprogresslistwidget.h"
#include "qfextensionglobalsettingsreadwrite.h"
#include "qfextensionmeasurementdevice.h"
#include "qfextensionlightsource.h"
#include "qfextensionfilterchanger.h"
#include "qfespimb040opticssetupbase.h"
#include "qfextensionlinearstage.h"
#include "qfextensioncamera.h"
#include "qfcameraconfigcombobox.h"
#include "qfcameracombobox.h"
#include "qfmanyfilessettings.h"
#include "filters.h"
#include "objectives.h"
#include "qfespimb040simplecameraconfig.h"
#include "qfespimb040samplestageconfig.h"
#include "qflightsourceconfigwidget.h"
#include "qfshutterconfigwidget.h"
#include "qfstageconfigwidget.h"
#include "qffilterchangerconfigwidget.h"
#include "qfmeasurementdeviceconfigwidget.h"
#include "qfespimb040simplecameraconfig.h"

#include <qalgorithms.h>

class QFESPIMB040MainWindow; // forward
namespace Ui {
    class QFESPIMB040OpticsSetup2; // forward
}

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg): instrument setup widget
    \ingroup qf3ext_spimb040
 */
class QFESPIMB040OpticsSetup2 : public QFESPIMB040OpticsSetupBase {
        Q_OBJECT

    public:
        explicit QFESPIMB040OpticsSetup2(QWidget* pluginMainWidget, QWidget* parent,  QFPluginLogService* log, QFPluginServices* pluginServices);
        ~QFESPIMB040OpticsSetup2();




        void loadOptSetup(const QString& filename);
        QString getLastOptSetup() const;




        void setLogging(QFPluginLogService* log);


        /** \brief load settings */
        void loadSettings(QFManyFilesSettings& settings, QString prefix=QString(""));
        void loadSettings(const QString& settingsFilename, QString prefix=QString(""));
        void loadSettings(const QStringList& settingsFilenames, QString prefix=QString(""), bool firstReadonly=true);
        /** \brief save settings */
        void storeSettings(QFManyFilesSettings& settings, QString prefix=QString(""));
        void storeSettings(const QString& settingsFilename, QString prefix=QString(""));
        void storeSettings(const QStringList& settingsFilenames, QString prefix=QString(""), bool firstReadonly=true);

        /** \brief removes the lightpath widget from it's place in the dialog and returns a pointer to it. */
        virtual QWidget* takeLightpathWidget() const;


        /** \brief returns the ID (int) of the given special shutter */
        virtual int getSpecialShutterID(Shutters shutter) const;

        /** \brief returns the ID (int) of the given special shutter */
        virtual int getSpecialStageID(specialStages shutter) const;

        /** \brief returns the ID (int) of the given special shutter */
        virtual int getSpecialBrightfieldID(specialBrightfieldSources shutter) const;

        /** \brief returns the ID (int) of the given special shutter */
        virtual int getSpecialFilterChangerID(specialFilterChangers shutter) const;


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
        virtual QString getStageName(int stage) const;
        QString getAxisNameForStage(QFExtensionLinearStage* stage, int axis);
        /*! returns the number of available stages */
        virtual int getStageCount() const;


        virtual QFExtensionFilterChanger* getFilterChanger(int changer) const;
        virtual int getFilterChangerID(int changer) const;
        virtual QString getFilterChangerName(int changer) const;
        /*! returns the number of available filter changers */
        virtual int getFilterChangerCount() const;


        QFExtensionLightSource* getLaser(int laser);
        QFExtensionLightSource* getBrightfieldLightSource(int source);
        int getLaserID(int laser);
        int getBrightfieldLightSourceID(int source);
        /*! returns the number of available lasers */
        virtual int getLaserCount() const;
        /*! returns the number of available brightfield light sources */
        virtual int getBrightfieldLightSourceCount() const;
        virtual QString getLaserName(int laser) const;
        virtual QString getBrightfieldLightSourceName(int lightsource) const;

        bool isStageConnected(QFExtensionLinearStage* stage, int id, bool& found);

        QFCameraComboBox* cameraComboBox(int camera) const;
        QFESPIMB040SimpleCameraConfig* cameraConfig(int camera) const;
        QFCameraConfigComboBoxStartResume* getStopRelease(int camera) const;




        /** \brief set a special shutter state */
        void setSpecialShutter(int shutter, bool opened, bool blocking=false);
        /** \brief set a shutter state */
        void setShutter(int shutter, bool opened, bool blocking=false);
        /*! returns the number of available shutters */
        virtual int getShutterCount() const;

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

        void changeDVenabledState();

        void mainShutterToggled(bool checked);
        void mainShutterOn();
        void mainShutterOff();

    protected:
        void closeEvent(QCloseEvent * event);
        void showEvent( QShowEvent * event );

        void loadPluginGlobalSettings(QSettings& settings, QString prefix);
        void storePluginGlobalSettings(QSettings& settings, QString prefix) const;
        void loadPluginGlobalSettings(QSettings& settings, QObject *extensionObject, QString prefix);
        void storePluginGlobalSettings(QSettings& settings, QObject* extensionObject, QString prefix) const;


    private:
        Ui::QFESPIMB040OpticsSetup2 *ui;

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

        QAction* actMainShutter;



        QString lastOptSetup;



        QMap<QString, QGroupBox*> ui_groups;
        QMap<QString, QWidget*> ui_widgets;
        QMap<QString, QLabel*> ui_labels;
        QMap<QString, QF3FilterCombobox*> ui_filters;
        QMap<QString, QF3ObjectiveCombobox*> ui_objectives;
        QMap<QString, QPair<QCheckBox*, QF3DualViewWidget*> > ui_dualviews;

        QString objProjection;
        QString objDetection;

        struct CameraWidgets {
            CameraWidgets() {
                config=NULL;
                filters.clear();
                tubelens="";
                shutter="";
                dualview="";
            }

            QFESPIMB040SimpleCameraConfig* config;
            QList<QString> filters;
            QString tubelens;
            QString shutter;
            QString dualview;
        };
        QMap<QString, CameraWidgets> ui_cameras;
        QStringList cameraIndex;

        QMap<QString, QFMeasurementDeviceConfigWidget*> ui_measurementdevices;
        QStringList measurementdeviceIndex;

        struct SpecialStage {
            SpecialStage() {
                device=0;
                stageconfig=NULL;
                stage=NULL;
            }

            QFESPIMB040SampleStageConfig* stageconfig;
            QFStageConfigWidget* stage;
            int device;
        };

        SpecialStage specialStageX;
        SpecialStage specialStageY;
        SpecialStage specialStageZ;
        SpecialStage specialStageR;

        QMap<QString, QFESPIMB040SampleStageConfig*> ui_stageconfigs;
        QMap<QString, QFStageConfigWidget*> ui_stages;
        /** \brief a list of all stages, if the ID is member of ui_stages, the int is ignored, otherwise the int specifies whether this is the x, the y or the z stage in a ui_stageconfigs */
        QList<QPair<QString, int> > stageIndex;
        QMap<QString, QFFilterChangerConfigWidget*> ui_filterchangers;
        QStringList filterChangerIndex;
        QString filtercDetection;
        QMap<QString, QFShutterConfigWidget*> ui_shutter;
        QStringList shutterIndex;

        enum MainShutterMode {
            msComplete=0, // when main shutter is closed, this shutter is close. Hwen main shutter is opened this shutter is opened
            msOpenOnly,   // when main shutter is closed, this shutter is not operated. Hwen main shutter is opened this shutter is opened
            msCloseOnly   // when main shutter is closed, this shutter is closed. Hwen main shutter is opened this shutter is not operated
        };

        struct MainShutterMember {
            QFShutterConfigWidget* shutter;
            MainShutterMode mode;
        };

        QList<MainShutterMember> shutterMain;
        QSet<int> waitForMainShutter;

        //QFShutterConfigWidget* shutterTransmission;
        QString shutterTransmissionID;
        //QFShutterConfigWidget* shutterLaser2;
        QString shutterLaser2ID;
        //QFShutterConfigWidget* shutterLaser1;
        QString shutterLaser1ID;

        struct LightsourceWidgets {
            LightsourceWidgets() {
                config=NULL;
                filters.clear();
                shutter="";
            }
            QFLightSourceConfigWidget* config;
            QList<QString> filters;
            QString shutter;
        };
        QMap<QString, LightsourceWidgets> ui_lightsource;
        QStringList lightsourceIndex;
        QStringList laserIndex;
        QStringList brightfieldIndex;
        QMap<QString, QTabWidget*> ui_tabs;




        template <class T, class TSet>
        friend void loadValueSettingsForAllInMap(T map_begin, T map_end, TSet& settings, const QString& prefix);
        template <class T, class TSet>
        friend void saveValueSettingsForAllInMap(T map_begin, T map_end, TSet& settings, const QString& prefix);




};



#endif // QFESPIMB040OPTICSSETUP2_H
