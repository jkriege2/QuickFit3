//////////////
///
/// => TO DO: Making the stage threadsafe instead of polling
///


#ifndef MULTICONTROL_STAGE_H
#define MULTICONTROL_STAGE_H

#include <time.h>
#include <QObject>
#include <QVector>
#include "qfextension.h"
#include "qfextensionlinearstage.h"
#include "../../../../../LIB/trunk/jkserialconnection.h"
#include "mcstagecontroller.h"
#include "multicontrol_settingsdialog.h"

/*!
    \defgroup qf3ext_MulticontrolStage QFExtensionLinearStage implementation for Multicontrol2000Stages
    \ingroup qf3extensionplugins
*/

class MulticontrolStage : public QObject, public QFExtensionBase, public QFExtensionLinearStage
{
        Q_OBJECT
        Q_INTERFACES(QFExtension QFExtensionLinearStage)// QFExtensionLinearStage)
    public:
        /** Default constructor */
        MulticontrolStage(QObject* parent=NULL);
        /** Default destructor */
        virtual ~MulticontrolStage();

    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("stage_mc2000"); };
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("Motion Stage: Multicontrol 2000"); };
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("Motion Stage: Multicontrol 2000"); };
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Sindbad the Sailor"); };
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("GPL 3.0"); };
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); };
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/multicontrol2000_logo.png"); };
        /** \copydoc QFExtension::deinit() */
        virtual void deinit();
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };

    protected:


        /* TO BE DONE */
        /** \copydoc QFExtensionBase::projectChanged() */
        void projectChanged(QFProject* oldProject, QFProject* project);   //TO DO
        /** \copydoc QFExtensionBase::initExtension() */
        void initExtension();                                             //TO DO
        /** \copydoc QFExtensionBase::loadSettings() */
        void loadSettings(ProgramOptions* settings);                      //TO DO
        /** \copydoc QFExtensionBase::storeSettings() */
        void storeSettings(ProgramOptions* settings);                     //TO DO


    /////////////////////////////////////////////////////////////////////////////
    // Public Interface for using the MC 2000
    // QFExtensionLinearStage routines
    /////////////////////////////////////////////////////////////////////////////

    public:
        /** \copydoc QFExtensionLinearStage::) */
        virtual unsigned int getAxisCount();

        /** \copydoc QFExtensionLinearStage::showSettingsDialog() */
        virtual void showSettingsDialog(unsigned int axis, QWidget* parent=NULL); /////// TO DO
        /** \copydoc QFExtensionLinearStage::isConnected() */
        /* axis = 1 => ask for x-axis
                = 2 => ask for x- and y-axis
                = 3 => ask for x-, y- and z-axis */
        virtual bool isConnected(unsigned int axis);



        /** \copydoc QFExtensionLinearStage::connectDevice() */
        // PARAMETER UNSIGNED INT AXIS IS NOT USED; EXISTS ONLY DUE TO REASONS FO CONSISTENCY WITH THE INTERFACE
        virtual void connectDevice(unsigned int axis);


        /** \copydoc QFExtensionLinearStage::disconnectDevice() */
        virtual void disconnectDevice(unsigned int axis);
        /** \copydoc QFExtensionLinearStage::setLogging() */
        virtual void setLogging(QFPluginLogService* logService);

        /** \copydoc QFExtensionLinearStage::setJoystickActive() */
        // first parameter is NOT USED!
        virtual void setJoystickActive(unsigned int axis, bool enabled, double maxVelocity = 1000000);
        /** \copydoc QFExtensionLinearStage::isJoystickActive() */
        // parameter is NOT USED!!
        virtual bool isJoystickActive(unsigned int axis);

        /** \copydoc QFExtensionLinearStage::getAxisState() */
        virtual AxisState getAxisState(unsigned int axis);



        /** \copydoc QFExtensionLinearStage::stop() */
        virtual void stop(unsigned int axis); // NOT USED, IMPLEMENTATION IS VOID


        /** \copydoc QFExtensionLinearStage::getPosition() */
        virtual double getPosition(unsigned int axis);
        /** \copydoc QFExtensionLinearStage::getSpeed() */

        // function not implemented yet;
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


        //bool checkComConnected();

        //void selectAxis(int i);
        /** \brief send a command to the Mercury controller (this automatically adds a command terminating character (carriage return) */
        void sendCommand(std::string command);

        /** \brief send a command to the Mercury controller (this automatically adds a command terminating character (carriage return)
         *         and returns the result (the standard finishing sequence CR LF ETX will be cut from the string) */
        std::string queryCommand(std::string command);
        void checkComError();


        /////////////////////////////
        // Additional stuff
        /////////////////////////////
        uint8_t getDimensions();

protected slots:
        void reconfigureStage();

    protected:
        QSettings *stageSettings;
        MulticontrolSettingsDialog *settingsDialog;
        QFPluginLogService *logService;
        //QAction *actCalibrateJoysticks;
        MCStageController *stagecontroller;

        /*
          !!!!!!! NEW STUFF !!!!!!!
        */

/*
        struct AxisDescription {
            ** \brief ID of the Mercury C-863 controller for the axis
             *
             * This contains the address character (!!!) of the controller in a daisy chain. The character is either
             * \c '0' ... \c '9' or \c 'A' ... \c 'F'
             *
             QChar ID;

    //         QFExtensionLinearStage::AxisState state;


             ** \brief indicates whether the joystick is enabled or not
             bool joystickEnabled;

             double velocity;

             QString name;
        };
*/
        QChar currentID;
//      QVector<AxisDescription> axes;

        /** \brief tty port */
        QString COMPort;

        QMutex* mutexSerial;

        float maxAcceleration;
        float maxVelocity;
        float maxJoySpeed;

};

#endif // MULTICONTROL_STAGE_H
