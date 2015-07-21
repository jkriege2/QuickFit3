#ifndef QFRDRIMAGINGFCSWIZARD_H
#define QFRDRIMAGINGFCSWIZARD_H

#include <QPointer>
#include <QWidget>
#include "qfwizard.h"
#include "qfselectfileslistwidget.h"
#include "qfimageplot.h"
#include "qfrdrimagingfcscorrelationjobthread.h"
#include "qspecialtoolbutton.h"
#include "qfenhancedlineedit.h"
#include "qfstyledbutton.h"
#include "qfpixelsizeedit.h"
#include "qfframerangeedit.h"
#include "qfrdrimagingfcscorrelationdialog.h"
#include "qfrdrimagingfcspixelfromobjective.h"
#include "qfevaluationitemfactory.h"
#include "qfcroppixelsedit.h"
#include "qfrdrimagingfcswizardcorrelationprogress.h"

class QFRDRImagingFCSWizard_BackgroundIsValid; // forward
class QFRDRImagingFCSWizard_ImagestackIsValid;
class QFRDRImagingFCSWizard_BackgroundNextId;
class QFRDRImagingFCSWizard_ProcessNextId;

class QFRDRImagingFCSWizard : public QFWizard {
        Q_OBJECT
    public:
        explicit QFRDRImagingFCSWizard(bool is_project=false, QWidget* parent=NULL);
        ~QFRDRImagingFCSWizard();
        enum Pages {
            InitPage,
            FileSelectionPage,
            ImagePage,
            MicroscopyPage,
            BackgroundPage,
            CalibrationPage,
            CropAndBinPage,
            CorrelationPage,
            ProcessCorrelationPage,
            LastPage,
            LastPageCalibration,
        };

    protected slots:
        void selectFileClicked();
        void edtFilenameTextChanged(const QString& filename);
        void initImagePreview();
        void finishedImageProps();
        void initFileSelection();
        void finishedIntro();
        void backgroundModeChanged(int mode);
        void calcPixelSize();
        void calibrationRegionChanged(int region);
        void calibrationCropValuesChanged();
        void cropRegionChanged(int region);
        void cropValuesChanged();
        void microscopyChoosen();
        void calibrationSetupFinished();
        void calibWxyTestChanged();
        void cropSetupFinished();
        void correlationValuesChanged();
    protected:

        QFRadioButtonListWizardPage* wizIntro;
        QFFormWizardPage* wizSelfiles;
        QFFormWizardPage* wizMicroscopy;
        QDoubleSpinBox* spinWz;
        QComboBox* cmbMicroscopy;
        QLabel* labWz;
        QDoubleSpinBox* spinWxy;
        QLabel* labWxy;

        QFImagePlotWizardPage* wizImageProps;
        QStringList imageFilters;
        QStringList imageFormatNames;
        QStringList imageFormatIDs;
        bool isProject;
        QFPixelSizeEdit* widPixSize;
        QPushButton* btnPixSize;
        QFFrameRangeEdit* widFrameRange;
        QDoubleSpinBox* spinFrametime;
        QFEnhancedLineEdit* edtFilename;
        QFStyledButton* btnFilename;
        QFEnhancedComboBox* cmbDualView;
        QFEnhancedComboBox* cmbFileformat;
        QFPluginServices* pluginServices;
        QLabel* labFileError;



        QFFormWizardPage* wizBackgroundAndBleach;
        QFEnhancedLineEdit* edtBackgroundFilename;
        QFStyledButton* btnBackgroundFilename;
        QComboBox* cmbBackgroundMode;
        QSpinBox* spinBackgroundOffset;
        QLabel* labBackgroundError;
        QLabel* labImageProps;
        QComboBox* cmbBleachCorrection;

        QFGridWizardPage* wizProcessJobs;
        QFRDRImagingFCSWizardCorrelationProgress* widProcess;

        QFFormWizardPage* wizFinalizePage;
        QLabel* labFinal;
        QPointer<QCheckBox> chkLastImFCSFit1;
        QPointer<QCheckBox> chkLastImFCCSFit;
        QPointer<QFEnhancedComboBox> cmbImFCSFitMode;

        QFFormWizardPage* wizFinalizePageCalibration;

        QFImagePlotWizardPage* wizCalibration;
        QComboBox* cmbCalibRegion;
        QSpinBox* spinCalibrationCenterSize;
        QFCropPixelsEdit* widCropCalibration;
        QDoubleSpinBox* spinCalibExectedWxy;
        QSpinBox* spinCalibExpectedWxyTests;
        QDoubleSpinBox* spinCalibExpectedWxySteps;
        QLabel* labCalibExpectedWxyTests;
        QSpinBox* spinCalibBinMax;
        QLabel* labCalibBinMax;



        QFImagePlotWizardPage* wizCropAndBin;
        QComboBox* cmbCropRegion;
        QSpinBox* spinCropCenterSize;
        QFCropPixelsEdit* widCrop;
        QSpinBox* spinBinning;
        QLabel* labBinning;

        QFFormWizardPage* wizCorrelation;
        QCheckBox* chkACF;
        QCheckBox* chk2ColorFCCS;
        QComboBox* cmb2PixelFCCS;
        QDoubleSpinBox* spinTauMax;
        QSpinBox* spinSegments;
        QLabel* labSegments;

        int channels;
        int frame_count_io;
        qint64 filesize_io;
        double frametime_io;
        double baseline_offset_io;
        QString backgroundF_io;
        int background_width;
        int background_height;
        int background_count;
        double pixel_width_io;
        double pixel_height_io;
        int dualViewMode_io;
        int image_width_io;
        int image_height_io;
        QString inputconfigfile_io;
        bool hasPixel_io;
        double* frame_data_io;
        QList<double> calibWxyTest;
        friend class QFRDRImagingFCSWizard_BackgroundIsValid;
        friend class QFRDRImagingFCSWizard_ImagestackIsValid;
        friend class QFRDRImagingFCSWizard_BackgroundNextId;
        friend class QFRDRImagingFCSWizard_ProcessNextId;

        bool isCalibration;

};

class QFRDRImagingFCSWizard_BackgroundIsValid: public QFWizardValidateFunctor {
    public:
        inline QFRDRImagingFCSWizard_BackgroundIsValid(QFRDRImagingFCSWizard* wizard):
            QFWizardValidateFunctor()
        {
            this->wizard=wizard;
        }
        virtual bool isValid(QFWizardPage* page);
    protected:
        QPointer<QFRDRImagingFCSWizard> wizard;
};

class QFRDRImagingFCSWizard_ImagestackIsValid: public QFWizardValidateFunctor {
    public:
        inline QFRDRImagingFCSWizard_ImagestackIsValid(QFRDRImagingFCSWizard* wizard):
            QFWizardValidateFunctor()
        {
            this->wizard=wizard;
        }
        virtual bool isValid(QFWizardPage* page);
    protected:
        QPointer<QFRDRImagingFCSWizard> wizard;
};

class QFRDRImagingFCSWizard_BackgroundNextId: public QFWizardNextPageFunctor {
    public:
        inline QFRDRImagingFCSWizard_BackgroundNextId(QFRDRImagingFCSWizard* wizard):
            QFWizardNextPageFunctor()
        {
            this->wizard=wizard;
        }
        virtual int nextID(const QFWizardPage* page) const;
    protected:
        QPointer<QFRDRImagingFCSWizard> wizard;
};
class QFRDRImagingFCSWizard_ProcessNextId: public QFWizardNextPageFunctor {
    public:
        inline QFRDRImagingFCSWizard_ProcessNextId(QFRDRImagingFCSWizard* wizard):
            QFWizardNextPageFunctor()
        {
            this->wizard=wizard;
        }
        virtual int nextID(const QFWizardPage* page) const;
    protected:
        QPointer<QFRDRImagingFCSWizard> wizard;
};

#endif // QFRDRIMAGINGFCSWIZARD_H
