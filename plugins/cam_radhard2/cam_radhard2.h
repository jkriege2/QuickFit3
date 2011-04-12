#ifndef CAM_RADHARD2_H
#define CAM_RADHARD2_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include "../interfaces/qfextensioncamera.h"
#include "radhard2lib/radhard2.h"
#include "radhard2lib/radhard2lib.h"
#include "radhard2lib/radhard2flash.h"
#include "highrestimer.h"
#include "qenhancedlineedit.h"
#include "jkstyledbutton.h"



/*!
    \defgroup qf3ext_radhard2 QFExtensionCamera implementation for Radhard2 SPAD array
    \ingroup qf3extensionplugins
    \defgroup radhard2 Radhard2 driver Classes and Routines
    \ingroup qf3ext_radhard2
*/



/*! \brief QFExtensionCamera implementation for Radhard2 SPAD array
    \ingroup qf3ext_radhard2

    This extensions allows access to data from the Radhard2 SPAD array. The array is produced by the
    group of Prof. Edoardo Charbon from the EPF Lausanne and TU Delft. It features 32x32 pixels and a readout
    time of at least 100kFrames/sec. This plugin operates together with the original firmware, which
    may be loaded, using the menu entry this plugin creates.

    This plugin reads/write configuration data to an INI file: \c plugins/extension/cam_radhard2/cam_radhard2.ini

    Loading a new Firmware into the FPGA uses code provided by E.Charbon, which may be found in SVN:
    \c B040/SPIM/trunk/docs_from_edoardo/radhard_stuff/radhard/software/usbflashloader
 */
class QFExtensionCameraRadhard2 : public QObject, public QFExtensionBase, public QFExtensionCamera {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFExtensionCamera)
    public:
        /** Default constructor */
        QFExtensionCameraRadhard2(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFExtensionCameraRadhard2();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("cam_radhard2"); };
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("Radhard2 SPAD array"); };
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("reads images from the Radhard2 SPAD array."); };
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan Krieger"); };
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2011 by J. Krieger"); };
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr(""); };
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/cam_radhard2_logo.png"); };
        /** \copydoc QFExtension::deinit() */
        virtual void deinit();

    protected:
        QFPluginLogService* logService;
        /** \brief QAction used to program the FPGA */
        QAction* actProgramFPGA;

        /** \copydoc QFExtensionBase::projectChanged() */
        virtual void projectChanged(QFProject* oldProject, QFProject* project);
        /** \copydoc QFExtensionBase::initExtension() */
        virtual void initExtension();
        /** \copydoc QFExtensionBase::loadSettings() */
        virtual void loadSettings(ProgramOptions* settings);
        /** \copydoc QFExtensionBase::storeSettings() */
        virtual void storeSettings(ProgramOptions* settings);


    /////////////////////////////////////////////////////////////////////////////
    // QFExtensionCamera routines
    /////////////////////////////////////////////////////////////////////////////
    public:
        /** \copydoc QFExtensionCamera::getCameraCount() */
        virtual unsigned int getCameraCount();
        /** \copydoc QFExtensionCamera::showCameraSettingsDialog() */
        virtual void showCameraSettingsDialog(unsigned int camera, QSettings& settings, QWidget* parent=NULL);
        /** \copydoc QFExtensionCamera::useCameraSettings() */
        virtual void useCameraSettings(unsigned int camera, const QSettings& settings);
        /** \copydoc QFExtensionCamera::getImageWidth() */
        virtual int getImageWidth(unsigned int camera);
        /** \copydoc QFExtensionCamera::getImageHeight() */
        virtual int getImageHeight(unsigned int camera);
        /** \copydoc QFExtensionCamera::isConnected() */
        virtual bool isConnected(unsigned int camera);
        /** \copydoc QFExtensionCamera::acquire() */
        virtual bool acquire(unsigned int camera, uint32_t* data, uint64_t* timestamp=NULL);
        /** \copydoc QFExtensionCamera::connectDevice() */
        virtual bool connectDevice(unsigned int camera);
        /** \copydoc QFExtensionCamera::disconnectDevice() */
        virtual void disconnectDevice(unsigned int camera);
        /** \copydoc QFExtensionCamera::getExposureTime() */
        virtual double getExposureTime(unsigned int camera);
        /** \copydoc QFExtensionCamera::setLogging() */
        virtual void setLogging(QFPluginLogService* logService) { this->logService=logService; };


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

    signals:
        /** \brief signal that may be used to display the current exposure time as a string */
        void displayAcquisitionTime(const QString& time);

    protected slots:

        /*! \brief slot which is called when the user edits a parameter in the configuration dialog

            This procedure clalculates the exposure time and frame rate and displays it emitting
            displayAcquisitionTime(). Note that this may only be called when spIterations and
            spDivider point to existing objects!!!
         */
        void updateAcquisitionTime();
        /** \brief send the divider to the hardware */
        void sendDivider();
        /** \brief send the iterations to the hardware */
        void sendIterations();
        /** \brief program the FPGA */
        void programFPGA();
        void programFPGAClicked();
        /*! \brief program the given \a bitfile into the FPGA (m for master or s for slave)

            \param bitfile which file to flash
            \param fpga \c 'm' to program master FPGA or \c 's' to program slave FPGA
            \param[out] messageOut contains the messages output by the flashing procedure
            \param retries number of retries when programming was not successfull
            \return \c true on successfull programming
        */
        bool flashFPGA(QString bitfile, char fpga, QString& messageOut, int retries=10);

    protected:
        /** \brief are we connected? */
        bool conn;
        /** \brief timer */
        HighResTimer timer;

        /** \brief access object for Radhard2 chip (provided by Charbon Lab) */
        Radhard2* radhard2;

        /** \brief the current divider setting */
        int divider;

        /** \brief the current iterations setting */
        int iterations;

        /** \brief subtract an offset of 1 */
        bool subtractOne;

        /** \brief Bitfile for programming */
        QString bitfile;
        /** \brief Bitfile for programming when connecting */
        QString autoflashbitfile;
        /** \brief indicates if the driver should try to download a bitfile to the FPGA on connecting */
        bool autoflash;

        QEnhancedLineEdit* edtBitfile;
        QLabel* labFlashSuccess;
        QSpinBox* spIterations;
        QSpinBox* spDivider;

        double calcExposureTime(double iterations, double divider);

};

#endif // CAM_RADHARD2_H
