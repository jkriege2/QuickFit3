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

void QFRDRImagingFCSMaskByIntensity::init(bool *mask, double *image, uint16_t width, uint16_t height) {
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

}

void QFRDRImagingFCSMaskByIntensity::updateMask() {
    double level=ui->edtLevel->value();
    if (ui->radMaskLarger->isChecked()) {
        for (uint16_t i=1; i<m_width*m_height; i++) {
            m_mask[i]=m_image[i]>level;
        }
    } else if (ui->radMaskSmaller->isChecked()) {
        for (uint16_t i=1; i<m_width*m_height; i++) {
            m_mask[i]=m_image[i]<level;
        }
    } else {
        for (uint16_t i=1; i<m_width*m_height; i++) {
            m_mask[i]=false;
        }
    }
    ui->pltMain->update_plot();
}
