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
    protected:
        QFFormWizardPage* wizSelfiles;
        QFImagePlotWizardPage* wizImageProps;
        QStringList imageFilters;
        QStringList imageFormatNames;
        QStringList imageFormatIDs;
        bool isProject;
        QFPixelSizeEdit* widPixSize;
        QFFrameRangeEdit* widFrameRange;
        QDoubleSpinBox* spinFrametime;
        QSpinBox* wizLSAnalysisspinMaskSize;
        QComboBox* wizLSAnalysiscmbFitDir;
        QComboBox* wizLSAnalysiscmbStackMode;
        QFEnhancedLineEdit* edtFilename;
        QFStyledButton* btnFilename;
        QFEnhancedLineEdit* edtBackground;
        QFStyledButton* btnBackground;
        QCheckBox* chkBackground;
        QFEnhancedComboBox* cmbDualView;
        QFEnhancedComboBox* cmbFileformat;
        QFPluginServices* pluginServices;



        int channels;
        int frame_count_io;
        qint64 filesize_io;
        double frametime_io;
        double baseline_offset_io;
        QString backgroundF_io;
        double pixel_width_io;
        double pixel_height_io;
        int dualViewMode_io;
        int image_width_io;
        int image_height_io;
        QString inputconfigfile_io;
        bool hasPixel_io;

};

#endif // QFRDRIMAGINGFCSWIZARD_H
