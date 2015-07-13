#ifndef QFRDRIMAGINGFCSWIZARD_H
#define QFRDRIMAGINGFCSWIZARD_H

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

class QFRDRImagingFCSWizard_BackgroundIsValid; // forward
class QFRDRImagingFCSWizard_ImagestackIsValid;

class QFRDRImagingFCSWizard : public QFWizard {
        Q_OBJECT
    public:
        QFRDRImagingFCSWizard(bool isp=false, QWidget* parent=NULL);
        ~QFRDRImagingFCSWizard();
    protected slots:
        void selectFileClicked();
        void edtFilenameTextChanged(const QString& filename);
        void initImagePreview();
        void initFileSelection();
        void backgroundModeChanged(int mode);
        void calcPixelSize();
    protected:
        QFFormWizardPage* wizSelfiles;
        QFImagePlotWizardPage* wizImageProps;
        QStringList imageFilters;
        QStringList imageFormatNames;
        QStringList imageFormatIDs;
        bool isProject;
        QFPixelSizeEdit* widPixSize;
        QPushButton* btnPixSize;
        QFFrameRangeEdit* widFrameRange;
        QDoubleSpinBox* spinFrametime;
        QSpinBox* wizLSAnalysisspinMaskSize;
        QComboBox* wizLSAnalysiscmbFitDir;
        QComboBox* wizLSAnalysiscmbStackMode;
        QFEnhancedLineEdit* edtFilename;
        QFStyledButton* btnFilename;
        QFEnhancedComboBox* cmbDualView;
        QFEnhancedComboBox* cmbFileformat;
        QFPluginServices* pluginServices;
        QLabel* labFileError;

        QFFormWizardPage* wizBackground;
        QFEnhancedLineEdit* edtBackgroundFilename;
        QFStyledButton* btnBackgroundFilename;
        QComboBox* cmbBackgroundMode;
        QSpinBox* spinBackgroundOffset;
        QLabel* labBackgroundError;

        QLabel* labImageProps;


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
        QFRDRImagingFCSWizard_BackgroundIsValid* fctrBack;
        QFRDRImagingFCSWizard_ImagestackIsValid* fctrStack;
        friend class QFRDRImagingFCSWizard_BackgroundIsValid;
        friend class QFRDRImagingFCSWizard_ImagestackIsValid;

};

class QFRDRImagingFCSWizard_BackgroundIsValid: public QFWizardValidateFunctor {
    public:
        QFRDRImagingFCSWizard_BackgroundIsValid(QFRDRImagingFCSWizard* wizard);
        virtual bool isValid(QFWizardPage* page);
    protected:
        QFRDRImagingFCSWizard* wizard;
};

class QFRDRImagingFCSWizard_ImagestackIsValid: public QFWizardValidateFunctor {
    public:
        QFRDRImagingFCSWizard_ImagestackIsValid(QFRDRImagingFCSWizard* wizard);
        virtual bool isValid(QFWizardPage* page);
    protected:
        QFRDRImagingFCSWizard* wizard;
};

#endif // QFRDRIMAGINGFCSWIZARD_H
