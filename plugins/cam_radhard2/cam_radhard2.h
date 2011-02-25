#ifndef CAM_RADHARD2_H
#define CAM_RADHARD2_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include "../interfaces/qfextensioncamera.h"
#include "radhard2lib/radhard2.h"
#include "radhard2lib/radhard2lib.h"
#include "highrestimer.h"



/*!
    \defgroup qf3ext_radhard2 QFExtensionCamera implementation for Radhard2 SPAD array
    \ingroup qf3extensionplugins
    \defgroup radhard2 Radhard2 driver Classes and Routines
    \ingroup qf3ext_radhard2
*/



/*! \brief QFExtensionCamera implementation for Radhard2 SPAD array
    \ingroup qf3ext_radhard2
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
        /** \copydoc QFExtensionCamera::createSettingsWidget() */
        virtual void showSettingsWidget(unsigned int camera, QWidget* parent=NULL);
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

        /** \brief indent all following lines in the logging pane */
        virtual void log_indent();
        /** \brief undo former log_indent() */
        virtual void log_unindent();

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

        /** \brief slot which is called when the user edits a parameter in the configuration dialog
         */
        void updateAcquisitionTime();
        /** \brief send the divider to the hardware */
        void sendDivider();
        /** \brief send the iterations to the hardware */
        void sendIterations();

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

};

#endif // CAM_RADHARD2_H
