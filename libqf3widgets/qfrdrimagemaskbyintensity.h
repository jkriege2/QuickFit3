#ifndef QFRDRIMAGEMASKBYINTENSITY_H
#define QFRDRIMAGEMASKBYINTENSITY_H

#include <QDialog>
#include <stdint.h>
#include "jkqtpimageelements.h"
#include "jkqtpimagetools.h"

namespace Ui {
    class QFRDRImageMaskByIntensity;
}

/*! \brief This dialog allows to select pixels by intensity
    \ingroup qf3rdrdp_imaging_fcs
*/
class QFRDRImageMaskByIntensity : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFRDRImageMaskByIntensity(QWidget *parent = 0);
        ~QFRDRImageMaskByIntensity();

        void init(bool* mask, double* image, uint16_t width, uint16_t height);

        int getMaskMode() const;

    protected slots:
        void updateMask();
        void updateEnabledWidgets();
        void updateWidgets();
        
    private:
        Ui::QFRDRImageMaskByIntensity *ui;

        bool* m_mask;
        double* m_image;
        uint16_t m_width;
        uint16_t m_height;

        double min;
        double max;

        JKQTPMathImage* plteImage;
        JKQTPOverlayImageEnhanced* plteMask;
};

#endif // QFRDRIMAGEMASKBYINTENSITY_H
