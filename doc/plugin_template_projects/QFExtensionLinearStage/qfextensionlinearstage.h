#ifndef QFEXTENSIONLINEARSTAGEIMPLEMENTATION_H
#define QFEXTENSIONLINEARSTAGEIMPLEMENTATION_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include "../interfaces/qfextensionlinearstage.h"

/*!
    \defgroup qf3ext_GROUPNAME QFExtensionLinearStage implementation
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtensionLinearStage implementation
    \ingroup qf3ext_GROUPNAME
 */
class QFExtensionLinearStageImplementation : public QObject, public QFExtensionBase, public QFExtensionLinearStage {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFExtensionLinearStage)
    public:
        /** Default constructor */
        QFExtensionLinearStageImplementation(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFExtensionLinearStageImplementation();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString(/* internal ID for this stage */); };
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
    // QFExtensionLinearStage routines
    /////////////////////////////////////////////////////////////////////////////
    public:
        /** \copydoc QFExtensionLinearStage::) */
		virtual unsigned int getAxisCount();


        /** \copydoc QFExtensionLinearStage::showSettingsDialog() */
        virtual void showSettingsDialog(QWidget* parent=NULL);
        /** \copydoc QFExtensionLinearStage::isConnected() */
        virtual bool isConnected(unsigned int axis);
        /** \copydoc QFExtensionLinearStage::connectDevice() */
        virtual void connectDevice(unsigned int axis);
        /** \copydoc QFExtensionLinearStage::disconnectDevice() */
        virtual void disconnectDevice(unsigned int axis);
        /** \copydoc QFExtensionLinearStage::setLogging() */
        virtual void setLogging(QFPluginLogService* logService);

        /** \copydoc QFExtensionLinearStage::setJoystickActive() */
        virtual void setJoystickActive(unsigned int axis, bool enabled);
        /** \copydoc QFExtensionLinearStage::isJoystickActive() */
        virtual bool isJoystickActive(unsigned int axis);

        /** \copydoc QFExtensionLinearStage::getAxisState() */
        virtual AxisState getAxisState(unsigned int axis);

        /** \copydoc QFExtensionLinearStage::stop() */
        virtual void stop(unsigned int axis);

        /** \copydoc QFExtensionLinearStage::getPosition() */
        virtual double getPosition(unsigned int axis);

        /** \copydoc QFExtensionLinearStage::move() */
        virtual void move(double newPosition);
		
		
		


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

};

#endif // QFEXTENSIONCAMERA_H
