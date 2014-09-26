/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

        void init(bool* mask, double* image, uint32_t width, uint32_t height);

        int getMaskMode() const;

    protected slots:
        void updateMask();
        void updateEnabledWidgets();
        void updateWidgets();
        
    private:
        Ui::QFRDRImageMaskByIntensity *ui;

        bool* m_mask;
        double* m_image;
        uint32_t m_width;
        uint32_t m_height;

        double min;
        double max;

        JKQTPMathImage* plteImage;
        JKQTPOverlayImageEnhanced* plteMask;
};

#endif // QFRDRIMAGEMASKBYINTENSITY_H
