////////////////////////////////
///
///  TO DO: Reactivate the input parameter "Number of Runs" via the QSettings-Object in the settings dialog,
///  if the interface method is used => Settings Object Available
///



#ifndef ALV_AUTOCORRELATOR_H
#define ALV_AUTOCORRELATOR_H

#include <time.h>
#include <QObject>
#include <QVector>
#include<QtGlobal>
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <QtPlugin>
#include <limits>
#include <qDebug>
#include "qfextension.h"
#include "qfextensionmanager.h"
#include "alvsma5edataout.h"
#include "alvsma5edatain.h"
#include "qfextension.h"
#include "qfextensionacquisition.h"
#include "alvsettingsdialog.h"

/*!
    \defgroup qf3ext_MulticontrolStage QFExtensionLinearStage implementation for Multicontrol2000Stages
    \ingroup qf3extensionplugins
*/

/*
  This class is restricted to be used with the ALV-Correlator-5000 (Version 3.0.xx.xx);
  The ALV-Correlator software and the card driver is available for windows only.
  QT 4.8 or higher is required to use native keys for the shared memory segments.
*/


class AlvAutocorrelator : public QObject, public QFExtensionBase, public QFExtensionAcquisition //,public QFPlugin
{
        Q_OBJECT
        Q_INTERFACES(QFExtension QFExtensionAcquisition)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.AlvAutocorrelator")
#endif

    public:
        /** Default constructor */
        AlvAutocorrelator(QObject* parent=NULL);
        /** Default destructor */
        virtual ~AlvAutocorrelator();



    /////////////////////////////////////////////////////////////////////////////
    // QFExtension routines
    /////////////////////////////////////////////////////////////////////////////
        /** \copydoc QFExtension::getID() */
        virtual QString getID() const  { return QString("alv_autocorrelator5000"); };
        /** \copydoc QFExtension::getName() */
        virtual QString getName() const  { return tr("Auto correlator card: ALV-5000/E"); };
        /** \copydoc QFExtension::getDescription() */
        virtual QString getDescription() const  { return tr("Auto correlator card: ALV-5000/E"); };
        /** \copydoc QFExtension::getAuthor() */
        virtual QString getAuthor() const  { return tr("Sindbad the Sailor"); };
        /** \copydoc QFExtension::getCopyright() */
        virtual QString getCopyright() const  { return tr("GPL 3.0"); };
        /** \copydoc QFExtension::getWeblink() */
        virtual QString getWeblink() const  { return tr("http://www.dkfz.de/Macromol/quickfit/"); };
        /** \copydoc QFExtension::getIconFilename() */
        virtual QString getIconFilename() const  { return QString(":/abc.png"); };
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


        /*
          |-----------------------------------------------|
          | Public programming interface                  |
          |-----------------------------------------------|
        */

        /////////////////////////////////////////////////////
        //// QFExtensionAcquisition Acquisition routines ////
        /////////////////////////////////////////////////////



 public:
        /** \brief return the number of acquisition devices for this extension;
             As there this plugin is designed for one autocorrelator card, the
             return value will always be one in this interface */
        virtual unsigned int getAcquisitionDeviceCount();

        /*! \brief displays a modal dialog which allows the user to set the properties of a camera

            The initial settings are stored in the settings attribute. When the user clicks "OK" (or any
            equivalent action) the new settings will also be stored in this structure. See the section on
            \link QFExtensionAcquisition_settings Camera Settings \endlink for details on the \a settings argument.

            \param[in] camera the camera the settings widget should apply to
            \param[in,out] settings initially this contains the current settings which should be displayed in the
                                    dialog. After a successfull call this the new settings will be returned in this
                                    argument.
            \param[in] parent parent widget for the returned QWidget
         */
        virtual void showAcquisitionDeviceSettingsDialog(unsigned int device, QSettings& settings, QWidget* parent=NULL);

         /** \brief returns \c true if the device is connected
           As there is only one autocorrelator card, the function parameter is void of meaning */
        virtual bool isAcquisitionDeviceConnected(unsigned int device);

        /** \brief connect to the device/activate it;
        As there is only one autocorrelator card, the function parameter is void of meaning */
        virtual bool connectAcquisitionDevice(unsigned int device);

        /** \brief connect to the device/activate it
           As there is only one autocorrelator card, the function parameter is void of meaning */
        virtual void disconnectAcquisitionDevice(unsigned int device);

        /////////////// setLogging is already implemented for QFExtensionBase!
        ///             Still it has no crucial functionality as logging is also provided by the
        ///            QPluginLogTools.
        /// /** \brief set QFPluginLogServices to use (or \c NULL) for message/error logging */
        /// virtual void setLogging(QFPluginLogService* logService);
        ///////////////

         /*! \brief set camera settings from the specified QSettings object for a subsequent startAcquisition()
             \param[in] camera the camera the settings widget should apply to
             \param[in] settings the acquisition settings to use (may be created using showCameraSettingsDialog() )
             \param[in] filenamePrefix the acquisition result is stored in file(s) based on this baseFilename

             settings used;

          */
         virtual bool prepareAcquisition(unsigned int device, const QSettings& settings, QString filenamePrefix=QString("")); // TO BE IMPLEMENTED

         /*! \brief start an acquisition

             \param camera start an acquisition for this camera
             \retunr \c true on success
          */
         virtual bool startAcquisition(unsigned int device); // TO BE IMPLEMENTED
         /*! \brief cancel an acquisition started with startAcquisition()

             \param camera start an acquisition for this camera
          */
         virtual void cancelAcquisition(unsigned int device); // TO BE IMPLEMENTED
         /*! \brief check whether an acquisition started with startAcquisition() is running

             \param camera acquisition for this camera
             \param[out] percentageDone may be used (if \c !=NULL ) to return the percentage [0..100] of
                                        acquisition completion
         */
         virtual bool isAcquisitionRunning(unsigned int device, double* percentageDone=NULL);  // TO BE IMPLEMENTED


         /** \brief used by getAcquisitionDescription() to return a description of the output files*/
         struct AcquititonFileDescription {
            /** \brief name of the output file */
            QString name;
            /** \brief type of the output file (e.g. RAW16, RAW32, TIFF8, TIFF16, autocorrelations, PNG, JPEG, ...) */
            QString type;
            /** \brief a description of the file contents (human readable) */
            QString description;
         };

         /*! \brief after completion of acquisition started with startAcquisition() this returns information about the acquistion

             \param camera acquisition for this camera
             \param[out] files list of files generated by the acquisition (for details: see AcquititonFileDescription )
             \param[out] parameters describing the acquisition and the files

             For \a parameters there are some default key values that you should use to guarantee a certain degree of
             standardization, as these may be used by other plugins/applications. Note that none of these key/value pairs
             is in any way required and you can create any other key/value pairs, you like to describe your acquisition)!
             .
          */
         virtual void getAcquisitionDescription(unsigned int device, QList<QFExtensionAcquisition::AcquititonFileDescription>* files, QMap<QString, QVariant>* parameters); // TO BE IMPLEMENTED
         /*! \brief return the progress (0..100) of the current acquisition

             \param camera which camera to use for the acquisition
             \return the progress as an integer between 0 and 100
         */
         virtual int getAcquisitionProgress(unsigned int device);



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
        /** \copydoc QFExtensionLinearStage::setLogging();
             The method does not have to be called, as the logging functionality is now
             provided over static methods from QFPluginLogTools.*/
        void setLogging(QFPluginLogService* logService);

        ///////////////////////
        // properties
        ///////////////////////
protected:
        AlvSmA5EDataOut *sm_data_out;
        AlvSmA5EDataIn *sm_data_in;
        QFile *outfile;


        // TEST WIDGET and prototype for settings dialogue AUTOCORRELATOR PLUGIN
     // for testButtons

        QWidget *alvactions_dialog;
        QGridLayout *alvsettings_layout;
        QPushButton *test_reader, *settings_caller, *starter, *stopper;
        QLineEdit *crshow0, *crshow1;
        QProgressBar *progress;
        QTimer *monitor;
protected slots:
        void refresh_monitor();


        /////////////////////
        // Internal acquisition stuff
        ///////////////////
protected:
        QTimer *AcquisitionTimer;
        QTime *EstimatedTime;
        int AcquisitionRuns; // Total numbers of acquisition runs due to count rate;
        int AcquisitionRunCounter; // is increased with the start of each new measurement;
        bool AcquisitionRunning;
        int ilastProgress;
        double flastProgress;
        /*
          These list contain pointers to the blocks with the correlation data from the ALV device;
          ValCorrLengths gives the number of acquired correlation data from each run;
          ValCRLengths gives the number of CR data values in CR, and therefore delivers crucial information
          about the beginning of the CR1 value block:

          ----------------------------------
          |     CR0     |     CR1     |    | <-- 512 entries totally, with |CR0| = |CR1| and &CR1 = &CR0 + |CR1|
          ----------------------------------     If |CR0| < 256 , the last block doesn't contain valid data
          */
        QList<int32_t> *ValCorrLengths;
        QList<int32_t> *ValCRLengths;
        QList<QList<float>*> *Corr0AcquisitionResults;
        QList<QList<float>*> *Corr1AcquisitionResults;
        QList<QList<float>*> *FastModeAcquisitionResults;
        QList<QList<float>*> *CR0AcquisitionResults;
        QList<QList<float>*> *CR1AcquisitionResults;

        void prepareResultLists(); // clears the three data lists after the memory allocated to their pointers has been freed



        void startRun();
        void stopRun();
        void storeRunData();
        void writeALVOutputFile();

        void startAcquisitionRun();

        //////////////////////
        //  Additional Stuff
        //////////////////////

        QFPluginLogService* logService;


public slots:

    //void startAlv();
    bool prepare_shared_memory();
    // test
    void call_show_settings_dialog();
    void test_action(); // read stuff from alv

   // void triggerALVSoftware(); // set the "activation byte"



    void call_startAcquistion();
    void call_stopAcquistion();


    //    void confirmSettings();
    //void transferParametersOnly();


protected slots:
    void finishAcquisitionStep();
//    void increaseAcquisitionRunCounter();



        /*
          |-----------------------------------------------|
          | internal functions to talk to the AVL program |
          |-----------------------------------------------|
         */

signals:
    //void restore_control();


};

#endif // ALV_AUTOCORRELATOR_H
