/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-26 12:40:44 +0200 (Fr, 26 Sep 2014) $  (revision $Rev: 3508 $)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "qfrdrmaskbyoverviewimagedlg.h"
#include "ui_qfrdrmaskbyoverviewimagedlg.h"
#include "programoptions.h"
#include "qftools.h"
QFRDRMaskByOverviewImage::QFRDRMaskByOverviewImage(QWidget *parent) :
    QFDialog(parent),
    ui(new Ui::QFRDRMaskByOverviewImage)
{
    m_mask=NULL;
    m_maskt=NULL;
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

    iniName="QFRDRMaskByOverviewImage";

    ui->cmbMaskingMode->setCurrentIndex(ProgramOptions::getConfigValue(iniName+"/maskingmode",2).toInt());
    ui->cmbMaskMode->setCurrentIndex(ProgramOptions::getConfigValue(iniName+"/maskmode", 0).toInt());

    loadWidgetGeometry(*(ProgramOptions::getInstance()->getQSettings()), this, iniName+"/pos");
    updateWidgets();
    updateEnabledWidgets();
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}

QFRDRMaskByOverviewImage::~QFRDRMaskByOverviewImage() {
    ProgramOptions::setConfigValue(iniName+"/maskingmode",ui->cmbMaskingMode->currentIndex());
    ProgramOptions::setConfigValue(iniName+"/maskmode", ui->cmbMaskMode->currentIndex());
    saveWidgetGeometry(*(ProgramOptions::getInstance()->getQSettings()), this, iniName+"/pos");
    if (m_mask) qfFree(m_mask);
    m_mask=NULL;
    if (m_maskt) qfFree(m_maskt);
    m_maskt=NULL;
    m_width=0;
    m_height=0;
    delete ui;
}



bool *QFRDRMaskByOverviewImage::getMask() const
{
    return m_mask;
}

bool *QFRDRMaskByOverviewImage::getMaskWithOld(bool *oldmask) const
{// replace, add, remove
    if (ui->cmbMaskMode->currentIndex()==0) {
        return getMask();
    } else if (ui->cmbMaskMode->currentIndex()==1) {
        for (uint64_t i=0; i<m_width*m_height; i++) {
            m_maskt[i]=m_mask[i]||oldmask[i];
        }
    } else if (ui->cmbMaskMode->currentIndex()==2) {
        for (uint64_t i=0; i<m_width*m_height; i++) {
            m_maskt[i]=oldmask[i];
            if (m_mask[i]) m_maskt[i]=false;
        }
    }
    return m_maskt;
}

void QFRDRMaskByOverviewImage::init(QFRDROverviewImagesInterface *rdr)
{
    disconnect(ui->cmbImage, SIGNAL(currentIndexChanged(int)), this, SLOT(imageChanged(int)));
    this->rdr=rdr;
    ui->cmbImage->clear();
    if (m_mask) qfFree(m_mask);
    m_mask=NULL;
    if (m_maskt) qfFree(m_maskt);
    m_maskt=NULL;
    m_width=0;
    m_height=0;
    if (rdr) {
        m_width=rdr->getOverviewImageWidth();
        m_height=rdr->getOverviewImageHeight();
        m_mask=(bool*)qfMalloc(m_width*m_height*sizeof(bool));
        m_maskt=(bool*)qfMalloc(m_width*m_height*sizeof(bool));
        for (int i=0; i<rdr->getOverviewImageCount(); i++) {
            ui->cmbImage->addItem(rdr->getOverviewImageName(i));
        }
        ui->cmbImage->setCurrentIndex(0);
        connect(ui->cmbImage, SIGNAL(currentIndexChanged(int)), this, SLOT(imageChanged(int)));
        imageChanged(ui->cmbImage->currentIndex());
    }
}


void QFRDRMaskByOverviewImage::updateMask() {
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

void QFRDRMaskByOverviewImage::updateEnabledWidgets()
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

void QFRDRMaskByOverviewImage::imageChanged(int i)
{
    if (i>=0 && i<rdr->getOverviewImageCount()) {
        m_image=rdr->getOverviewImage(i);
        m_width=rdr->getOverviewImageWidth();
        m_height=rdr->getOverviewImageHeight();
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
}



void QFRDRMaskByOverviewImage::updateWidgets()
{
    updateEnabledWidgets();


    if (m_image) {
        double min=0, max=0;


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




    }
    updateMask();
}

