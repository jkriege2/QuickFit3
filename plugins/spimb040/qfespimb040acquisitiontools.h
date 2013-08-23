#ifndef QFESPIMB040ACQUISITIONTOOLS_H
#define QFESPIMB040ACQUISITIONTOOLS_H
#include <QString>
#include "../interfaces/qfextensioncamera.h"
#include "../interfaces/qfextensionlinearstage.h"
#include "qfextension.h"
#include "qfespimb040opticssetup.h"
#include <QMap>
#include <QVariant>

class QFESPIMB040AcquisitionTools {
    public:
        ~QFESPIMB040AcquisitionTools() {};
        /*! \brief save an image from the given camera as a 16-bit TIFF image into \a filename

            \note This function configures the camera with the preview settings, acquires ONE frame and saves it to the given file.
                  If the gray value range of the frame exceeds 16-bit it is compressed to 16-bit (i.e. gray_max -> 65535) and a 32-bit
                  version is saved!

            \param extension QFExtension of the used camera
            \param ecamera QFExtensionCamera of the used camera
            \param camera device index of the camera inside ecamera
            \param previewSettingsFilename the preview settings file used for this acquisition
            \param filename the file to save to (if a 32-version is created, its filename is the same as \A filename, but with the extension \c .32.tiff !).
            \param[out] filename32 may be used to return the filename of the potentially created 32-bit TIFF (is set to an empty string, if no 32-bit TIFF was created)
            \return \c true on success
         */
        virtual bool savePreview(QFExtension* extension, QFExtensionCamera* ecamera, int camera, const QString& previewSettingsFilename, const QString& filename, QString* filename32, QMap<QString, QVariant>* acquisitionDescription=NULL, const QString& acquisitionDescriptionPrefix=QString(""), bool mainShutterOpenOnlyForAcquisition=false)=0;

        virtual bool prepareCamera(int num, int camera, QFExtensionCamera *cam, const QString& acquisitionSettingsFilename, int &width, int &height, uint32_t **buffer, const QString& acquisitionTitle=QString("B040 SPIM Acquisition"))=0;

        virtual bool connectStageForAcquisition(QFExtensionLinearStage* stage, int stageAxis, double& stageInitialPos, const QString& acquisitionTitle=QString("B040 SPIM Acquisition"), int stageNum=1)=0;
        /*! \brief set a lighpath and acquire one image with the given camera and settings.

            \note THe lightpath is not altered, if lightpathFilename is left empty.
         */
        virtual bool acquireImageWithLightpath(const QString& lightpathFilename, const QString& lightpathName, QFExtension* extension1, QFExtensionCamera* ecamera1, int camera1, const QString& previewSettingsFilename1, const QString& outputFilename, const QString& imageID, const QString& imageDescription, QList<QFExtensionCamera::CameraAcquititonFileDescription>& moreFiles1, QMap<QString, QVariant>& acquisitionDescription1, bool mainShutterOpenOnlyForAcquisition=false)=0;

        /*! \brief do an acquisition on one or two cameras

            \note if \a frames \t <=0, the number of frames will NOT be changed, otherwise the number of frames will be set accordingly (if the driver allows it!)
            \note if \a measured is \c NULL no measurements will be stored during acquisition. otherwise they will be stored to this map.
         */
        virtual bool acquireSeries(const QString& lightpathName, const QString& imageID, const QString& imageDescription, bool useCam1, QFExtension* extension1, QFExtensionCamera* ecamera1, int camera1, const QString& acquisitionPrefix1, const QString& acquisitionSettingsFilename1, QMap<QString, QVariant>& acquisitionDescription1, QList<QFExtensionCamera::CameraAcquititonFileDescription>& moreFiles1, bool useCam2, QFExtension* extension2, QFExtensionCamera* ecamera2, int camera2, const QString& acquisitionPrefix2, const QString& acquisitionSettingsFilename2, QMap<QString, QVariant>& acquisitionDescription2, QList<QFExtensionCamera::CameraAcquititonFileDescription>& moreFiles2, const QMap<QFExtensionCamera::CameraSetting, QVariant>& camsettings1, const QMap<QFExtensionCamera::CameraSetting, QVariant>& camsettings2, QList<QFESPIMB040OpticsSetupBase::measuredValues>* measured=NULL, QProgressListDialog* progress=NULL, bool* userCanceled=NULL)=0;
        virtual bool acquireSeries(const QString& lightpathName, const QString& imageID, const QString& imageDescription, bool useCam1, QFExtension* extension1, QFExtensionCamera* ecamera1, int camera1, const QString& acquisitionPrefix1, QSettings& acquisitionSettings1, QMap<QString, QVariant>& acquisitionDescription1, QList<QFExtensionCamera::CameraAcquititonFileDescription>& moreFiles1, bool useCam2, QFExtension* extension2, QFExtensionCamera* ecamera2, int camera2, const QString& acquisitionPrefix2, QSettings& acquisitionSettings2, QMap<QString, QVariant>& acquisitionDescription2, QList<QFExtensionCamera::CameraAcquititonFileDescription>& moreFiles2, QList<QFESPIMB040OpticsSetupBase::measuredValues>* measured=NULL, QProgressListDialog* progress=NULL, bool* userCanceled=NULL)=0;

        virtual void getAdditionalCameraSettings(QFExtensionCamera* ecamera, int camera, const QString& prefix, QMap<QString, QVariant>& acquisitionDescription)=0;


        /*! \brief save a description of an acquisition to a QSettings object

            Settings will be stored to the file \c <filenamePrefix>_configuration.ini .

            \param use_cam use camera 0, 1, ... in the setup
            \param extension QFExtension object describing the used camera (must be castable to QFExtensionCamera)
            \param camera =0,1 which camera was used?
            \param filenamePrefix prefix for the acquisition output files
            \param acquisitionDescription additional key-value-pairs describing the acquisition mode (will be stored under \c [acquisition] heading)
            \param moreFiles a list of additionally created files
            \return filename of the settings file
        */
        virtual QString saveAcquisitionDescription(int use_cam, QFExtension* extension, QFExtensionCamera* ecamera, int camera, const QString& filenamePrefix, const QMap<QString, QVariant>& acquisitionDescription, const QList<QFExtensionCamera::CameraAcquititonFileDescription>& moreFiles, QDateTime startDateTime, bool getAcquisitionSettings=true)=0;

        /*! \brief save a description of an acquisition in preview mode to a QSettings object

            Settings will be stored to the file \c <filenamePrefix>_configuration.ini .

            \param use_cam use camera 0, 1, ... in the setup
            \param extension QFExtension object describing the used camera (must be castable to QFExtensionCamera)
            \param camera =0,1 which camera was used?
            \param filenamePrefix prefix for the acquisition output files
            \param acquisitionDescription additional key-value-pairs describing the acquisition mode (will be stored under \c [acquisition] heading)
            \param files files that have been stored (see  QFExtensionCamera::AcquititonFileDescription)
            \return filename of the settings file
        */
        virtual QString savePreviewDescription(int use_cam, QFExtension* extension, QFExtensionCamera* ecamera, int camera, const QString& filenamePrefix, const QMap<QString, QVariant>& acquisitionDescription, const QList<QFExtensionCamera::CameraAcquititonFileDescription>& files, QDateTime startDateTime, const QString& lightpathPrefix=QString(""), const QString& prefix=QString(""))=0;
        virtual QString saveMeasuredData(const QString &filenamePrefix, const QList<QFESPIMB040OpticsSetupBase::measuredValues> &data)=0;

};

Q_DECLARE_INTERFACE(QFESPIMB040AcquisitionTools,
                     "www.dkfz.de.b040.quickfit3.b040spim.QFESPIMB040AcquisitionTools/1.0")

#endif // QFESPIMB040ACQUISITIONTOOLS_H
