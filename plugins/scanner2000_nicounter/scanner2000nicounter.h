#ifndef SCANNER2000NICOUNTER_H
#define SCANNER2000NICOUNTER_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include <QPointer>
#include "qtlogfile.h"
#include "qfextensionmanager.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
#include "scancontroller.h"
#include "nidaq6602.h"

/*!
    \defgroup qf3ext_b040_ffmcontrol QFExtension implementation
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtension implementation
    \ingroup qf3ext_b040_ffmcontrol
 */
class Scanner2000NICounter : public QObject, public QFExtensionBase {
        Q_OBJECT
        Q_INTERFACES(QFExtension)
public:
        /** Default constructor */
    Scanner2000NICounter(QObject* parent=NULL);
        /** Default destructor */
    virtual ~Scanner2000NICounter();



public:

    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
    /** \copydoc QFExtension::getID() */
    virtual QString getID() const  { return QString("scanner2000_nicounter"); };
    /** \copydoc QFExtension::getName() */
    virtual QString getName() const  { return tr( "scannerCounter" ); };
        /** \copydoc QFExtension::getDescription() */
    virtual QString getDescription() const  { return tr("Plugin for the control of the combined use of the scanner 2000 and the NI counter card"); };
    /** \copydoc QFExtension::getAuthor() */
    virtual QString getAuthor() const  { return tr("Sindbad the Sailor"); };
    /** \copydoc QFExtension::getCopyright() */
    virtual QString getCopyright() const  { return tr( "GPL 3.0" ); };
        /** \copydoc QFExtension::getWeblink() */
    virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); };
    /** \copydoc QFExtension::getIconFilename() */
    virtual QString getIconFilename() const  { return QString(":/b040_ffmcontrol/qfeb040ffmcontrol.png"); };
    /** \brief plugin version  */
    virtual void getVersion(int& major, int& minor) const {
        major=1;
        minor=0;
    };
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

    ScanController *scancontroller;
    NIDAQ6602 *counterCard;

protected slots:
    /** \brief target, used in example code in initExtension() */
    void startPlugin();

private:
    void createTestWidget();
    QWidget *testwidget;
    QGridLayout *testLayout;
    QPushButton *readLineButton, *readPointButton, *confStaticButton, *confLineButton, *statTaskStarter, *lineTaskStarter;
    //*measTaskStarter, *measTaskStopper, *triggerTaskStarter, *triggerTaskStopper, *tasksStarter,
    QSpinBox *measnumBox;
    //QCheckBox *autoStart;
    //QTimer *timer;
    // test stuff

    QSpinBox *scanx_axis, *scany_axis; //test
    QPushButton *scaninitButton; //test
    QPushButton *scanmoveButton; //test
    QPushButton *scanconnectButton;
    QPushButton *scandisconnectButton;
    QPushButton *triggerMoveButton;
    QSpinBox *triggerChannelBox;
    QDoubleSpinBox *moveTimeBox, *measTimeBox, *pointMeasTimeBox;


 //// TEST INTERFACE FOR NI-DAQ
private slots:
    //// TEST WIDGET



    void connectScanner();
    void disconnectScanner();
    void initScanner();
    void callMoveScanner();
    void callTriggerMove();
private:
    void moveScanner(int16_t newxpos, int16_t newypos, float movetime);
    void triggerMove(uint8_t triggerChannel,int16_t newxpos, int16_t newypos, float movetime);


    ////TEST INTERFACE FOR NI-DAQ
private slots:
    void testLineReadfunction();
    void testPointReadfunction();
    void configureLineMode();
    void configureStaticMode();
    void pointMeas();
    void lineMeas();



    /*
    void configureLineMode(uint32_t lineLength, uint32_t newLineMeasTimePerPixel);
    void configureStaticMode();
    bool getLineMode();  // true  => configured for LineMode | false => configured for StaticMode
    void startMeasInLineMode(bool *ok); // start measurement task; actual measurement will be triggered by
    void startMeasStaticMode(bool *ok);
    QList <QList<float> > getLineData(bool *ok); // first list: data form measTask1, second list from measTask2; rates in photons per ms
    QList<float> getStaticCountRate(bool *ok);   //
    */

        // END TEST WIDGET
private:

    //

};

#endif //SCANNER2000NICOUNTER_H
