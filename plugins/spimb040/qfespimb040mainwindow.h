#ifndef QFESPIMB040MAINWINDOW_H
#define QFESPIMB040MAINWINDOW_H

#include <QWidget>
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


#include "qtlogfile.h"
#include "qfextensionmanager.h"
#include "../interfaces/qfextensioncamera.h"
#include "../interfaces/qfextensionlinearstage.h"
#include "qfextension.h"
#include "tools.h"
#include "qfespimb040cameraconfig.h"
#include "qfespimb040samplestageconfig.h"
#include "qfespimb040imagestackconfigwidget.h"

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg) main window
    \ingroup qf3ext_spimb040

    This window allows to configure a SPIM microscope setup. Images are displayed in sub windows.

    It also implements QFPluginLogService and may thus be used for logging.
 */
class QFESPIMB040MainWindow : public QWidget, public QFPluginLogService {
        Q_OBJECT
    public:
        QFESPIMB040MainWindow(QFExtensionServices* pluginServices, QWidget* parent=NULL);
        virtual ~QFESPIMB040MainWindow();

        /** \brief load settings */
        void loadSettings(ProgramOptions* settings);
        /** \brief save settings */
        void storeSettings(ProgramOptions* settings);



    protected:


        /** \brief main log widget */
        QtLogFile* logMain;


        QFESPIMB040CameraConfig* camConfig1;
        QFESPIMB040CameraConfig* camConfig2;
        QFESPIMB040SampleStageConfig* sampleStages;
        QFExtensionServices* m_pluginServices;

        QTabWidget* tabAcquisition;
        QCheckBox* chkAcquisitionUseCam1;
        QCheckBox* chkAcquisitionUseCam2;
        QLineEdit* edtAcquisitionPrefix1;
        QLineEdit* edtAcquisitionPrefix2;
        QPushButton* btnAcquire;
        QSpinBox* spinAcquisitionCount;

        QFESPIMB040ImageStackConfigWidget* widImageStack;



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

            \param extension QFExtension object describing the used camera (must be castable to QFExtensionCamera)
            \param camera =0,1 which camera was used?
            \param filenamePrefix prefix for the acquisition output files
            \param acquisitionDescription additional key-value-pairs describing the acquisition mode (will be stored under \c [acquisition] heading)
            \return filename of the settings file
        */
        QString saveAcquisitionDescription(QFExtension* extension, QFExtensionCamera* ecamera, int camera, const QString& filenamePrefix, const QMap<QString, QVariant>& acquisitionDescription);

        /*! \brief save a description of an acquisition in preview mode to a QSettings object

            Settings will be stored to the file \c <filenamePrefix>_configuration.ini .

            \param extension QFExtension object describing the used camera (must be castable to QFExtensionCamera)
            \param camera =0,1 which camera was used?
            \param filenamePrefix prefix for the acquisition output files
            \param acquisitionDescription additional key-value-pairs describing the acquisition mode (will be stored under \c [acquisition] heading)
            \param files files that have been stored (see  QFExtensionCamera::AcquititonFileDescription)
            \return filename of the settings file
        */
        QString savePreviewDescription(QFExtension* extension, QFExtensionCamera* ecamera, int camera, const QString& filenamePrefix, const QMap<QString, QVariant>& acquisitionDescription, const QList<QFExtensionCamera::AcquititonFileDescription>& files);

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

#endif // QFESPIMB040MAINWINDOW_H
