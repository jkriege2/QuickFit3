#ifndef QFEXTENSIONCAMERA_H
#define QFEXTENSIONCAMERA_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include "../interfaces/qfextensioncamera.h"

/*!
    \defgroup qf3ext_GROUPNAME QFExtensionCamera implementation
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtensionCamera implementation
    \ingroup qf3ext_GROUPNAME
 */
class QFExtensionCameraImplementation : public QObject, public QFExtensionBase, public QFExtensionCamera {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFExtensionCamera)
    public:
        /** Default constructor */
        QFExtensionCameraImplementation(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFExtensionCameraImplementation();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString(QF_TOSTR(TARGETNAME)); };
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr(/* human readable name for plugin */); };
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr(/* human-readable description for plugin */); };
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr(/* author name */); };
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr(/* copyright information */); };
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr(/* your homepage */); };
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { /* plug in name from .qrc file */ return QString(":/qfextensioncamera_logo.png"); };
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


};

#endif // QFEXTENSIONCAMERA_H
