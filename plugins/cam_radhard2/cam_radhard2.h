/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CAM_RADHARD2_H
#define CAM_RADHARD2_H

#include <time.h>
#include <QObject>
#include "qfextension.h"
#include "qfextensioncamera.h"
#include "radhard2lib/radhard2.h"
#include "radhard2lib/radhard2lib.h"
#include "../base_classes/radhard2flash.h"
#include "highrestimer.h"
#include "qenhancedlineedit.h"
#include "jkstyledbutton.h"
#include "qfradhard2flashtool.h"



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
        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };

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
        virtual unsigned int getCameraCount() const;
        /** \copydoc QFExtensionCamera::showCameraSettingsDialog() */
        virtual void showCameraSettingsDialog(unsigned int camera, QSettings& settings, QWidget* parent=NULL);
        /** \copydoc QFExtensionCamera::useCameraSettings() */
        virtual void useCameraSettings(unsigned int camera, const QSettings& settings);
        /** \copydoc QFExtensionCamera::getImageWidth() */
        virtual int getCameraImageWidth(unsigned int camera);
        /** \copydoc QFExtensionCamera::getImageHeight() */
        virtual int getCameraImageHeight(unsigned int camera);
        /** \copydoc QFExtensionCamera::isConnected() */
        virtual bool isCameraConnected(unsigned int camera);
        /** \copydoc QFExtensionCamera::acquire() */
        virtual bool acquireOnCamera(unsigned int camera, uint32_t* data, uint64_t* timestamp=NULL, QMap<QString, QVariant>* parameters=NULL);
        /** \copydoc QFExtensionCamera::connectDevice() */
        virtual bool connectCameraDevice(unsigned int camera);
        /** \copydoc QFExtensionCamera::disconnectDevice() */
        virtual void disconnectCameraDevice(unsigned int camera);
        /** \copydoc QFExtensionCamera::getExposureTime() */
        virtual double getCameraExposureTime(unsigned int camera);
        /** \copydoc QFExtensionCamera::setLogging() */
        virtual void setCameraLogging(QFPluginLogService* logService) { this->logService=logService; };
        /** \copydoc QFExtensionCamera::getPixelWidth() */
        virtual double getCameraPixelWidth(unsigned int camera);
        /** \copydoc QFExtensionCamera::getPixelHeight() */
        virtual double getCameraPixelHeight(unsigned int camera);
        /** \copydoc QFExtensionCamera::getCameraName() */
        virtual QString getCameraName(unsigned int camera);
        /** \copydoc QFExtensionCamera::getCameraSensorName() */
        virtual QString getCameraSensorName(unsigned int camera);

        /** \copydoc QFExtensionCamera::prepareAcquisition() */
        virtual bool prepareCameraAcquisition(unsigned int camera, const QSettings& settings, QString filenamePrefix=QString(""));
        /** \copydoc QFExtensionCamera::startAcquisition() */
        virtual bool startCameraAcquisition(unsigned int camera);
        /** \copydoc QFExtensionCamera::cancelAcquisition() */
        virtual void cancelCameraAcquisition(unsigned int camera);
        /** \copydoc QFExtensionCamera::isAcquisitionRunning() */
        virtual bool isCameraAcquisitionRunning(unsigned int camera, double* percentageDone=NULL);
        /** \copydoc QFExtensionCamera::getAcquisitionDescription() */
        virtual void getCameraAcquisitionDescription(unsigned int camera, QList<QFExtensionCamera::CameraAcquititonFileDescription>* files, QMap<QString, QVariant>* parameters);
        /** \copydoc QFExtensionCamera::getAcquisitionPreview() */
        virtual bool getCameraAcquisitionPreview(unsigned int camera, uint32_t* data);
        /** \copydoc QFExtensionCamera::getAcquisitionProgress() */
        virtual int getCameraAcquisitionProgress(unsigned int camera);


        /** \copydoc QFExtensionCamera::isCameraSettingChangable() */
        virtual bool isCameraSettingChangable(QFExtensionCamera::CameraSetting which) ;
        /** \copydoc QFExtensionCamera::changeCameraSetting() */
        virtual void changeCameraSetting(QSettings& settings, QFExtensionCamera::CameraSetting which, QVariant value);
        /** \copydoc QFExtensionCamera::getCameraSetting() */
        virtual QVariant getCameraSetting(QSettings& settings, QFExtensionCamera::CameraSetting which) ;
        /** \copydoc QFExtensionCamera::getCurrentCameraSetting() */
        virtual QVariant getCameraCurrentSetting(int camera, CameraSetting which);

        /** \brief returns \c true if the given CameraSetting is changable by changeCameraSetting() */
        inline virtual bool isCameraSettingChangable(const QString& which)  { return isCameraSettingChangable(QStringToSetting(which)); }
        /*! \brief change the given CameraSetting in the given QSettings object

            \note <b>this will change the contents of your QSettings object, so possibly this changed version is also written back to the harddisk!!!</b>
         */
        inline virtual void changeCameraSetting(QSettings& settings, const QString& which, QVariant value)  {  changeCameraSetting(settings, QStringToSetting(which), value); }
        /** \brief extract the given CameraSetting from the given QSettings object */
        inline virtual QVariant getCameraSetting(QSettings& settings, const QString& which)  { return getCameraSetting(settings, QStringToSetting(which)); }
        /** \brief extract the given CameraSetting from the given camera */
        inline virtual QVariant getCameraCurrentSetting(int camera, const QString& which)  { return getCameraCurrentSetting(camera, QStringToSetting(which)); }


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
        /*! \brief program the given \a bitfile into the FPGA (m for master or s for slave)

            \param bitfile which file to flash
            \param fpga \c 'm' to program master FPGA or \c 's' to program slave FPGA
            \param[out] messageOut contains the messages output by the flashing procedure
            \param retries number of retries when programming was not successfull
            \param retryDelayMS delay between two retries in milliseconds
            \return \c true on successfull programming
        */
        bool flashFPGA(QString autobitfile, char fpga, QString& messageOut, int retries=10, int retryDelayMS=500);
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
        int retries;
        int retryDelay;

        QEnhancedLineEdit* edtBitfile;
        QTextEdit* edtFlashSuccess;
        QSpinBox* spIterations;
        QSpinBox* spDivider;

        double calcExposureTime(double iterations, double divider);

};

#endif // CAM_RADHARD2_H
