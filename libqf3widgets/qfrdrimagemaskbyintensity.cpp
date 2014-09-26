/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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


#include "qfrdrimagemaskbyintensity.h"
#include "ui_qfrdrimagemaskbyintensity.h"
#include "programoptions.h"
#include "qftools.h"
QFRDRImageMaskByIntensity::QFRDRImageMaskByIntensity(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImageMaskByIntensity)
{
    m_mask=NULL;
    m_image=NULL;
    m_width=0;
    m_height=0;
    ui->setupUi(this);
    plteImage=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0,0,JKQTPMathImageGRAY, ui->pltMain->get_plotter());
    ui->pltMain->addGraph(plteImage);
    QColor colTrue=QColor("red");
    colTrue.setAlphaF(0.7);
    plteMask=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0,0,colTrue, ui->pltMain->get_plotter());
    ui->pltMain->addGraph(plteMask);
    min=max=0;

    ui->cmbMaskingMode->setCurrentIndex(ProgramOptions::getConfigValue("QFRDRImageMaskByIntensity/maskingmode",2).toInt());
    ui->cmbMaskMode->setCurrentIndex(ProgramOptions::getConfigValue("QFRDRImageMaskByIntensity/maskmode", 0).toInt());
    loadWidgetGeometry(*(ProgramOptions::getInstance()->getQSettings()), this, "QFRDRImageMaskByIntensity/pos");
    updateWidgets();
    updateEnabledWidgets();
}

QFRDRImageMaskByIntensity::~QFRDRImageMaskByIntensity() {
    saveWidgetGeometry(*(ProgramOptions::getInstance()->getQSettings()), this, "QFRDRImageMaskByIntensity/pos");
    ProgramOptions::setConfigValue("QFRDRImageMaskByIntensity/maskingmode",ui->cmbMaskingMode->currentIndex());
    ProgramOptions::setConfigValue("QFRDRImageMaskByIntensity/maskmode", ui->cmbMaskMode->currentIndex());
    delete ui;
}

void QFRDRImageMaskByIntensity::init(bool *mask, double *image, uint32_t width, uint32_t height) {
    m_mask=mask;
    m_image=image;
    m_width=width;
    m_height=height;
    ui->labRange->setText("");
    plteImage->set_data(m_image, m_width, m_height, JKQTPMathImageBase::DoubleArray);
    plteImage->set_width(m_width);
    plteImage->set_height(m_height);
    plteMask->set_data(m_mask, m_width, m_height);
    plteMask->set_width(m_width);
    plteMask->set_height(m_height);
    plteImage->get_colorBarRightAxis()->set_minTicks(3);
    plteImage->get_colorBarTopAxis()->set_minTicks(5);
    plteImage->set_colorBarRightVisible(true);
    plteImage->set_colorBarTopVisible(false);

    ui->pltMain->set_displayMousePosition(false);
    ui->pltMain->set_displayToolbar(true);
    ui->pltMain->get_plotter()->set_maintainAspectRatio(true);
    ui->pltMain->get_plotter()->set_aspectRatio((double)m_width/(double)m_height);//1);
    ui->pltMain->get_plotter()->set_maintainAxisAspectRatio(true);
    ui->pltMain->get_plotter()->set_axisAspectRatio((double)m_width/(double)m_height);

    ui->pltMain->setAbsoluteXY(0,m_width,0,m_height);
    ui->pltMain->zoom(0,m_width,0,m_height);


    updateWidgets();

}

int QFRDRImageMaskByIntensity::getMaskMode() const {
    return ui->cmbMaskMode->currentIndex();
}

void QFRDRImageMaskByIntensity::updateMask() {
    updateEnabledWidgets();
    double levell=ui->edtLevel->value();
    double level=ui->edtLevel_2->value();
    if (ui->cmbMaskingMode->currentIndex()==0) { // in-range
        for (uint32_t i=0; i<m_width*m_height; i++) {
            double l=level;
            double ll=levell;
            m_mask[i]=(m_image[i]>=ll)&&(m_image[i]<=l);
        }
    } else if (ui->cmbMaskingMode->currentIndex()==1) { // out-of-range
        for (uint32_t i=0; i<m_width*m_height; i++) {
            double l=level;
            double ll=levell;

            m_mask[i]=!((m_image[i]>=ll)&&(m_image[i]<=l));
        }
    } else if (ui->cmbMaskingMode->currentIndex()==2) { // < threshold
        for (uint32_t i=0; i<m_width*m_height; i++) {
            double l=levell;

            m_mask[i]=m_image[i]<=l;
        }
    } else if (ui->cmbMaskingMode->currentIndex()==3) { // > threshold
        for (uint32_t i=0; i<m_width*m_height; i++) {
            double l=level;

            m_mask[i]=m_image[i]>=l;
        }
    } else {
        for (uint32_t i=1; i<m_width*m_height; i++) {
            m_mask[i]=false;
        }
    }
    ui->pltMain->update_plot();
}

void QFRDRImageMaskByIntensity::updateEnabledWidgets()
{


    if (ui->cmbMaskingMode->currentIndex()==0) { // in-range
        ui->edtLevel->setEnabled(true);
        ui->edtLevel_2->setEnabled(true);

     } else if (ui->cmbMaskingMode->currentIndex()==1) { // out-of-range
        ui->edtLevel->setEnabled(true);
        ui->edtLevel_2->setEnabled(true);

    } else if (ui->cmbMaskingMode->currentIndex()==2) { // < threshold
        ui->edtLevel->setEnabled(true);
        ui->edtLevel_2->setEnabled(false);

    } else if (ui->cmbMaskingMode->currentIndex()==3) { // > threshold
        ui->edtLevel->setEnabled(false);
        ui->edtLevel_2->setEnabled(true);

    }


    ui->sldLevel->setEnabled(ui->edtLevel->isEnabled());
    ui->sldLevel_2->setEnabled(ui->edtLevel_2->isEnabled());

}



void QFRDRImageMaskByIntensity::updateWidgets()
{
    updateEnabledWidgets();

    double l1=ui->edtLevel->value();

    if (m_image) {
        double min, max, min2, max2;

        bool first=true;
        for (uint16_t i=1; i<m_width*m_height; i++) {
            if (first) {
                min=max=m_image[i];
                first=false;
            } else {
                if (m_image[i]>max) max=m_image[i];
                if (m_image[i]<min) min=m_image[i];
            }
        }
        ui->labRange->setText(tr("%1 ... %2").arg(min).arg(max));


        ui->sldLevel->setDRange(min-10.0, max+10.0);
        ui->sldLevel_2->setDRange(min-10.0, max+10.0);
        ui->edtLevel->setRange(min-10.0, max+10.0);
        ui->edtLevel_2->setRange(min-10.0, max+10.0);

        ui->edtLevel->setValue(min+0.2*(max-min));
        ui->edtLevel_2->setValue(max);
        ui->sldLevel->setValue(min+0.2*(max-min));
        ui->sldLevel_2->setValue(max);

    }
    updateMask();
}

