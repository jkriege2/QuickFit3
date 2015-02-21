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

#ifndef QFESPIMB040MAINWINDOW2_H
#define QFESPIMB040MAINWINDOW2_H
#include "qfversion.h"
#include <QWidget>
#include "qvisiblehandlesplitter.h"
#include <QSplitter>
#include <QWidget>
#include <QComboBox>
#include <QSettings>
#include <QMenu>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <cmath>
#include <QLabel>
#include <QPushButton>
#include <QToolBar>
#include <QTime>
#include <QTabWidget>
#include <QGroupBox>
#include "qfenhancedtabwidget.h"


#include "programoptions.h"
#include "jkimage.h"
#include "qfespimb040cameraview.h"
#include "jkqttools.h"
#include "qvisiblehandlesplitter.h"
#include "qfespimb040acquisitiontools.h"
#include "datatools.h"


#include "qtlogfile.h"
#include "qfextensionmanager.h"
#include "qfextensioncamera.h"
#include "qfextensionlinearstage.h"
#include "qfextension.h"
#include "tools.h"
#include "qfespimb040samplestageconfig.h"
#include "qfespimb040imagestackconfigwidget2.h"
#include "qfespimb040acquisitionconfigwidget2.h"
#include "qfespimb040camparamstackconfigwidget2.h"
#include "qfespimb040opticssetup.h"
#include "qfespimb040opticssetup2.h"
#include "qtriple.h"
#include "qftools.h"
#include "qfespimb040experimentdescription.h"
#include "qfespimb040acquisitiondescription.h"
#include "qfespimb040deviceparamstackconfigwidget.h"
#include "qfespimb040scriptedacquisition.h"
#include "qfespimb040configtabwidget.h"
#include "qfespimb040overviewacquisitionconfigwidget.h"

class QFESPIMB040; // forward

/*! \brief new SPIM Control Extension (B040, DKFZ Heidelberg) main window
    \ingroup qf3ext_spimb040

    This window allows to configure a SPIM microscope setup. Images are displayed in sub windows.

    It also implements QFPluginLogService and may thus be used for logging.
 */
class QFESPIMB040MainWindow2 : public QWidget, public QFPluginLogService, public QFESPIMB040AcquisitionTools{
        Q_OBJECT
        Q_INTERFACES(QFPluginLogService QFESPIMB040AcquisitionTools)
    public:
        QFESPIMB040MainWindow2(const QString& optSetupFile, QFPluginServices* pluginServices, QWidget* parent=NULL, bool newOpticsSetup=false, QFESPIMB040* plugin=NULL);
        virtual ~QFESPIMB040MainWindow2();

        /** \brief load settings */
        void loadSettings(ProgramOptions* settings);
        /** \brief save settings */
        void storeSettings(ProgramOptions* settings);

        void setOptSetup(const QString& optSetupFile);

        QFESPIMB040AcquisitionConfigWidget2* getWidAcquisition() const;


    protected:


        /** \brief main log widget */
        QtLogFile* logMain;

        QString optSetupFile;

        /** \brief main splitter */
        QVisibleHandleSplitter* splitter;

        QFESPIMB040OpticsSetupBase* optSetup;
        QFESPIMB040OpticsSetup* optSetup1;
        QFESPIMB040OpticsSetup2* optSetup2;
        QFESPIMB040ExperimentDescription* widExperimentDescription;
        QFPluginServices* m_pluginServices;

        QFEnhancedTabWidget* tabAcquisition;
        QFESPIMB040AcquisitionConfigWidget2* widAcquisition;
        QFESPIMB040ImageStackConfigWidget2* widImageStack;
        QFESPIMB040CamParamStackConfigWidget2* widCamParamScan;
        QFESPIMB040DeviceParamStackConfigWidget* widDeviceParamScan;
        QFESPIMB040AcquisitionDescription* widAcquisitionDescription;
        QFESPIMB040ScriptedAcquisition* widScriptedAcquisition;
        //QFESPIMB040ConfigTabWidget* widConfig;
        QFESPIMB040OverviewAcquisitionConfigWidget* widOverview;


        QFEnhancedTabWidget* tabMain;
        QPushButton* btnHelp;

        QFESPIMB040* plugin;



        /** \brief handles the close event, also close all camera views in camViews
         *
         *  disconnect devices and close camera view
         */
        void closeEvent ( QCloseEvent * event );

        void showEvent( QShowEvent * event );


        /** \brief Create all widgets on this window, called in the constructor before createActions() */
        void createWidgets(QFExtensionManager* extManager, bool newOpticsSetup);

    public slots:
        /*! \brief runs an image acquisition

            This function starts an image acquisition for every of the cameras. Then it waits until the acquisitions are completed.
          */
        void doAcquisition();
        /*! \brief runs an image stack acquisition

            This function is configured by widImageStack (QFESPIMB040ImageStackConfigWidget). It takes a stage and scans it over a range of values
            (given by start, delta and steps). At each position it records a single frame from every camera. The frames are stored in a multi-frame
            16-bit TIFF file for eachcamera separately.
          */
        void doImageStack();

        void doCamParamStack();

        void doDeviceParameterStack();


        void displayHelp();

        /*! \brief switch on/off the laser by opening/closing the main shutter

            If \A blocking is \c true, the function blocks (calling QApplication::processEvents() )
            until the shutter reached its desired state.
         */
        bool setMainIlluminationShutter(bool on_off=true, bool blocking=true);

        //acquisitionPrefix1+"_overview.tif"
    public:
        virtual void savePreviewMovie(int camera, int frames, const QString& filename);
        virtual void savePreviewMovie(int camera, int frames, const QString& filename, const QMap<QString, QVariant>& cam_params);

        void storeCameaConfig(QSettings& setting, QFExtensionCamera* cam, int camID, bool getMeasured=true);

        /*! \brief save an image from the given camera as a 16-bit TIFF image into \a filename

            \note This function configures the camera with the preview settings, acquires ONE frame and saves it to the given file.
                  If the gray value range of the frame exceeds 16-bit it is compressed to 16-bit (i.e. gray_max -> 65535) and a 32-bit
                  version is saved!

            \param extension QFExtension of the used camera
            \param ecamera QFExtensionCamera of the used camera
            \param camera device index of the camera inside ecamera
            \param previewSettingsFilename the preview settings file used for this acquisition
            \param filename the file to save to (if a 32-version is created, its filename is the same as \A filename, but with the extension \c .32.tiff !).
            \param[out] filename32 may be used to return the filename of the potentially created 32-bit TIFF (is set to an empty string, if no 32-bit TIFF was created)
            \param mainShutterOpenOnlyForAcquisition if \c true, the main shutter is closed initially, only opened for the acquisition and closed after it again.
            \return \c true on success
         */
        bool savePreview(QFExtension* extension, QFExtensionCamera* ecamera, int camera, const QString& previewSettingsFilename, const QString& filename, QString* filename32, QMap<QString, QVariant>* acquisitionDescription=NULL, const QString& acquisitionDescriptionPrefix=QString(""), bool mainShutterOpenOnlyForAcquisition=false, int frames=1, bool getMeasurements=true);

        bool prepareCamera(int num, int camera, QFExtensionCamera *cam, const QString& acquisitionSettingsFilename, int &width, int &height, uint32_t **buffer, const QString& acquisitionTitle=QString("B040 SPIM Acquisition"));

        bool connectStageForAcquisition(QFExtensionLinearStage* stage, int stageAxis, double& stageInitialPos, const QString& acquisitionTitle=QString("B040 SPIM Acquisition"), int stageNum=1);
        /*! \brief set a lighpath and acquire one image with the given camera and settings.

            \note THe lightpath is not altered, if lightpathFilename is left empty.
         */
        bool acquireImageWithLightpath(const QString& lightpathFilename, const QString& lightpathName, QFExtension* extension1, QFExtensionCamera* ecamera1, int camera1, const QString& previewSettingsFilename1, const QString& outputFilename, const QString& imageID, const QString& imageDescription, QList<QFExtensionCamera::CameraAcquititonFileDescription>& moreFiles1, QMap<QString, QVariant>& acquisitionDescription1, bool mainShutterOpenOnlyForAcquisition=false, int frames=1, bool measureDuringAcquisitions=true);

        /*! \brief do an acquisition on one or two cameras

            \note if \a frames \t <=0, the number of frames will NOT be changed, otherwise the number of frames will be set accordingly (if the driver allows it!)
            \note if \a measured is \c NULL no measurements will be stored during acquisition. otherwise they will be stored to this map.
         */
        bool acquireSeries(const QString& lightpathName, const QString& imageID, const QString& imageDescription, bool useCam1, QFExtension* extension1, QFExtensionCamera* ecamera1, int camera1, const QString& acquisitionPrefix1, const QString& acquisitionSettingsFilename1, QMap<QString, QVariant>& acquisitionDescription1, QList<QFExtensionCamera::CameraAcquititonFileDescription>& moreFiles1, bool useCam2, QFExtension* extension2, QFExtensionCamera* ecamera2, int camera2, const QString& acquisitionPrefix2, const QString& acquisitionSettingsFilename2, QMap<QString, QVariant>& acquisitionDescription2, QList<QFExtensionCamera::CameraAcquititonFileDescription>& moreFiles2, const QMap<QFExtensionCamera::CameraSetting, QVariant>& camsettings1, const QMap<QFExtensionCamera::CameraSetting, QVariant>& camsettings2, const QMap<QString, QVariant>& camsettingsstr1, const QMap<QString, QVariant>& camsettingsstr2, QList<QFESPIMB040OpticsSetupBase::measuredValues>* measured=NULL, QProgressListDialog* progress=NULL, bool* userCanceled=NULL, bool measureDuringAcquisitions=true);
        bool acquireSeries(const QString& lightpathName, const QString& imageID, const QString& imageDescription, bool useCam1, QFExtension* extension1, QFExtensionCamera* ecamera1, int camera1, const QString& acquisitionPrefix1, QSettings& acquisitionSettings1, QMap<QString, QVariant>& acquisitionDescription1, QList<QFExtensionCamera::CameraAcquititonFileDescription>& moreFiles1, bool useCam2, QFExtension* extension2, QFExtensionCamera* ecamera2, int camera2, const QString& acquisitionPrefix2, QSettings& acquisitionSettings2, QMap<QString, QVariant>& acquisitionDescription2, QList<QFExtensionCamera::CameraAcquititonFileDescription>& moreFiles2, QList<QFESPIMB040OpticsSetupBase::measuredValues>* measured=NULL, QProgressListDialog* progress=NULL, bool* userCanceled=NULL, bool measureDuringAcquisitions=true);

        void getAdditionalCameraSettings(QFExtensionCamera* ecamera, int camera, const QString& prefix, QMap<QString, QVariant>& acquisitionDescription, bool getMeasurements=true);

        /*! \brief save a description of an acquisition to a QSettings object

            Settings will be stored to the file \c <filenamePrefix>_configuration.ini .

            \param use_cam use camera 0, 1, ... in the setup
            \param extension QFExtension object describing the used camera (must be castable to QFExtensionCamera)
            \param camera =0,1 which camera was used?
            \param filenamePrefix prefix for the acquisition output files
            \param acquisitionDescription additional key-value-pairs describing the acquisition mode (will be stored under \c [acquisition] heading)
            \param moreFiles a list of additionally created files
            \return filename of the settings file
        */
        QString saveAcquisitionDescription(int use_cam, QFExtension* extension, QFExtensionCamera* ecamera, int camera, const QString& filenamePrefix, const QMap<QString, QVariant>& acquisitionDescription, const QList<QFExtensionCamera::CameraAcquititonFileDescription>& moreFiles, QDateTime startDateTime, bool getAcquisitionSettings=true);

        /*! \brief save a description of an acquisition in preview mode to a QSettings object

            Settings will be stored to the file \c <filenamePrefix>_configuration.ini .

            \param use_cam use camera 0, 1, ... in the setup
            \param extension QFExtension object describing the used camera (must be castable to QFExtensionCamera)
            \param camera =0,1 which camera was used?
            \param filenamePrefix prefix for the acquisition output files
            \param acquisitionDescription additional key-value-pairs describing the acquisition mode (will be stored under \c [acquisition] heading)
            \param files files that have been stored (see  QFExtensionCamera::AcquititonFileDescription)
            \return filename of the settings file
        */
        QString savePreviewDescription(int use_cam, QFExtension* extension, QFExtensionCamera* ecamera, int camera, const QString& filenamePrefix, const QMap<QString, QVariant>& acquisitionDescription, const QList<QFExtensionCamera::CameraAcquititonFileDescription>& files, QDateTime startDateTime, const QString& lightpathPrefix=QString(""), const QString& prefix=QString(""));
        QString saveMeasuredData(const QString &filenamePrefix, const QList<QFESPIMB040OpticsSetupBase::measuredValues> &data);

    public:

        /** \brief log project text message
         *  \param message the message to log
         */
        virtual void log_text(QString message);
        /** \brief log project warning message
         *  \param message the warning message to log
         */
        virtual void log_warning(QString message);
        /** \brief log project error message
         *  \param message the error message to log
         */
        virtual void log_error(QString message);

    protected slots:
        void styleChanged(const QString& style, const QString& stylesheet);
    private:
        /** \brief loads the given style and stylesheet and sets the given widget to it */
        void setStyleAndStylesheet(QWidget *widget, const QString &style, const QString &stylesheet);

};

#endif // QFESPIMB040MAINWINDOW2_H
