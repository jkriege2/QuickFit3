#include "qfrdrimagingfcsimageparametergroupbox.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include "qfrawdatarecord.h"
#include "statistics_tools.h"

QFRDRImagingFCSImageParameterGroupBox::QFRDRImagingFCSImageParameterGroupBox(QWidget *parent) :
    QGroupBox(parent)
{
    initGrp();
}

QFRDRImagingFCSImageParameterGroupBox::QFRDRImagingFCSImageParameterGroupBox(const QString &title, QWidget *parent):
    QGroupBox(title, parent)
{
    initGrp();
}

void QFRDRImagingFCSImageParameterGroupBox::setSelectedMaskStyle(JKQTPOverlayImageEnhanced *plot) const
{
    QColor col=maskColors.value(cmbImageStyle->currentIndex(), QColor("red"));
    plot->set_trueColor(Qt::transparent);
    plot->set_symbolSizeFactor(0.7);
    plot->set_drawAsRectangles(true);
    plot->set_symbol(JKQTPnoSymbol);
    plot->set_falseColor(col);
    //plot->set_symbolWidth();


    /*if (cmbImageStyle->currentIndex()==0) {
        plteImageExcluded->set_trueColor(QColor("black"));
    } else if (cmbImageStyle->currentIndex()==1) {
        plteImageExcluded->set_trueColor(QColor("white"));
    } else if (cmbImageStyle->currentIndex()==2) {
        plteImageExcluded->set_trueColor(QColor("grey"));
    } else if (cmbImageStyle->currentIndex()==3) {
        plteImageExcluded->set_trueColor(excludedColor);
    } else if (cmbImageStyle->currentIndex()==3) {
        plteImageExcluded->set_trueColor(Qt::transparent);
    }*/
}

void QFRDRImagingFCSImageParameterGroupBox::setSelectedImageStyle(JKQTPMathImage *plteImage, bool* plteOverviewExcludedData)
{

    plteImage->set_palette(cmbColorbar->colorPalette());
    plteImage->set_autoImageRange(false);//chkImageAutoScale->isChecked());
    //plteImage->set_autoImageRange(false);
    double mi=0, ma=0;
    if (plteImage->get_data() && plteOverviewExcludedData) statisticsMaskedMinMax((double*)plteImage->get_data(), plteOverviewExcludedData, plteImage->get_Nx()*plteImage->get_Ny(), mi, ma, false);
    if (chkImageAutoScale->isChecked() && plteImage->get_data()!=NULL && plteOverviewExcludedData!=NULL) {
        plteImage->set_imageMin(mi);
        plteImage->set_imageMax(ma);
    }
    //qDebug()<<"paletteChanged():  "<<plteImageData << plteOverviewExcludedData<<":     "<<mi<<"..."<<ma;
    switch(cmbOutOfRangeMode->currentIndex()) {
        case 0:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::LastPaletteColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::LastPaletteColor);
            break;
        default:
        case 1:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMinFailColor(QColor("black"));
            plteImage->set_rangeMaxFailColor(QColor("black"));
            break;
        case 2:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMinFailColor(QColor("white"));
            plteImage->set_rangeMaxFailColor(QColor("white"));
            break;
        case 3:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::Transparent);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::Transparent);
            break;
        case 4:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMinFailColor(QColor("red"));
            plteImage->set_rangeMaxFailColor(QColor("red"));
            break;
        case 5:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMinFailColor(QColor("green"));
            plteImage->set_rangeMaxFailColor(QColor("green"));
            break;
        case 6:
            plteImage->set_rangeMinFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImage::GivenColor);
            plteImage->set_rangeMinFailColor(QColor("blue"));
            plteImage->set_rangeMaxFailColor(QColor("blue"));
            break;
    }

    edtColMin->setEnabled(!chkImageAutoScale->isChecked());
    edtColMax->setEnabled(!chkImageAutoScale->isChecked());
    if (chkImageAutoScale->isChecked()) {
        //double mi=0, ma=0;
        //if (mi==ma) plteImage->getDataMinMax(mi, ma);
        edtColMin->setValue(mi);
        edtColMax->setValue(ma);
    }
    plteImage->set_imageMin(edtColMin->value());
    plteImage->set_imageMax(edtColMax->value());

    emit settingsChanged();
}

int QFRDRImagingFCSImageParameterGroupBox::getMaskStyle() const
{
    return cmbImageStyle->currentIndex();
}

void QFRDRImagingFCSImageParameterGroupBox::setMaskStyle(int idx)
{
    cmbImageStyle->setCurrentIndex(idx);
}

JKQTPMathImage::ColorPalette QFRDRImagingFCSImageParameterGroupBox::colorPalette() const
{
    return cmbColorbar->colorPalette();
}

void QFRDRImagingFCSImageParameterGroupBox::setColorPalette(JKQTPMathImage::ColorPalette palette)
{
    cmbColorbar->setColorPalette(palette);
}

void QFRDRImagingFCSImageParameterGroupBox::saveConfig(QFRawDataRecord *current, const QString &egroup, const QString &param, const QString &imageID)
{
    current->setQFProperty(QString("imfcs_imed_colorbar_%1_%3_%2").arg(egroup).arg(param).arg(imageID), cmbColorbar->currentIndex(), false, false);
    current->setQFProperty(QString("imfcs_imed_style_%1_%3_%2").arg(egroup).arg(param).arg(imageID), cmbImageStyle->currentIndex(), false, false);
    current->setQFProperty(QString("imfcs_imed_oorc_%1_%3_%2").arg(egroup).arg(param).arg(imageID), cmbOutOfRangeMode->currentIndex(), false, false);
    current->setQFProperty(QString("imfcs_imed_autorange_%1_%3_%2").arg(egroup).arg(param).arg(imageID), chkImageAutoScale->isChecked(), false, false);
    if (!chkImageAutoScale->isChecked()) {
        current->setQFProperty(QString("imfcs_imed_colmin_%1_%3_%2").arg(egroup).arg(param).arg(imageID), edtColMin->value(), false, false);
        current->setQFProperty(QString("imfcs_imed_colmax_%1_%3_%2").arg(egroup).arg(param).arg(imageID), edtColMax->value(), false, false);
    }

}

void QFRDRImagingFCSImageParameterGroupBox::loadConfig(QFRawDataRecord *current, const QString &egroup, const QString &param, const QString &imageID, const QString &prefix)
{
    double mi=0, ma=1;
    /*connectParameterWidgets(false);
    //histogram->connectParameterWidgets(false);
    plteImage->getDataMinMax(mi, ma);
    //statisticsMaskedMinMax(plteImage->get_data(), plteOverviewExcludedData, plteImage->get_Nx()+plteImage->get_Ny(), mi, ma, false);*/
    int d=current->getProperty(QString("imfcs_imed_colorbar_%1_%3_%2").arg(egroup).arg(param).arg(imageID),
                               current->getProperty(QString("imfcs_imed_colorbar_%1_%2").arg(egroup).arg(param),
                               ProgramOptions::getInstance()->getQSettings()->value(prefix+QString("colorbar"), cmbColorbar->currentIndex()))).toInt();
    if (d>=0) cmbColorbar->setCurrentIndex(d);
    else if (cmbColorbar->count()>0) cmbColorbar->setColorPalette(JKQTPMathImage::MATLAB);
    cmbImageStyle->setCurrentIndex(current->getProperty(QString("imfcs_imed_style_%1_%3_%2").arg(egroup).arg(param).arg(imageID), current->getProperty(QString("imfcs_imed_style_%1_%2").arg(egroup).arg(param), cmbImageStyle->currentIndex())).toInt());
    cmbOutOfRangeMode->setCurrentIndex(current->getProperty(QString("imfcs_imed_oorc_%1_%3_%2").arg(egroup).arg(param).arg(imageID), current->getProperty(QString("imfcs_imed_oorc_%1_%2").arg(egroup).arg(param), cmbOutOfRangeMode->currentIndex())).toInt());
    chkImageAutoScale->setChecked(current->getProperty(QString("imfcs_imed_autorange_%1_%3_%2").arg(egroup).arg(param).arg(imageID), current->getProperty(QString("imfcs_imed_autorange_%1_%2").arg(egroup).arg(param), true)).toBool());
    if (!chkImageAutoScale->isChecked()) {
        edtColMin->setValue(current->getProperty(QString("imfcs_imed_colmin_%1_%3_%2").arg(egroup).arg(param).arg(imageID), current->getProperty(QString("imfcs_imed_colmin_%1_%2").arg(egroup).arg(param), mi)).toDouble());
        edtColMax->setValue(current->getProperty(QString("imfcs_imed_colmax_%1_%3_%2").arg(egroup).arg(param).arg(imageID), current->getProperty(QString("imfcs_imed_colmax_%1_%2").arg(egroup).arg(param), ma)).toDouble());
    }

}

void QFRDRImagingFCSImageParameterGroupBox::writeSettings(QSettings &settings, const QString &prefix) const
{
    settings.setValue(prefix+QString("colorbar"), cmbColorbar->currentIndex());
    settings.setValue(prefix+QString("paramstyle"), cmbImageStyle->currentIndex());
    settings.setValue(prefix+QString("outofrange_mode"), cmbOutOfRangeMode->currentIndex());
}

void QFRDRImagingFCSImageParameterGroupBox::readSettings(QSettings &settings, const QString &prefix)
{
    cmbColorbar->setCurrentIndex(settings.value(prefix+QString("colorbar"),  JKQTPMathImage::MATLAB).toInt());
    cmbImageStyle->setCurrentIndex(settings.value(prefix+QString("paramstyle"), 0).toInt());
    cmbOutOfRangeMode->setCurrentIndex(settings.value(prefix+QString("outofrange_mode"), 1).toInt());

}

void QFRDRImagingFCSImageParameterGroupBox::initGrp()
{
    setFlat(true);

    QFormLayout* gli=new QFormLayout(this);
    gli->setSpacing(2);
    setLayout(gli);

    cmbImageStyle=new QComboBox(this);
    {
        QPixmap selPix(15,15);
        QColor ovlSelCol=QColor("black");
        maskColors<<ovlSelCol;
        selPix.fill(ovlSelCol);
        cmbImageStyle->addItem(QIcon(selPix), tr("black mask"));
        ovlSelCol=QColor("white");
        maskColors<<ovlSelCol;
        selPix.fill(ovlSelCol);
        cmbImageStyle->addItem(QIcon(selPix), tr("white mask"));
        ovlSelCol=QColor("grey");
        maskColors<<ovlSelCol;
        selPix.fill(ovlSelCol);
        cmbImageStyle->addItem(QIcon(selPix), tr("grey mask"));
        ovlSelCol=QColor("blue");
        ovlSelCol.setAlphaF(0.5);
        maskColors<<ovlSelCol;
        selPix.fill(ovlSelCol);
        cmbImageStyle->addItem(QIcon(selPix), tr("blue overlay mask"));
        ovlSelCol=Qt::transparent;
        maskColors<<ovlSelCol;
        selPix.fill(ovlSelCol);
        cmbImageStyle->addItem(tr("none: image only"));
    }
    gli->addRow(tr("st&yle:"), cmbImageStyle);


    cmbColorbar=new JKQTPMathImageColorPalette(this);
    cmbColorbar->setColorPalette(JKQTPMathImage::MATLAB);
    gli->addRow(tr("color &palette:"), cmbColorbar);

    chkImageAutoScale=new QCheckBox("auto", this);
    chkImageAutoScale->setChecked(true);
    gli->addRow(tr("color &range:"), chkImageAutoScale);

    edtColMin=new QFDoubleEdit(this);
    edtColMin->setCheckBounds(false, false);
    edtColMax=new QFDoubleEdit(this);
    edtColMax->setCheckBounds(false, false);
    QHBoxLayout* coll=new QHBoxLayout();
    coll->addWidget(edtColMin,1);
    coll->addWidget(new QLabel(" ... "));
    coll->addWidget(edtColMax,1);
    coll->setContentsMargins(0,0,0,0);
    gli->addRow(QString(""), coll);

    cmbOutOfRangeMode=new QComboBox(this);
    cmbOutOfRangeMode->addItem(tr("min/max color"));
    cmbOutOfRangeMode->addItem(tr("black"));
    cmbOutOfRangeMode->addItem(tr("white"));
    cmbOutOfRangeMode->addItem(tr("transparent"));
    cmbOutOfRangeMode->addItem(tr("red"));
    cmbOutOfRangeMode->addItem(tr("green"));
    cmbOutOfRangeMode->addItem(tr("blue"));
    gli->addRow(tr("out-&of-range mode:"), cmbOutOfRangeMode);


}
