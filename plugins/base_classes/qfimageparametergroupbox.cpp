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



#include "qfimageparametergroupbox.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include "qfrawdatarecord.h"
#include "statistics_tools.h"

QFImageParameterGroupBox::QFImageParameterGroupBox(QWidget *parent) :
    QGroupBox(parent)
{
    initGrp();
}

QFImageParameterGroupBox::QFImageParameterGroupBox(const QString &title, QWidget *parent):
    QGroupBox(title, parent)
{
    initGrp();
}

void QFImageParameterGroupBox::addAction(QAction *act)
{
    QGroupBox::addAction(act);

    cmbColorbar->setContextMenuPolicy(Qt::ActionsContextMenu);;
    cmbImageStyle->setContextMenuPolicy(Qt::ActionsContextMenu);;
    cmbOutOfRangeMode->setContextMenuPolicy(Qt::ActionsContextMenu);;
    chkImageAutoScale->setContextMenuPolicy(Qt::ActionsContextMenu);;
    edtColMin->setContextMenuPolicy(Qt::ActionsContextMenu);;
    edtColMax->setContextMenuPolicy(Qt::ActionsContextMenu);;

    cmbColorbar->addAction(act);
    cmbImageStyle->addAction(act);
    cmbOutOfRangeMode->addAction(act);
    chkImageAutoScale->addAction(act);
    edtColMin->addAction(act);
    edtColMax->addAction(act);
}

void QFImageParameterGroupBox::setSelectedMaskStyle(JKQTPOverlayImageEnhanced *plot) const
{
    QColor col=maskColors.value(cmbImageStyle->currentIndex(), QColor("red"));
    plot->set_trueColor(col);
    plot->set_symbolSizeFactor(0.7);
    plot->set_drawAsRectangles(true);
    plot->set_symbol(JKQTPnoSymbol);
    plot->set_falseColor(Qt::transparent);
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

void QFImageParameterGroupBox::setSelectedImageStyle(JKQTPMathImage *plteImage, bool* plteOverviewExcludedData, bool emitSignals)
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
            plteImage->set_rangeMinFailAction(JKQTPMathImageLastPaletteColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImageLastPaletteColor);
            break;
        default:
        case 1:
            plteImage->set_rangeMinFailAction(JKQTPMathImageGivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImageGivenColor);
            plteImage->set_rangeMinFailColor(QColor("black"));
            plteImage->set_rangeMaxFailColor(QColor("black"));
            break;
        case 2:
            plteImage->set_rangeMinFailAction(JKQTPMathImageGivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImageGivenColor);
            plteImage->set_rangeMinFailColor(QColor("white"));
            plteImage->set_rangeMaxFailColor(QColor("white"));
            break;
        case 3:
            plteImage->set_rangeMinFailAction(JKQTPMathImageTransparent);
            plteImage->set_rangeMaxFailAction(JKQTPMathImageTransparent);
            break;
        case 4:
            plteImage->set_rangeMinFailAction(JKQTPMathImageGivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImageGivenColor);
            plteImage->set_rangeMinFailColor(QColor("red"));
            plteImage->set_rangeMaxFailColor(QColor("red"));
            break;
        case 5:
            plteImage->set_rangeMinFailAction(JKQTPMathImageGivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImageGivenColor);
            plteImage->set_rangeMinFailColor(QColor("green"));
            plteImage->set_rangeMaxFailColor(QColor("green"));
            break;
        case 6:
            plteImage->set_rangeMinFailAction(JKQTPMathImageGivenColor);
            plteImage->set_rangeMaxFailAction(JKQTPMathImageGivenColor);
            plteImage->set_rangeMinFailColor(QColor("blue"));
            plteImage->set_rangeMaxFailColor(QColor("blue"));
            break;
    }

    edtColMin->setEnabled(!chkImageAutoScale->isChecked());
    edtColMax->setEnabled(!chkImageAutoScale->isChecked());
    if (chkImageAutoScale->isChecked()) {
        //double mi=0, ma=0;
        //if (mi==ma) plteImage->getDataMinMax(mi, ma);
        //qDebug()<<"mi="<<mi<<" ma="<<ma<<" edtMin="<<edtColMin->value()<<" edtMax="<<edtColMax->value();
        //qDebug()<<"mi="<<mi-edtColMin->value()<<" ma="<<ma-edtColMax->value();
        if (edtColMin->value()!=mi || edtColMax->value()!=ma) {
            disconnect(edtColMax, SIGNAL(valueChanged(double)), this, SLOT(emitSettingsChanged()));
            disconnect(edtColMin, SIGNAL(valueChanged(double)), this, SLOT(emitSettingsChanged()));
            edtColMin->setValue(mi);
            edtColMax->setValue(ma);
            connect(edtColMax, SIGNAL(valueChanged(double)), this, SLOT(emitSettingsChanged()));
            connect(edtColMin, SIGNAL(valueChanged(double)), this, SLOT(emitSettingsChanged()));
            if (emitSignals) emit settingsChanged();
        }
    }
    plteImage->set_imageMin(edtColMin->value());
    plteImage->set_imageMax(edtColMax->value());

}

int QFImageParameterGroupBox::getMaskStyle() const
{
    return cmbImageStyle->currentIndex();
}

void QFImageParameterGroupBox::setMaskStyle(int idx)
{
    cmbImageStyle->setCurrentIndex(idx);
}

bool QFImageParameterGroupBox::isColorAutoscale() const
{
    return chkImageAutoScale->isChecked();
}

void QFImageParameterGroupBox::setColorAutoscale(bool enable)
{
    chkImageAutoScale->setChecked(enable);
}

double QFImageParameterGroupBox::getColMin() const
{
    return edtColMin->value();
}

double QFImageParameterGroupBox::getColMax() const
{
    return edtColMax->value();
}

void QFImageParameterGroupBox::setColMin(double value)
{
    edtColMin->setValue(value);
}

void QFImageParameterGroupBox::setColMax(double value)
{
    edtColMax->setValue(value);
}

int QFImageParameterGroupBox::getImageStyle() const
{
    return cmbImageStyle->currentIndex();
}

bool QFImageParameterGroupBox::getImageStyleDisplayMask() const
{
    return cmbImageStyle->currentIndex()<cmbImageStyle->count()-1;

}

void QFImageParameterGroupBox::setImageStyle(int i)
{
    cmbImageStyle->setCurrentIndex(i);
}

int QFImageParameterGroupBox::getOutOfRangeMode() const
{
    return cmbOutOfRangeMode->currentIndex();
}

void QFImageParameterGroupBox::setOutOfRangeMode(int i)
{
    cmbOutOfRangeMode->setCurrentIndex(i);
}

QString QFImageParameterGroupBox::getColorbarName() const
{
    return cmbColorbar->currentText();
}

QString QFImageParameterGroupBox::getImageStyleName() const
{
    return cmbImageStyle->currentText();
}

QString QFImageParameterGroupBox::getOutOfRangeName() const
{
    return cmbOutOfRangeMode->currentText();
}

JKQTPMathImageColorPalette QFImageParameterGroupBox::colorPalette() const
{
    return cmbColorbar->colorPalette();
}

void QFImageParameterGroupBox::setColorPalette(JKQTPMathImageColorPalette palette)
{
    cmbColorbar->setColorPalette(palette);
}

void QFImageParameterGroupBox::saveConfig(QFRawDataRecord *current, const QString &/*egroup*/, const QString &param, const QString &imageID)
{
    current->setQFProperty(QString("imfcs_imed_colorbar_%1_%2").arg(param).arg(imageID), cmbColorbar->currentIndex(), false, false);
    current->setQFProperty(QString("imfcs_imed_style_%1_%2").arg(param).arg(imageID), cmbImageStyle->currentIndex(), false, false);
    current->setQFProperty(QString("imfcs_imed_oorc_%1_%2").arg(param).arg(imageID), cmbOutOfRangeMode->currentIndex(), false, false);
    current->setQFProperty(QString("imfcs_imed_autorange_%1_%2").arg(param).arg(imageID), chkImageAutoScale->isChecked(), false, false);
    if (!chkImageAutoScale->isChecked()) {
        current->setQFProperty(QString("imfcs_imed_colmin_%1_%2").arg(param).arg(imageID), edtColMin->value(), false, false);
        current->setQFProperty(QString("imfcs_imed_colmax_%1_%2").arg(param).arg(imageID), edtColMax->value(), false, false);
    }

}

void QFImageParameterGroupBox::loadConfig(QFRawDataRecord *current, const QString &egroup, const QString &param, const QString &imageID, const QString &prefix, double mi, double ma)
{

    int d=current->getQFPropertyHirarchy2(QString("imfcs_imed_colorbar_%1_%2").arg(param).arg(imageID),QString("imfcs_imed_colorbar_%1_%3_%2").arg(egroup).arg(param).arg(imageID),
                               current->getProperty(QString("imfcs_imed_colorbar_%1_%2").arg(egroup).arg(param),
                               ProgramOptions::getInstance()->getQSettings()->value(prefix+QString("colorbar"), cmbColorbar->currentIndex()))).toInt();
    if (d>=0) cmbColorbar->setCurrentIndex(d);
    else if (cmbColorbar->count()>0) cmbColorbar->setColorPalette(JKQTPMathImageMATLAB);
    cmbImageStyle->setCurrentIndex(current->getQFPropertyHirarchy2(QString("imfcs_imed_style_%1_%2").arg(param).arg(imageID),QString("imfcs_imed_style_%1_%3_%2").arg(egroup).arg(param).arg(imageID), current->getProperty(QString("imfcs_imed_style_%1_%2").arg(egroup).arg(param), cmbImageStyle->currentIndex())).toInt());
    cmbOutOfRangeMode->setCurrentIndex(current->getQFPropertyHirarchy2(QString("imfcs_imed_oorc_%1_%2").arg(param).arg(imageID),QString("imfcs_imed_oorc_%1_%3_%2").arg(egroup).arg(param).arg(imageID), current->getProperty(QString("imfcs_imed_oorc_%1_%2").arg(egroup).arg(param), cmbOutOfRangeMode->currentIndex())).toInt());
    chkImageAutoScale->setChecked(current->getQFPropertyHirarchy2(QString("imfcs_imed_autorange_%1_%2").arg(param).arg(imageID),QString("imfcs_imed_autorange_%1_%3_%2").arg(egroup).arg(param).arg(imageID), current->getProperty(QString("imfcs_imed_autorange_%1_%2").arg(egroup).arg(param), true)).toBool());
    if (!chkImageAutoScale->isChecked()) {
        edtColMin->setValue(current->getQFPropertyHirarchy2(QString("imfcs_imed_colmin_%1_%2").arg(param).arg(imageID),QString("imfcs_imed_colmin_%1_%3_%2").arg(egroup).arg(param).arg(imageID), current->getProperty(QString("imfcs_imed_colmin_%1_%2").arg(egroup).arg(param), mi)).toDouble());
        edtColMax->setValue(current->getQFPropertyHirarchy2(QString("imfcs_imed_colmax_%1_%2").arg(param).arg(imageID),QString("imfcs_imed_colmax_%1_%3_%2").arg(egroup).arg(param).arg(imageID), current->getProperty(QString("imfcs_imed_colmax_%1_%2").arg(egroup).arg(param), ma)).toDouble());
    }

}

void QFImageParameterGroupBox::writeSettings(QSettings &settings, const QString &prefix) const
{
    settings.setValue(prefix+QString("colorbar"), cmbColorbar->currentIndex());
    settings.setValue(prefix+QString("paramstyle"), cmbImageStyle->currentIndex());
    settings.setValue(prefix+QString("outofrange_mode"), cmbOutOfRangeMode->currentIndex());
}

void QFImageParameterGroupBox::readSettings(QSettings &settings, const QString &prefix)
{
    cmbColorbar->setCurrentIndex(settings.value(prefix+QString("colorbar"),  JKQTPMathImageMATLAB).toInt());
    cmbImageStyle->setCurrentIndex(settings.value(prefix+QString("paramstyle"), 0).toInt());
    cmbOutOfRangeMode->setCurrentIndex(settings.value(prefix+QString("outofrange_mode"), 1).toInt());

}

void QFImageParameterGroupBox::connectWidgets(bool connectTo)
{
    if (connectTo) {
        connect(cmbColorbar, SIGNAL(currentIndexChanged(int)), this, SLOT(emitSettingsChanged()));
        connect(cmbImageStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(emitSettingsChanged()));
        connect(cmbOutOfRangeMode, SIGNAL(currentIndexChanged(int)), this, SLOT(emitSettingsChanged()));
        connect(chkImageAutoScale, SIGNAL(toggled(bool)), this, SLOT(emitSettingsChanged()));
        connect(edtColMax, SIGNAL(valueChanged(double)), this, SLOT(emitSettingsChanged()));
        connect(edtColMin, SIGNAL(valueChanged(double)), this, SLOT(emitSettingsChanged()));
    } else {
        disconnect(cmbColorbar, SIGNAL(currentIndexChanged(int)), this, SLOT(emitSettingsChanged()));
        disconnect(cmbImageStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(emitSettingsChanged()));
        disconnect(cmbOutOfRangeMode, SIGNAL(currentIndexChanged(int)), this, SLOT(emitSettingsChanged()));
        disconnect(chkImageAutoScale, SIGNAL(toggled(bool)), this, SLOT(emitSettingsChanged()));
        disconnect(edtColMax, SIGNAL(valueChanged(double)), this, SLOT(emitSettingsChanged()));
        disconnect(edtColMin, SIGNAL(valueChanged(double)), this, SLOT(emitSettingsChanged()));
    }
}

void QFImageParameterGroupBox::emitSettingsChanged()
{
    edtColMin->setEnabled(!chkImageAutoScale->isChecked());
    edtColMax->setEnabled(!chkImageAutoScale->isChecked());
    emit settingsChanged();
}

void QFImageParameterGroupBox::initGrp()
{
    setContextMenuPolicy(Qt::ActionsContextMenu);
    setFlat(true);

    QFormLayout* gli=new QFormLayout();
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


    cmbColorbar=new JKQTPMathImageColorPaletteComboBox(this);
    cmbColorbar->setColorPalette(JKQTPMathImageMATLAB);
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
