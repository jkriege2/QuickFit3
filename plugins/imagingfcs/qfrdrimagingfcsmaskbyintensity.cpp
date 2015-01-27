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



#include "qfrdrimagingfcsmaskbyintensity.h"
#include "ui_qfrdrimagingfcsmaskbyintensity.h"
#include "programoptions.h"
#include "qftools.h"
#include "statistics_tools.h"

QFRDRImagingFCSMaskByIntensity::QFRDRImagingFCSMaskByIntensity(QWidget *parent, bool selection) :
    QDialog(parent),
    ui(new Ui::QFRDRImagingFCSMaskByIntensity)
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

    iniName="QFRDRImagingFCSMaskByIntensity";
    if (selection) iniName="QFRDRImagingFCSSelectByIntensity";

    ui->cmbMaskingMode->setCurrentIndex(ProgramOptions::getConfigValue(iniName+"/maskingmode",2).toInt());
    ui->cmbMaskMode->setCurrentIndex(ProgramOptions::getConfigValue(iniName+"/maskmode", 0).toInt());
    ui->chkColorScaling->setChecked(ProgramOptions::getConfigValue(iniName+"/chkColorScaling", false).toBool());
    ui->edtImgRangeMin->setValue(ProgramOptions::getConfigValue(iniName+"/edtImgRangeMin", 5).toDouble());
    ui->edtImgRangeMax->setValue(ProgramOptions::getConfigValue(iniName+"/edtImgRangeMax", 5).toDouble());
    loadWidgetGeometry(*(ProgramOptions::getInstance()->getQSettings()), this, iniName+"/geo/");
    updateDualView();
    updateEnabledWidgets();
}

QFRDRImagingFCSMaskByIntensity::~QFRDRImagingFCSMaskByIntensity() {
    saveWidgetGeometry(*(ProgramOptions::getInstance()->getQSettings()), this, iniName+"/geo/");
    ProgramOptions::setConfigValue(iniName+"/maskingmode",ui->cmbMaskingMode->currentIndex());
    ProgramOptions::setConfigValue(iniName+"/maskmode", ui->cmbMaskMode->currentIndex());
    ProgramOptions::setConfigValue(iniName+"/channelsequal", ui->chkEqualChannels->isChecked());
    ProgramOptions::setConfigValue(iniName+"/chkColorScaling", ui->chkColorScaling->isChecked());
    ProgramOptions::setConfigValue(iniName+"/edtImgRangeMin", ui->edtImgRangeMin->value());
    ProgramOptions::setConfigValue(iniName+"/edtImgRangeMax", ui->edtImgRangeMax->value());
    delete ui;
}

void QFRDRImagingFCSMaskByIntensity::init(bool *mask, double *image, uint32_t width, uint32_t height, int dualView) {
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

    ui->cmbDualView->setCurrentIndex(dualView);
    ui->chkEqualChannels->setChecked(false);

    updateDualView();
}

int QFRDRImagingFCSMaskByIntensity::getMaskMode() const {
    return ui->cmbMaskMode->currentIndex();
}

void QFRDRImagingFCSMaskByIntensity::updateMask() {
    updateEnabledWidgets();
    double levell=ui->edtLevel->value();
    double level2l=ui->edtLevel2->value();
    double level=ui->edtLevel_2->value();
    double level2=ui->edtLevel2_2->value();
    if (ui->cmbMaskingMode->currentIndex()==0) { // in-range
        for (uint32_t i=0; i<m_width*m_height; i++) {
            double l=level;
            double ll=levell;
            if (ui->cmbDualView->currentIndex()==1) {
                if (i%m_width>=m_width/2) {
                    l=level2;
                    ll=level2l;
                }
            } else if (ui->cmbDualView->currentIndex()==2) {
                if (i/m_width>=m_height/2) {
                    l=level2;
                    ll=level2l;
                }
            }
            m_mask[i]=(m_image[i]>=ll)&&(m_image[i]<=l);
        }
    } else if (ui->cmbMaskingMode->currentIndex()==1) { // out-of-range
        for (uint32_t i=0; i<m_width*m_height; i++) {
            double l=level;
            double ll=levell;
            if (ui->cmbDualView->currentIndex()==1) {
                if (i%m_width>=m_width/2) {
                    l=level2;
                    ll=level2l;
                }
            } else if (ui->cmbDualView->currentIndex()==2) {
                if (i/m_width>=m_height/2) {
                    l=level2;
                    ll=level2l;
                }
            }

            m_mask[i]=!((m_image[i]>=ll)&&(m_image[i]<=l));
        }
    } else if (ui->cmbMaskingMode->currentIndex()==2) { // < threshold
        for (uint32_t i=0; i<m_width*m_height; i++) {
            double l=levell;
            if (ui->cmbDualView->currentIndex()==1) {
                if (i%m_width>=m_width/2) {
                    l=level2l;
                }
            } else if (ui->cmbDualView->currentIndex()==2) {
                if (i/m_width>=m_height/2) {
                    l=level2l;
                }
            }

            m_mask[i]=m_image[i]<=l;
        }
    } else if (ui->cmbMaskingMode->currentIndex()==3) { // > threshold
        for (uint32_t i=0; i<m_width*m_height; i++) {
            double l=level;
            if (ui->cmbDualView->currentIndex()==1) {
                if (i%m_width>=m_width/2) {
                    l=level2;
                }
            } else if (ui->cmbDualView->currentIndex()==2) {
                if (i/m_width>=m_height/2) {
                    l=level2;
                }
            }

            m_mask[i]=m_image[i]>=l;
        }
    } else {
        for (uint32_t i=1; i<m_width*m_height; i++) {
            m_mask[i]=false;
        }
    }
    if (ui->chkEqualChannels->isChecked()) {
        if (ui->cmbDualView->currentIndex()==1) {
            for (uint32_t y=0; y<m_height; y++) {
                for (uint32_t x=0; x<m_width/2; x++) {
                    m_mask[y*m_width+(x+m_width/2)]=m_mask[y*m_width+x];
                }
            }
        } else if (ui->cmbDualView->currentIndex()==2) {
            for (uint32_t y=0; y<m_height/2; y++) {
                for (uint32_t x=0; x<m_width; x++) {
                    m_mask[(y+m_height/2)*m_width+x]=m_mask[y*m_width+x];
                }
            }
        }
    }
    ui->pltMain->update_plot();
}

void QFRDRImagingFCSMaskByIntensity::updateEnabledWidgets()
{
    bool twocolor=ui->cmbDualView->currentIndex()>0;
    ui->chkEqualChannels->setEnabled(twocolor);


    if (ui->cmbMaskingMode->currentIndex()==0) { // in-range
        ui->edtLevel->setEnabled(true);
        ui->edtLevel_2->setEnabled(true);

        ui->edtLevel2->setEnabled(twocolor);
        ui->edtLevel2_2->setEnabled(twocolor);
     } else if (ui->cmbMaskingMode->currentIndex()==1) { // out-of-range
        ui->edtLevel->setEnabled(true);
        ui->edtLevel_2->setEnabled(true);

        ui->edtLevel2->setEnabled(twocolor);
        ui->edtLevel2_2->setEnabled(twocolor);
    } else if (ui->cmbMaskingMode->currentIndex()==2) { // < threshold
        ui->edtLevel->setEnabled(true);
        ui->edtLevel_2->setEnabled(false);

        ui->edtLevel2->setEnabled(twocolor);
        ui->edtLevel2_2->setEnabled(false);
    } else if (ui->cmbMaskingMode->currentIndex()==3) { // > threshold
        ui->edtLevel->setEnabled(false);
        ui->edtLevel_2->setEnabled(true);

        ui->edtLevel2->setEnabled(false);
        ui->edtLevel2_2->setEnabled(twocolor);
    }


    ui->sldLevel->setEnabled(ui->edtLevel->isEnabled());
    ui->sldLevel_2->setEnabled(ui->edtLevel_2->isEnabled());
    ui->sldLevel2->setEnabled(ui->edtLevel2->isEnabled());
    ui->sldLevel2_2->setEnabled(ui->edtLevel2_2->isEnabled());

}

void QFRDRImagingFCSMaskByIntensity::on_edtImgRangeMin_valueChanged(double val)
{
    updateImage();
}

void QFRDRImagingFCSMaskByIntensity::on_edtImgRangeMax_valueChanged(double val)
{
    updateImage();
}

void QFRDRImagingFCSMaskByIntensity::on_chkColorScaling_toggled(bool checked)
{
    updateImage();
}

void QFRDRImagingFCSMaskByIntensity::updateImage()
{
    ui->pltMain->set_doDrawing(false);
    if (ui->chkColorScaling->isChecked()) {
        plteImage->set_autoImageRange(false);
        plteImage->set_imageMin(statisticsQuantile(m_image, m_width*m_height, ui->edtImgRangeMin->value()/100.0));
        plteImage->set_imageMax(statisticsQuantile(m_image, m_width*m_height, 1.0-ui->edtImgRangeMax->value()/100.0));
    } else {
        plteImage->set_autoImageRange(true);
    }
    ui->pltMain->set_doDrawing(true);
    ui->pltMain->update_plot();
}

void QFRDRImagingFCSMaskByIntensity::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayPluginHelpWindow("imaging_fcs", "imfcs_maskbyintensity.html");
}



void QFRDRImagingFCSMaskByIntensity::updateDualView()
{
    updateEnabledWidgets();

    double l1=ui->edtLevel->value();
    double l2=ui->edtLevel2->value();

    if (m_image) {
        double min, max, min2, max2;

        if (ui->cmbDualView->currentIndex()==0) {
            bool first=true;
            for (uint32_t i=1; i<m_width*m_height; i++) {
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

            ui->edtLevel->setValue(min+0.25*(max-min));
            ui->edtLevel_2->setValue(max);
            ui->sldLevel->setDValue(min+0.25*(max-min));
            ui->sldLevel_2->setDValue(max);


            ui->sldLevel2->setDRange(min-10.0, max+10.0);
            ui->sldLevel2_2->setDRange(min-10.0, max+10.0);
            ui->edtLevel2->setRange(min-10.0, max+10.0);
            ui->edtLevel2_2->setRange(min-10.0, max+10.0);

            ui->edtLevel2->setValue(min+0.25*(max-min));
            ui->edtLevel2_2->setValue(max);
            ui->sldLevel2->setDValue(min+0.25*(max-min));
            ui->sldLevel2_2->setDValue(max);


        } else if (ui->cmbDualView->currentIndex()==1) {
            bool first=true, first2=true;
            for (uint32_t i=1; i<m_width*m_height; i++) {
                if (i%m_width<=m_width/2) {
                    if (first) {
                        min=max=m_image[i];
                        first=false;
                    } else {
                        if (m_image[i]>max) max=m_image[i];
                        if (m_image[i]<min) min=m_image[i];
                    }
                } else {
                    if (first2) {
                        min2=max2=m_image[i];
                        first2=false;
                    } else {
                        if (m_image[i]>max2) max2=m_image[i];
                        if (m_image[i]<min2) min2=m_image[i];
                    }
                }
            }
            ui->labRange->setText(tr("%1 ... %2\n%3 ... %4").arg(min).arg(max).arg(min2).arg(max2));
            ui->sldLevel->setDRange(min-10.0, max+10.0);
            ui->sldLevel_2->setDRange(min-10.0, max+10.0);
            ui->edtLevel->setRange(min-10.0, max+10.0);
            ui->edtLevel_2->setRange(min-10.0, max+10.0);

            ui->edtLevel->setValue(min+0.25*(max-min));
            ui->edtLevel_2->setValue(max);
            ui->sldLevel->setDValue(min+0.25*(max-min));
            ui->sldLevel_2->setDValue(max);


            ui->sldLevel2->setDRange(min2-10.0, max2+10.0);
            ui->sldLevel2_2->setDRange(min2-10.0, max2+10.0);
            ui->edtLevel2->setRange(min2-10.0, max2+10.0);
            ui->edtLevel2_2->setRange(min2-10.0, max2+10.0);

            ui->edtLevel2->setValue(min2+0.25*(max2-min2));
            ui->edtLevel2_2->setValue(max2);
            ui->sldLevel2->setDValue(min2+0.25*(max2-min2));
            ui->sldLevel2_2->setDValue(max2);

        } else if (ui->cmbDualView->currentIndex()==2) {
            bool first=true, first2=true;
            for (uint32_t i=1; i<m_width*m_height; i++) {
                if (i/m_width<=m_height/2) {
                    if (first) {
                        min=max=m_image[i];
                        first=false;
                    } else {
                        if (m_image[i]>max) max=m_image[i];
                        if (m_image[i]<min) min=m_image[i];
                    }
                } else {
                    if (first2) {
                        min2=max2=m_image[i];
                        first2=false;
                    } else {
                        if (m_image[i]>max2) max2=m_image[i];
                        if (m_image[i]<min2) min2=m_image[i];
                    }
                }
            }
            ui->labRange->setText(tr("%1 ... %2\n%3 ... %4").arg(min).arg(max).arg(min2).arg(max2));
            ui->sldLevel->setDRange(min-10.0, max+10.0);
            ui->sldLevel_2->setDRange(min-10.0, max+10.0);
            ui->edtLevel->setRange(min-10.0, max+10.0);
            ui->edtLevel_2->setRange(min-10.0, max+10.0);

            ui->edtLevel->setValue(min+0.25*(max-min));
            ui->edtLevel_2->setValue(max);
            ui->sldLevel->setDValue(min+0.25*(max-min));
            ui->sldLevel_2->setDValue(max);


            ui->sldLevel2->setDRange(min2-10.0, max2+10.0);
            ui->sldLevel2_2->setDRange(min2-10.0, max2+10.0);
            ui->edtLevel2->setRange(min2-10.0, max2+10.0);
            ui->edtLevel2_2->setRange(min2-10.0, max2+10.0);

            ui->edtLevel2->setValue(min2+0.25*(max2-min2));
            ui->edtLevel2_2->setValue(max2);
            ui->sldLevel2->setDValue(min2+0.25*(max2-min2));
            ui->sldLevel2_2->setDValue(max2);
        }
    }
    updateMask();
}

