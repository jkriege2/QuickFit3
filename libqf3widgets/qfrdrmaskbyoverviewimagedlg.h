/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-26 12:40:44 +0200 (Fr, 26 Sep 2014) $  (revision $Rev: 3508 $)

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

#ifndef QFRDRMaskByOverviewImage_H
#define QFRDRMaskByOverviewImage_H

#include <QDialog>
#include <stdint.h>
#include "jkqtpimageelements.h"
#include "jkqtpimagetools.h"
#include "libwid_imexport.h"
#include "qfrdroverviewimageinterface.h"


namespace Ui {
    class QFRDRMaskByOverviewImage;
}

/*! \brief This dialog allows to select pixels by intensity
    \ingroup qf3rdrdp_imaging_fcs
*/
class QFWIDLIB_EXPORT QFRDRMaskByOverviewImage : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFRDRMaskByOverviewImage(QWidget *parent = 0);
        ~QFRDRMaskByOverviewImage();

        bool* getMask() const;
        bool* getMaskWithOld(bool* oldmask) const;

        void init(QFRDROverviewImagesInterface* rdr);

    protected slots:
        void updateMask();
        void updateWidgets();
        void updateEnabledWidgets();
        void imageChanged(int i);
        
    protected:

        Ui::QFRDRMaskByOverviewImage *ui;

        bool* m_mask;
        bool* m_maskt;
        double* m_image;
        uint32_t m_width;
        uint32_t m_height;

        double min;
        double max;

        JKQTPMathImage* plteImage;
        JKQTPOverlayImageEnhanced* plteMask;

        QString iniName;

        QFRDROverviewImagesInterface* rdr;
};

#endif // QFRDRMaskByOverviewImage_H
