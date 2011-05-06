#ifndef STAGE_PI_H
#define STAGE_PI_H

#include <time.h>
#include <QObject>
#include <QVector>
#include "qfextension.h"
#include "../interfaces/qfextensionlinearstage.h"
#include "../../../../../LIB/trunk/jkserialconnection.h"

/*!
    \defgroup qf3ext_StagePI QFExtensionLinearStage implementation for PI mercury stages
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtensionLinearStage implementation for PI mercury stages
    \ingroup qf3ext_StagePI
 */
class QFExtensionLinearStagePI : public QObject, public QFExtensionBase, public QFExtensionLinearStage {
        Q_OBJECT
        Q_INTERFACES(QFExtension QFExtensionLinearStage)
    public:
        /** Default constructor */
        QFExtensionLinearStagePI(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFExtensionLinearStagePI();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("stage_pimercury"); };
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("Motion Stage: PI Mercury 863"); };
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("Motion Stage: PI Mercury 863"); };
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Jan Krieger"); };
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("(c) 2011 by Jan Krieger"); };
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr(""); };
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/stage_pi_logo.png"); };
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
        virtual void showSettingsDialog(unsigned int axis, QWidget* parent=NULL);
        /** \copydoc QFExtensionLinearStage::isConnected() */
        virtual bool isConnected(unsigned int axis);
        /** \copydoc QFExtensionLinearStage::connectDevice() */
        virtual void connectDevice(unsigned int axis);
        /** \copydoc QFExtensionLinearStage::disconnectDevice() */
        virtual void disconnectDevice(unsigned int axis);
        /** \copydoc QFExtensionLinearStage::setLogging() */
        virtual void setLogging(QFPluginLogService* logService);

        /** \copydoc QFExtensionLinearStage::setJoystickActive() */
        virtual void setJoystickActive(unsigned int axis, bool enabled, double maxVelocity=100000);
        /** \copydoc QFExtensionLinearStage::isJoystickActive() */
        virtual bool isJoystickActive(unsigned int axis);

        /** \copydoc QFExtensionLinearStage::getAxisState() */
        virtual AxisState getAxisState(unsigned int axis);

        /** \copydoc QFExtensionLinearStage::stop() */
        virtual void stop(unsigned int axis);

        /** \copydoc QFExtensionLinearStage::getPosition() */
        virtual double getPosition(unsigned int axis);
        /** \copydoc QFExtensionLinearStage::getSpeed() */
        virtual double getSpeed(unsigned int axis);

        /** \copydoc QFExtensionLinearStage::move() */
        virtual void move(unsigned int axis, double newPosition);




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

        struct AxisDescription {
            /** \brief ID of the Mercury C-863 controller for the axis
             *
             * This contains the address character (!!!) of the controller in a daisy chain. The character is either
             * \c '0' ... \c '9' or \c 'A' ... \c 'F'
             */
             QChar ID;

             QFExtensionLinearStage::AxisState state;
             /** \brief indicates whether the joystick is enabled or not */
             bool joystickEnabled;

             double velocity;

             QString name;
        };

        QChar currentID;

        QVector<AxisDescription> axes;

        /** \brief tty port */
        QString COMPort;
        /** \brief speed of com port */
        int COMPortSpeed;
        /** \brief serial connection object */
        JKSerialConnection com;
        /** \brief settings of P-Term */
        unsigned int PTerm;
        /** \brief settings of i-Term */
        unsigned int iTerm;
        /** \brief settings of D-Term */
        unsigned int DTerm;
        /** \brief settings of i-Limit */
        unsigned int iLimit;
        /** \brief settings of acceleration */
        double acceleration;
        /** \brief the initial velocity of all axes */
        double initVelocity;
        /** \brief settings of max. velocity */
        double maxVelocity;

        bool checkComConnected();

        void selectAxis(int i);
        /** \brief send a command to the Mercury controller (this automatically adds a command terminating character (carriage return) */
        void sendCommand(std::string command);

        /** \brief send a command to the Mercury controller (this automatically adds a command terminating character (carriage return)
         *         and returns the result (the standard finishing sequence CR LF ETX will be cut from the string) */
        std::string queryCommand(std::string command);
        /** \brief this factor is used to get the control electronics position from the position in micron, given in microns/unit */
        double lengthFactor;

        /** \brief this factor is used to get the control electronics velocity from the velocity in micron/sec, given in (micron/sec)/unit */
        double velocityFactor;

        /** \brief this factor is used to get the control electronics acceleration from the acceleration in micron/sec^2, given in (micron/sec^2)/unit */
        double accelerationFactor;

        void checkComError();
};

#endif // STAGE_PI_H
