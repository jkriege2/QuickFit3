#ifndef QFEB040FFMCONTROL_H
#define QFEB040FFMCONTROL_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include "ffmmainwindow.h"
#include <QPointer>
#include "qtlogfile.h"
#include "qfextensionmanager.h"
//#include "../plugins/multicontrol_stage/mcstagecontroller.h"
#include <QtGui>
#include <QtPlugin>
#include <iostream>
/*!
    \defgroup qf3ext_b040_ffmcontrol QFExtension implementation
    \ingroup qf3extensionplugins
*/

/*! \brief QFExtension implementation
    \ingroup qf3ext_b040_ffmcontrol
 */
class QFEB040FFMControl : public QObject, public QFExtensionBase {
        Q_OBJECT
        Q_INTERFACES(QFExtension)
public:
        /** Default constructor */
    QFEB040FFMControl(QObject* parent=NULL);
        /** Default destructor */
    virtual ~QFEB040FFMControl();


    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
    /** \copydoc QFExtension::getID() */
    virtual QString getID() const  { return QString("b040_ffmcontrol"); };
    /** \copydoc QFExtension::getName() */
    virtual QString getName() const  { return tr( "QFEB040FFM" ); };
        /** \copydoc QFExtension::getDescription() */
    virtual QString getDescription() const  { return tr("Control software for the FCS Microscope"); };
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

    // !!! NEW STUFF !!!
      //MulticontrolStage* get_stage();

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
    QPointer<ffmMainWindow> mw;
    bool plugin_open;
    //MulticontrolStage* stage;
protected slots:
    /** \brief target, used in example code in initExtension() */
        void startPlugin();
};

#endif // QFEB040FFMCONTROL_H
