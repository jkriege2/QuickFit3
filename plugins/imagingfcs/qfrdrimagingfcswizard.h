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

class QFRDRImagingFCSWizard : public QFWizard
{
    public:
        QFRDRImagingFCSWizard(bool isp=false, QWidget* parent=NULL);
        ~QFRDRImagingFCSWizard();
    protected slots:
        void selectFileClicked();
        void edtFilenameTextChanged(const QString& filename);
        void initImagePreview();
    protected:
        QFFormWizardPage* wizSelfiles;
        QFImagePlotWizardPage* wizImageProps;
        QStringList imageFilters;
        QStringList imageFormatNames;
        bool isProject;
        QDoubleSpinBox* wizLSAnalysisedtPixelSize;
        QSpinBox* wizLSAnalysisspinMaskSize;
        QDoubleSpinBox* wizLSAnalysisedtStepSize;
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

};

#endif // QFRDRIMAGINGFCSWIZARD_H
