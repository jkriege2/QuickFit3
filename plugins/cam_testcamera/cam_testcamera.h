#ifndef QFECamTestCamera_H
#define QFECamTestCamera_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include "../interfaces/qfextensioncamera.h"
#include "highrestimer.h"

/*!
    \defgroup qf3ext_testcamera QFExtensionCamera implementation which outputs configurable test images
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtensionCamera implementation which outputs configurable test images
    \ingroup qf3ext_testcamera
 */
class QFECamTestCamera : public QObject, public QFExtensionBase, public QFExtensionCamera {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFExtensionCamera)
    public:
        /** Default constructor */
        QFECamTestCamera(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFECamTestCamera();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////

        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("cam_testcamera"); };

        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("Test Image Camera Device"); };

        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("outputs configurable test images and pretends to be a camera extension"); };

        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan Krieger"); };

        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2011 by Jan Krieger"); };

        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr(""); };

        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/cam_testcamera_logo.png"); };


        /** \copydoc QFExtension::deinit() */
        virtual void deinit();

    protected:

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

        /** \copydoc QFExtensionCamera::showSettingsWidget() */
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

    protected:
        QFPluginLogService* logService;
        /** \brief are we connected? */
        bool conn[2];
        /** \brief image counter */
        uint64_t counter[2];
        /** \brief time when device was connected */
        time_t startTime[2];
        /** \brief timer */
        HighResTimer timer[2];
        /** \brief width of the resulting image */
        int width[2];
        /** \brief height of the resulting image */
        int height[2];
        /** \brief type of test pattern */
        int testpattern[2];
        /** \brief noise level 1 = 100% */
        double noise[2];
        /** \brief number of particles in "particle" mode */
        int particleN[2];
        /** \brief number of particles in "particle" mode */
        double particleBackground[2];
        /** \brief x-coordinates of particles */
        int* particleX[2];
        /** \brief y-coordinates of particles */
        int* particleY[2];
        /** \brief average number of photons per step */
        double particleBrightnes[2];
        /** \brief size of the particle PSF (gaussian) */
        double particlePSF[2];
        /** \brief indicate whether there should be some hot pixels */
        int hotpixels[2];

        void initParticles(int camera, int n);
        void stepParticles(int camera);

    public slots:
        void setWidth0(int width);
        void setHeight0(int height);
        void setTestPattern0(int pattern);
        void setNoise0(double n);
        void setParticleN0(int n);
        void setParticleBrightnes0(double b);
        void setParticleBackground0(double b);
        void setParticlePSF0(double psf);
        void setHotpixels0(int num);

        void setWidth1(int width);
        void setHeight1(int height);
        void setTestPattern1(int pattern);
        void setNoise1(double n);
        void setParticleN1(int n);
        void setParticleBrightnes1(double b);
        void setParticleBackground1(double b);
        void setParticlePSF1(double psf);
        void setHotpixels1(int num);
    signals:
        void displayNoise0(double n);
        void displayWidth0(int width);
        void displayHeight0(int height);
        void displayParticleN0(int n);
        void displayParticleBrightnes0(double brightnes);
        void displayParticleBackground0(double background);
        void displayParticlePSF0(double psf);
        void displayTestPattern0(int pattern);
        void enableParticles0(bool en);
        void enableDefault0(bool en);
        void displayHotpixels0(int num);

        void displayNoise1(double n);
        void displayWidth1(int width);
        void displayHeight1(int height);
        void displayParticleN1(int n);
        void displayParticleBrightnes1(double brightnes);
        void displayParticleBackground1(double background);
        void displayParticlePSF1(double psf);
        void displayTestPattern1(int pattern);
        void enableParticles1(bool en);
        void enableDefault1(bool en);
        void displayHotpixels1(int num);

};

#endif // QFECamTestCamera_H
