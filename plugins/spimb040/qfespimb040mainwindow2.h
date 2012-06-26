#ifndef QFESPIMB040MAINWINDOW2_H
#define QFESPIMB040MAINWINDOW2_H

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


#include "programoptions.h"
#include "jkimage.h"
#include "qfespimb040cameraview.h"
#include "jkqttools.h"
#include "qvisiblehandlesplitter.h"


#include "qtlogfile.h"
#include "qfextensionmanager.h"
#include "../interfaces/qfextensioncamera.h"
#include "../interfaces/qfextensionlinearstage.h"
#include "qfextension.h"
#include "tools.h"
#include "qfcameraconfigwidget.h"
#include "qfespimb040samplestageconfig.h"
#include "qfespimb040imagestackconfigwidget2.h"
#include "qfespimb040acquisitionconfigwidget2.h"
#include "qfespimb040camparamstackconfigwidget2.h"
#include "qfhtmlhelpwindow.h"
#include "qfespimb040opticssetup.h"
#include "qtriple.h"
#include "qftools.h"
#include "qfespimb040experimentdescription.h"

/*! \brief new SPIM Control Extension (B040, DKFZ Heidelberg) main window
    \ingroup qf3ext_spimb040

    This window allows to configure a SPIM microscope setup. Images are displayed in sub windows.

    It also implements QFPluginLogService and may thus be used for logging.
 */
class QFESPIMB040MainWindow2 : public QWidget, public QFPluginLogService {
        Q_OBJECT
        Q_INTERFACES(QFPluginLogService)
    public:
        QFESPIMB040MainWindow2(QFPluginServices* pluginServices, QWidget* parent=NULL);
        virtual ~QFESPIMB040MainWindow2();

        /** \brief load settings */
        void loadSettings(ProgramOptions* settings);
        /** \brief save settings */
        void storeSettings(ProgramOptions* settings);



    protected:


        /** \brief main log widget */
        QtLogFile* logMain;

        /** \brief main splitter */
        QVisibleHandleSplitter* splitter;

        QFESPIMB040OpticsSetup* optSetup;
        QFESPIMB040ExperimentDescription* widExperimentDescription;
        QFPluginServices* m_pluginServices;

        QTabWidget* tabAcquisition;
        QFESPIMB040AcquisitionConfigWidget2* widAcquisition;
        QFESPIMB040ImageStackConfigWidget2* widImageStack;
        QFESPIMB040CamParamStackConfigWidget2* widCamParamScan;


        QTabWidget* tabMain;
        QPushButton* btnHelp;



        /** \brief handles the close event, also close all camera views in camViews
         *
         *  disconnect devices and close camera view
         */
        void closeEvent ( QCloseEvent * event );

        void showEvent( QShowEvent * event );


        /** \brief Create all widgets on this window, called in the constructor before createActions() */
        void createWidgets(QFExtensionManager* extManager);

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
        QString saveAcquisitionDescription(int use_cam, QFExtension* extension, QFExtensionCamera* ecamera, int camera, const QString& filenamePrefix, const QMap<QString, QVariant>& acquisitionDescription, const QList<QFExtensionCamera::AcquititonFileDescription>& moreFiles, QDateTime startDateTime, bool getAcquisitionSettings=true);

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
        QString savePreviewDescription(int use_cam, QFExtension* extension, QFExtensionCamera* ecamera, int camera, const QString& filenamePrefix, const QMap<QString, QVariant>& acquisitionDescription, const QList<QFExtensionCamera::AcquititonFileDescription>& files, QDateTime startDateTime, const QString& lightpathPrefix=QString(""), const QString& prefix=QString(""));
        QString saveMeasuredData(const QString &filenamePrefix, const QList<QFESPIMB040OpticsSetup::measuredValues> &data);

    protected slots:
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

        void displayHelp();

        /*! \brief switch on/off the laser by opening/closing the main shutter

            If \A blocking is \c true, the function blocks (calling QApplication::processEvents() )
            until the shutter reached its desired state.
         */
        bool setMainIlluminationShutter(bool on_off=true, bool blocking=true);

    protected:
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
            \return \c true on success
         */
        bool savePreview(QFExtension* extension, QFExtensionCamera* ecamera, int camera, const QString& previewSettingsFilename, const QString& filename, QString* filename32);

        bool prepareCamera(int num, int camera, QFExtensionCamera *cam, const QString& acquisitionSettingsFilename, int &width, int &height, uint32_t **buffer, const QString& acquisitionTitle=QString("B040 SPIM Acquisition"));

        bool connectStageForAcquisition(QFExtensionLinearStage* stage, int stageAxis, double& stageInitialPos, const QString& acquisitionTitle=QString("B040 SPIM Acquisition"), int stageNum=1);

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
};

#endif // QFESPIMB040MAINWINDOW2_H
