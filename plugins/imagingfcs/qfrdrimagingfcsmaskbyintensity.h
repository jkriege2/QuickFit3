#ifndef QFRDRIMAGINGFCSMASKBYINTENSITY_H
#define QFRDRIMAGINGFCSMASKBYINTENSITY_H

#include <QDialog>
#include <stdint.h>
#include "jkqtpimageelements.h"
#include "jkqtpimagetools.h"

namespace Ui {
    class QFRDRImagingFCSMaskByIntensity;
}

/*! \brief This dialog allows to select pixels by intensity
    \ingroup qf3rdrdp_imaging_fcs
*/
class QFRDRImagingFCSMaskByIntensity : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFRDRImagingFCSMaskByIntensity(QWidget *parent = 0, bool selection=false);
        ~QFRDRImagingFCSMaskByIntensity();

        void init(bool* mask, double* image, uint32_t width, uint32_t height, int dualView);

        int getMaskMode() const;

    protected slots:
        void updateMask();
        void updateDualView();
        void updateEnabledWidgets();
        
    private:
        Ui::QFRDRImagingFCSMaskByIntensity *ui;

        bool* m_mask;
        double* m_image;
        uint32_t m_width;
        uint32_t m_height;

        double min;
        double max;

        JKQTPMathImage* plteImage;
        JKQTPOverlayImageEnhanced* plteMask;

        QString iniName;
};

#endif // QFRDRIMAGINGFCSMASKBYINTENSITY_H
