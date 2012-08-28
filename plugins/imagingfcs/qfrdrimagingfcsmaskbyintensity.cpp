#include "qfrdrimagingfcsmaskbyintensity.h"
#include "ui_qfrdrimagingfcsmaskbyintensity.h"

QFRDRImagingFCSMaskByIntensity::QFRDRImagingFCSMaskByIntensity(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRImagingFCSMaskByIntensity)
{
    m_mask=NULL;
    m_image=NULL;
    m_width=0;
    m_height=0;
    ui->setupUi(this);
    plteImage=new JKQTPMathImage(0,0,1,1,JKQTPMathImageBase::DoubleArray, NULL, 0,0,JKQTPMathImage::GRAY, ui->pltMain->get_plotter());
    ui->pltMain->addGraph(plteImage);
    QColor colTrue=QColor("red");
    colTrue.setAlphaF(0.7);
    plteMask=new JKQTPOverlayImageEnhanced(0,0,1,1,NULL, 0,0,colTrue, ui->pltMain->get_plotter());
    ui->pltMain->addGraph(plteMask);
    min=max=0;
}

QFRDRImagingFCSMaskByIntensity::~QFRDRImagingFCSMaskByIntensity() {
    delete ui;
}

void QFRDRImagingFCSMaskByIntensity::init(bool *mask, double *image, uint16_t width, uint16_t height, int dualView) {
    m_mask=mask;
    m_image=image;
    m_width=width;
    m_height=height;
    ui->labRange->setText("");
    if (m_image) {
        min=max=m_image[0];
        for (uint16_t i=1; i<m_width*m_height; i++) {
            if (m_image[i]>max) max=m_image[i];
            if (m_image[i]<min) min=m_image[i];
        }
        ui->labRange->setText(tr("%1 ... %2").arg(min).arg(max));
        ui->edtLevel->setRange(min, max);
        ui->edtLevel->setValue(min+0.2*(max-min));
    }
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

    ui->pltMain->set_zoomByDoubleAndRightMouseClick(false);
    ui->pltMain->set_displayMousePosition(false);
    ui->pltMain->set_displayToolbar(true);
    ui->pltMain->get_plotter()->set_maintainAspectRatio(true);
    ui->pltMain->get_plotter()->set_aspectRatio(1);
    ui->pltMain->get_plotter()->set_maintainAxisAspectRatio(true);
    ui->pltMain->get_plotter()->set_axisAspectRatio((double)m_width/(double)m_height);

    ui->pltMain->setAbsoluteXY(0,m_width,0,m_height);
    ui->pltMain->zoom(0,m_width,0,m_height);

    ui->cmbDualView->setCurrentIndex(dualView);
    ui->chkEqualChannels->setChecked(false);
}

int QFRDRImagingFCSMaskByIntensity::getMaskMode() const {
    return ui->cmbMaskMode->currentIndex();
}

void QFRDRImagingFCSMaskByIntensity::updateMask() {
    double level=ui->edtLevel->value();
    double level2=ui->edtLevel2->value();
    if (ui->radMaskLarger->isChecked()) {
        for (uint16_t i=0; i<m_width*m_height; i++) {
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
            m_mask[i]=m_image[i]>l;
        }
    } else if (ui->radMaskSmaller->isChecked()) {
        for (uint16_t i=0; i<m_width*m_height; i++) {
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
            m_mask[i]=m_image[i]<l;
        }
    } else {
        for (uint16_t i=1; i<m_width*m_height; i++) {
            m_mask[i]=false;
        }
    }
    if (ui->chkEqualChannels->isChecked()) {
        if (ui->cmbDualView->currentIndex()==1) {
            for (uint16_t y=0; y<m_height; y++) {
                for (uint16_t x=0; x<m_width/2; x++) {
                    m_mask[y*m_width+(x+m_width/2)]=m_mask[y*m_width+x];
                }
            }
        } else if (ui->cmbDualView->currentIndex()==2) {
            for (uint16_t y=0; y<m_height/2; y++) {
                for (uint16_t x=0; x<m_width; x++) {
                    m_mask[(y+m_height/2)*m_width+x]=m_mask[y*m_width+x];
                }
            }
        }
    }
    ui->pltMain->update_plot();
}

void QFRDRImagingFCSMaskByIntensity::updateDualView()
{
    ui->chkEqualChannels->setEnabled(ui->cmbDualView->currentIndex()>0);
    ui->edtLevel2->setEnabled(ui->cmbDualView->currentIndex()>0  &&  !ui->chkEqualChannels->isChecked());

    double l1=ui->edtLevel->value();
    double l2=ui->edtLevel2->value();

    if (m_image) {
        double min, max, min2, max2;

        if (ui->cmbDualView->currentIndex()==0) {
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
            ui->edtLevel->setRange(min, max);
            ui->edtLevel->setValue(min+0.2*(max-min));
            ui->edtLevel2->setRange(min, max);
            ui->edtLevel2->setValue(min+0.2*(max-min));
        } else if (ui->cmbDualView->currentIndex()==1) {
            bool first=true, first2=true;
            for (uint16_t i=1; i<m_width*m_height; i++) {
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
            ui->labRange->setText(tr("%1 ... %2").arg(min).arg(max));
            ui->edtLevel->setRange(min, max);
            ui->edtLevel->setValue(min+0.2*(max-min));
            ui->edtLevel2->setRange(min2, max2);
            ui->edtLevel2->setValue(min2+0.2*(max2-min2));
        } else if (ui->cmbDualView->currentIndex()==2) {
            bool first=true, first2=true;
            for (uint16_t i=1; i<m_width*m_height; i++) {
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
            ui->labRange->setText(tr("%1 ... %2").arg(min).arg(max));
            ui->edtLevel->setRange(min, max);
            ui->edtLevel->setValue(min+0.2*(max-min));
            ui->edtLevel2->setRange(min2, max2);
            ui->edtLevel2->setValue(min2+0.2*(max2-min2));
        }
    }
    updateMask();
}
