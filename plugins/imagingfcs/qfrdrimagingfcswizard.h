#ifndef QFRDRIMAGINGFCSWIZARD_H
#define QFRDRIMAGINGFCSWIZARD_H

#include <QWidget>
#include "qfwizard.h"
#include "qfselectfileslistwidget.h"
#include "qfimageplot.h"
#include "qfrdrimagingfcscorrelationjobthread.h"


class QFRDRImagingFCSWizard : public QFWizard
{
    public:
        QFRDRImagingFCSWizard(bool isp=false, QWidget* parent=NULL);
        ~QFRDRImagingFCSWizard();
    protected:
        QFSelectFilesListWizardPage* wizSelfiles;
        QFImagePlotWizardPage* wizLSAnalysisImgPreview;
        QStringList imageFilters;
        QStringList imageFormatNames;
        bool isProject;
        QDoubleSpinBox* wizLSAnalysisedtPixelSize;
        QSpinBox* wizLSAnalysisspinMaskSize;
        QDoubleSpinBox* wizLSAnalysisedtStepSize;
        QComboBox* wizLSAnalysiscmbFitDir;
        QComboBox* wizLSAnalysiscmbStackMode;

};

#endif // QFRDRIMAGINGFCSWIZARD_H
