/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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



#include "qfvisualize3dgleditor.h"
#include "ui_qfvisualize3dgleditor.h"
#include <cmath>
#include <QToolTip>

QString QFVisualize3DGLEditor::CDoubleToQString(double value) {
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    return loc.toString(value, 'g', 10);
}

double QFVisualize3DGLEditor::CQStringToDouble(QString value) {
    QLocale loc=QLocale::c();
    loc.setNumberOptions(QLocale::OmitGroupSeparator);
    return loc.toDouble(value) ;
}


QString QFVisualize3DGLEditor::boolToQString(bool data) {
    if (data) return QString("true");
    return QString("false");
}

bool QFVisualize3DGLEditor::QStringToBool(const QString& data){
    QString d=data.toLower();
    if (d=="true") return true;
    if (d=="t") return true;
    if (d=="1") return true;
    if (d=="j") return true;
    if (d=="y") return true;
    if (d=="yes") return true;
    if (d=="ja") return true;
    bool ok=false;
    int i=d.toInt(&ok);
    if (ok) return i!=0;
    return false;
}

QColor QFVisualize3DGLEditor::QStringToQColor(const QString& data) {
    QString d=data;
    if (d.startsWith("#")) d=d.replace(' ', '0');
    return QColor(d);
}

QFVisualize3DGLEditor::QFVisualize3DGLEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFVisualize3DGLEditor)
{
    defaultPalettes<<JKQTPMathImageGRAY;
    dataCounter=0;

    ui->setupUi(this);
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(0,0);
    /*QList<int> widths;
    widths<<width()*3/4<<width()/4;
    ui->splitter->setSizes(widths);*/


    ui->widget2->setCurrentVData(0);
    ui->cmbCurrentGeometry->setCurrentIndex(-1);

    ui->lutAlphaEditor->setAlphaMode(true);
    ui->spinAlphaLower->setRange(0,1);
    ui->spinAlphaUpper->setRange(0,1);

    ui->cmbAmbient->setCurrentColor(ui->widget2->ambientColor());
    ui->cmbBackground->setCurrentColor(ui->widget2->backgroundColor());
    ui->cmbBlend->setCurrentIndex((int)ui->widget2->blendMode());
    ui->cmbInterpolation->setCurrentIndex((int)ui->widget2->interpolationMode());


    ui->spinRotX->setValue(ui->widget2->xRotation());
    ui->spinRotY->setValue(ui->widget2->yRotation());
    ui->spinRotZ->setValue(ui->widget2->zRotation());

    ui->lutAlphaEditor->setLogHistogram(true);
    ui->lutEditor->setLogHistogram(true);

    updateGlobalWidgets();

    setCurrent(0);
    connect(ui->cmbCurrentGeometry, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrent(int)));

    connect(ui->chkCutPlane1, SIGNAL(toggled(bool)), ui->widCP1, SLOT(setEnabled(bool)));
    connect(ui->chkCutPlane2, SIGNAL(toggled(bool)), ui->widCP2, SLOT(setEnabled(bool)));
    connect(ui->chkVisible, SIGNAL(toggled(bool)), ui->grpAlpha, SLOT(setEnabled(bool)));
    connect(ui->chkVisible, SIGNAL(toggled(bool)), ui->grpBasics, SLOT(setEnabled(bool)));
    connect(ui->chkVisible, SIGNAL(toggled(bool)), ui->grpColor, SLOT(setEnabled(bool)));


}

QFVisualize3DGLEditor::~QFVisualize3DGLEditor()
{
#ifdef SAVE_CONFIG_VIZ3D
    QSettings set("viz3d.ini", QSettings::IniFormat);
    writeSettings(set);
    set.setValue("config", getConfig());
#endif
    delete ui;
    for (int i=0; i<data.size(); i++) {
        data[i].free();
    }

}



void QFVisualize3DGLEditor::removeVolumeData(int idx)
{
    if (idx>=0 && idx<data.size()) {
        ui->cmbCurrentGeometry->removeItem(idx);
        data[idx].free();
        data.removeAt(idx);
        ui->cmbCurrentGeometry->setCurrentIndex(qMax(0,idx-1));
        setCurrent(qMax(0,idx-1));
    }
}

void QFVisualize3DGLEditor::clearVolumeData()
{
    ui->cmbCurrentGeometry->clear();
    for (int i=0; i<data.size(); i++) {
        data[i].free();
    }
    data.clear();;
    setCurrent(-1);
    ui->widget2->clearAllVolumeData();
}

void QFVisualize3DGLEditor::initInternalState()
{
    mapVizMode[dataCounter]=QFVisualize3DGLWidget::TextureRendering;
    mapColorPal[dataCounter]=JKQTPMathImageGRAY;
    if (dataCounter<defaultPalettes.size()) mapColorPal[dataCounter]=defaultPalettes[dataCounter];

    //mapLUT[dataCounter]="";//ui->lutEditor->getDefaultConfig();
    //mapLUTAlpha[dataCounter]="";//ui->lutAlphaEditor->getDefaultConfig();


}

void QFVisualize3DGLEditor::addVolumeData(float *data, int sizeX, int sizeY, int sizeZ, double wx, double wy, double wz, const QString &name)
{
    volumeData vd;
    vd.data=data;
    vd.sizeX=sizeX;
    vd.sizeY=sizeY;
    vd.sizeZ=sizeZ;
    vd.wx=wx;
    vd.wy=wy;
    vd.wz=wz;
    vd.name=name;
    vd.internal=false;
    this->data.append(vd);

    initInternalState();
    ui->cmbCurrentGeometry->addItem(QString("%2 (%1)").arg(dataCounter).arg(name), dataCounter);
    ui->cmbCurrentGeometry->setCurrentIndex(this->data.size()-1);
    setCurrent(this->data.size()-1);

    dataCounter++;
}

void QFVisualize3DGLEditor::addCopiedVolumeData(float *data, int sizeX, int sizeY, int sizeZ, double wx, double wy, double wz, const QString &name)
{
    volumeData vd;
    vd.data=(float*)malloc(sizeX*sizeY*sizeZ*sizeof(float));
    vd.sizeX=sizeX;
    vd.sizeY=sizeY;
    vd.sizeZ=sizeZ;
    for (int i=0; i<sizeX*sizeY*sizeZ; i++) {
        vd.data[i]=data[i];
    }
    vd.wx=wx;
    vd.wy=wy;
    vd.wz=wz;
    vd.name=name;
    vd.internal=true;
    this->data.append(vd);

    initInternalState();
    ui->cmbCurrentGeometry->addItem(QString("%2 (%1)").arg(dataCounter).arg(name), dataCounter);
    ui->cmbCurrentGeometry->setCurrentIndex(this->data.size()-1);
    setCurrent(this->data.size()-1);

    dataCounter++;
}


void QFVisualize3DGLEditor::addCopiedVolumeData(double *data, int sizeX, int sizeY, int sizeZ, double wx, double wy, double wz, const QString &name)
{
    volumeData vd;
    vd.data=(float*)malloc(sizeX*sizeY*sizeZ*sizeof(float));
    vd.sizeX=sizeX;
    vd.sizeY=sizeY;
    vd.sizeZ=sizeZ;
    for (int i=0; i<sizeX*sizeY*sizeZ; i++) {
        vd.data[i]=data[i];
    }
    vd.wx=wx;
    vd.wy=wy;
    vd.wz=wz;
    vd.name=name;
    vd.internal=true;
    this->data.append(vd);

    //qDebug()<<"added: "<<vd.data<<"  count="<<this->data.size()<<"  dataCounter="<<dataCounter<<"  data="<<data<<" size="<<sizeX<<sizeY<<sizeZ;

    initInternalState();
    ui->cmbCurrentGeometry->addItem(QString("%2 (%1)").arg(dataCounter).arg(name), dataCounter);
    ui->cmbCurrentGeometry->setCurrentIndex(this->data.size()-1);
    setCurrent(this->data.size()-1);

    dataCounter++;
    //qDebug()<<"added: FINISHED!";
}

void QFVisualize3DGLEditor::addCopiedVolumeData(uint8_t *data, int sizeX, int sizeY, int sizeZ, double wx, double wy, double wz, const QString &name)
{
    volumeData vd;
    vd.data=(float*)malloc(sizeX*sizeY*sizeZ*sizeof(float));
    vd.sizeX=sizeX;
    vd.sizeY=sizeY;
    vd.sizeZ=sizeZ;
    for (int i=0; i<sizeX*sizeY*sizeZ; i++) {
        vd.data[i]=data[i];
    }
    vd.wx=wx;
    vd.wy=wy;
    vd.wz=wz;
    vd.name=name;
    vd.internal=true;
    this->data.append(vd);

    initInternalState();
    ui->cmbCurrentGeometry->addItem(QString("%2 (%1)").arg(dataCounter).arg(name), dataCounter);
    ui->cmbCurrentGeometry->setCurrentIndex(this->data.size()-1);
    setCurrent(this->data.size()-1);

    dataCounter++;

}

void QFVisualize3DGLEditor::addCopiedVolumeData(uint16_t *data, int sizeX, int sizeY, int sizeZ, double wx, double wy, double wz, const QString &name)
{
    volumeData vd;
    vd.data=(float*)malloc(sizeX*sizeY*sizeZ*sizeof(float));
    vd.sizeX=sizeX;
    vd.sizeY=sizeY;
    vd.sizeZ=sizeZ;
    for (int i=0; i<sizeX*sizeY*sizeZ; i++) {
        vd.data[i]=data[i];
    }
    vd.wx=wx;
    vd.wy=wy;
    vd.wz=wz;
    vd.name=name;
    vd.internal=true;
    this->data.append(vd);

    initInternalState();
    ui->cmbCurrentGeometry->addItem(QString("%2 (%1)").arg(dataCounter).arg(name), dataCounter);
    ui->cmbCurrentGeometry->setCurrentIndex(this->data.size()-1);
    setCurrent(this->data.size()-1);

    dataCounter++;

}

void QFVisualize3DGLEditor::addCopiedVolumeData(uint32_t *data, int sizeX, int sizeY, int sizeZ, double wx, double wy, double wz, const QString &name)
{
    volumeData vd;
    vd.data=(float*)malloc(sizeX*sizeY*sizeZ*sizeof(float));
    vd.sizeX=sizeX;
    vd.sizeY=sizeY;
    vd.sizeZ=sizeZ;
    for (int i=0; i<sizeX*sizeY*sizeZ; i++) {
        vd.data[i]=data[i];
    }
    vd.wx=wx;
    vd.wy=wy;
    vd.wz=wz;
    vd.name=name;
    vd.internal=true;
    this->data.append(vd);

    initInternalState();
    ui->cmbCurrentGeometry->addItem(QString("%2 (%1)").arg(dataCounter).arg(name), dataCounter);
    ui->cmbCurrentGeometry->setCurrentIndex(this->data.size()-1);
    setCurrent(this->data.size()-1);

    dataCounter++;

}

void QFVisualize3DGLEditor::addCopiedVolumeData(uint64_t *data, int sizeX, int sizeY, int sizeZ, double wx, double wy, double wz, const QString &name)
{
    volumeData vd;
    vd.data=(float*)malloc(sizeX*sizeY*sizeZ*sizeof(float));
    vd.sizeX=sizeX;
    vd.sizeY=sizeY;
    vd.sizeZ=sizeZ;
    for (int i=0; i<sizeX*sizeY*sizeZ; i++) {
        vd.data[i]=data[i];
    }
    vd.wx=wx;
    vd.wy=wy;
    vd.wz=wz;
    vd.name=name;
    vd.internal=true;
    this->data.append(vd);

    initInternalState();
    ui->cmbCurrentGeometry->addItem(QString("%2 (%1)").arg(dataCounter).arg(name), dataCounter);
    ui->cmbCurrentGeometry->setCurrentIndex(this->data.size()-1);
    setCurrent(this->data.size()-1);

    dataCounter++;

}

int QFVisualize3DGLEditor::getCurrentGeometryID() const
{
    return getCurrentGeometryID(ui->cmbCurrentGeometry->currentIndex());
}

int QFVisualize3DGLEditor::getCurrentGeometryID(int i) const
{
    if (i<0 || i>=ui->cmbCurrentGeometry->count()) return -1;
    return ui->cmbCurrentGeometry->itemData(i).toInt();
}

void QFVisualize3DGLEditor::addDefaultPalette(JKQTPMathImageColorPalette pal)
{
    defaultPalettes<<pal;
}

void QFVisualize3DGLEditor::setDefaultPalette(JKQTPMathImageColorPalette pal)
{
    defaultPalettes.clear();
    defaultPalettes<<pal;
}

void QFVisualize3DGLEditor::setDefaultPalettes(JKQTPMathImageColorPalette pal, JKQTPMathImageColorPalette pal1)
{
    defaultPalettes<<pal<<pal1;
}

void QFVisualize3DGLEditor::setDefaultPalettes(JKQTPMathImageColorPalette pal, JKQTPMathImageColorPalette pal1, JKQTPMathImageColorPalette pal3)
{
    defaultPalettes<<pal<<pal1<<pal3;
}

void QFVisualize3DGLEditor::setDefaultPalettes(JKQTPMathImageColorPalette pal, JKQTPMathImageColorPalette pal1, JKQTPMathImageColorPalette pal3, JKQTPMathImageColorPalette pal4)
{
    defaultPalettes<<pal<<pal1<<pal3<<pal4;
}

void QFVisualize3DGLEditor::clearDefaultPalettes()
{
    defaultPalettes.clear();
}

QString QFVisualize3DGLEditor::getConfig() const
{
    QStringList config;
    //  number of volumes
    config.append(QString::number(getVolumeDataCount()));
    // header size
    config.append(QString::number(9));
    // size of a volume record
    config.append(QString::number(15));
    // background color
    config.append(ui->widget2->backgroundColor().name());
    // ambient color
    config.append(ui->widget2->ambientColor().name());
    // blending
    config.append(QString::number((int)ui->widget2->blendMode()));
    // interpolation
    config.append(QString::number((int)ui->widget2->interpolationMode()));
    // bounding boxes visible
    config.append(boolToQString(ui->widget2->getShowBounds()));
    // x-angle
    config.append(CDoubleToQString(ui->widget2->xRotation()));
    // y-angle
    config.append(CDoubleToQString(ui->widget2->yRotation()));
    // z-angle
    config.append(CDoubleToQString(ui->widget2->zRotation()));

    for (int i=0; i<getVolumeDataCount(); i++) {
        int vid=getCurrentGeometryID(i);
        ui->widget2->setCurrentVData(i);
        config.append(QString::number(mapVizMode.value(vid, QFVisualize3DGLWidget::TextureRendering)));
        config.append(mapLUT.value(vid, ui->lutEditor->getDefaultConfig()));
        config.append(mapLUTAlpha.value(vid, ui->lutAlphaEditor->getDefaultConfig()));
        config.append(QString::number((int)mapColorPal.value(vid, JKQTPMathImageGRAY)));
        config.append(CDoubleToQString(qBound(0.0,ui->widget2->getTransparentLowerThreshold(), 1.0)));
        config.append(CDoubleToQString(qBound(0.0,ui->widget2->getTransparentUpperThreshold(), 1.0)));
        config.append(boolToQString(ui->widget2->getGeoVisible()));
        config.append(boolToQString(ui->widget2->cutPlane1Enabled()));
        config.append(QString::number((int)ui->widget2->cutPlane1Type()));
        config.append(QString::number((int)ui->widget2->cutPlane1Position()));
        config.append(boolToQString(ui->widget2->cutPlane2Enabled()));
        config.append(QString::number((int)ui->widget2->cutPlane2Type()));
        config.append(QString::number((int)ui->widget2->cutPlane2Position()));
        config.append(boolToQString(ui->widget2->singleColor()));
        config.append(ui->widget2->singleColorColor().name());
    }
    ui->widget2->setCurrentVData(ui->cmbCurrentGeometry->currentIndex());
    return config.join("\n");
}

void QFVisualize3DGLEditor::setConfig(const QString &configIn)
{
    //qDebug()<<"setConfig("<<configIn<<") ...";
    QStringList config=configIn.split("\n");
    if (config.size()<=0) return;
    bool ok=false;
    int cnt=config.value(0, "0").toInt(&ok);
    int header=config.value(1, "1").toInt(&ok);
    int delta=config.value(2, "1").toInt(&ok);
    bool upd=ui->widget2->get_doUpdate();
    ui->widget2->set_doUpdate(false);
    ui->widget2->setBackgroundColor(QStringToQColor(config.value(3, ui->widget2->backgroundColor().name())));
    ui->widget2->setAmbientColor(QStringToQColor(config.value(4, ui->widget2->ambientColor().name())));
    ui->widget2->setBlendMode(config.value(5, "0").toInt());
    ui->widget2->setInterpolationMode(config.value(6, "0").toInt());
    ui->widget2->setShowBounds(QStringToBool(config.value(7, "true")));
    ui->widget2->setXRotation(CQStringToDouble(config.value(8, "35")));
    ui->widget2->setYRotation(CQStringToDouble(config.value(9, "312")));
    ui->widget2->setZRotation(CQStringToDouble(config.value(10, "0")));


    if (ok) {
        int idx=2+header;
        for (int i=0; i<cnt; i++) {
            int vid=getCurrentGeometryID(i);
            //qDebug()<<"  volume "<<i<<": vid="<<vid;
            mapVizMode[vid]=(QFVisualize3DGLWidget::VisualizationMode)config.value(idx, "0").toInt();
            mapLUT[vid]=config.value(idx+1, ui->lutEditor->getDefaultConfig());
            mapLUTAlpha[vid]=config.value(idx+2, ui->lutAlphaEditor->getDefaultConfig());
            mapColorPal[vid]=(JKQTPMathImageColorPalette)config.value(idx+3, QString::number(JKQTPMathImageGRAY)).toInt();
            //qDebug()<<"  mapLUT "<<mapLUT[vid];
            //qDebug()<<"  mapLUTAlpha "<<mapLUTAlpha[vid];
            //qDebug()<<"  mapColorPal "<<mapColorPal[vid];
            if (vid>=0) {
                ui->widget2->setCurrentVData(i);
                ui->widget2->setLUT(ui->lutEditor->getLUT(mapLUT[vid], 4096, mapColorPal[vid]));
                ui->widget2->setAlphaLUT(ui->lutAlphaEditor->getAlphaLUT(mapLUTAlpha[vid], 4096));
                ui->widget2->setTransparentLowerThreshold(CQStringToDouble(config.value(idx+4, 0)));
                ui->widget2->setTransparentUpperThreshold(CQStringToDouble(config.value(idx+5, 0)));
                ui->widget2->setGeoVisible(QStringToBool(config.value(idx+6, 0)));
                if (delta>=10) ui->widget2->setCutPlane1Enabled(QStringToBool(config.value(idx+7, "false")));
                else ui->widget2->setCutPlane1Enabled(false);
                if (delta>=10) ui->widget2->setCutPlane1Type(config.value(idx+8, "0").toInt());
                else ui->widget2->setCutPlane1Type(0);
                if (delta>=10) ui->widget2->setCutPlane1Position(config.value(idx+9, "50").toInt());
                else ui->widget2->setCutPlane1Position(50);
                if (delta>=13) ui->widget2->setCutPlane2Enabled(QStringToBool(config.value(idx+10, "false")));
                else ui->widget2->setCutPlane2Enabled(false);
                if (delta>=13) ui->widget2->setCutPlane2Type(config.value(idx+11, "0").toInt());
                else ui->widget2->setCutPlane2Type(0);
                if (delta>=13) ui->widget2->setCutPlane2Position(config.value(idx+12, "50").toInt());
                else ui->widget2->setCutPlane2Position(50);
                if (delta>=15) ui->widget2->setSingleColor(QStringToBool(config.value(idx+13, "false")));
                else ui->widget2->setSingleColor(false);
                if (delta>=15) ui->widget2->setColor(QColor(config.value(idx+14, "grey")));
                else ui->widget2->setColor(QColor("grey"));
            }
            idx=idx+delta;
        }
    }
    updateGlobalWidgets();
    setCurrent(ui->cmbCurrentGeometry->currentIndex());
    ui->widget2->set_doUpdate(upd);
    //qDebug()<<"setConfig("<<configIn<<") ... DONE!";
}

void QFVisualize3DGLEditor::writeSettings(QSettings &settings, const QString &prefix) const
{
    settings.setValue(prefix+"splitter_sizes", ui->splitter->saveState());
}

void QFVisualize3DGLEditor::readSettings(QSettings &settings, const QString &prefix)
{
    ui->splitter->restoreState(settings.value(prefix+"splitter_sizes", ui->splitter->saveState()).toByteArray());
}

QFVisualize3DGLWidget *QFVisualize3DGLEditor::get3DWidget() const
{
    return ui->widget2;
}

bool QFVisualize3DGLEditor::get_doUpdate() const
{
    return ui->widget2->get_doUpdate();
}

void QFVisualize3DGLEditor::set_doUpdate(bool enabled)
{
    if (enabled) {
        update3DDataset();
    }
    ui->widget2->set_doUpdate(enabled);
}

QImage QFVisualize3DGLEditor::getImage() const
{
    return ui->widget2->getImage();
}

void QFVisualize3DGLEditor::update3DDataset()
{
    int idx=ui->cmbCurrentGeometry->currentIndex();
    if (idx>=0 && idx<data.size()) {
        ui->lutEditor->setHistogram(data[idx].data, data[idx].size());
        ui->lutAlphaEditor->setHistogram(data[idx].data, data[idx].size());

        while (ui->widget2->getVolumeDataCount()>data.size()) {
            ui->widget2->deleteVolumeData(ui->widget2->getVolumeDataCount()-1);
        }

        for (int i=0; i<data.size(); i++) {
            int id=getCurrentGeometryID(i);
            if (i<ui->widget2->getVolumeDataCount()) {
                ui->widget2->setCurrentVData(i);
                ui->widget2->setVolumeData(data[i].data, data[i].sizeX, data[i].sizeY, data[i].sizeZ, mapVizMode.value(id, QFVisualize3DGLWidget::TextureRendering), data[i].wx, data[i].wy, data[i].wz, false);
            } else {
                ui->widget2->addVolumeData(data[i].data, data[i].sizeX, data[i].sizeY, data[i].sizeZ, mapVizMode.value(id, QFVisualize3DGLWidget::TextureRendering), data[i].wx, data[i].wy, data[i].wz, false);
                ui->widget2->setCurrentVData(i);
            }
            /*setCurrent(i);
            ui->widget2->setLUT(ui->lutEditor->getCurrentLUT());
            ui->widget2->setAlphaLUT(ui->lutAlphaEditor->getCurrentLUT());
            ui->widget2->setTransparentLowerThreshold(ui->spinAlphaLower->value());
            ui->widget2->setTransparentUpperThreshold(ui->spinAlphaUpper->value());
            ui->widget2->setGeoVisible(ui->chkVisible->isChecked());*/

        }
        ui->widget2->setCurrentVData(idx);
    }
}

void QFVisualize3DGLEditor::updateGlobalWidgets()
{
    disconnect(ui->widget2, SIGNAL(xRotationChanged(double)), ui->spinRotX, SLOT(setValue(double)));
    disconnect(ui->spinRotX, SIGNAL(valueChanged(double)), ui->widget2, SLOT(setXRotation(double)));
    disconnect(ui->widget2, SIGNAL(yRotationChanged(double)), ui->spinRotY, SLOT(setValue(double)));
    disconnect(ui->spinRotY, SIGNAL(valueChanged(double)), ui->widget2, SLOT(setYRotation(double)));
    disconnect(ui->widget2, SIGNAL(zRotationChanged(double)), ui->spinRotZ, SLOT(setValue(double)));
    disconnect(ui->spinRotZ, SIGNAL(valueChanged(double)), ui->widget2, SLOT(setZRotation(double)));

    disconnect(ui->cmbBackground, SIGNAL( currentColorChanged(const QColor &)), ui->widget2, SLOT(setBackgroundColor(QColor)));
    disconnect(ui->cmbAmbient, SIGNAL( currentColorChanged(const QColor &)), ui->widget2, SLOT(setAmbientColor(QColor)));
    disconnect(ui->cmbBlend, SIGNAL( currentIndexChanged(int)), ui->widget2, SLOT(setBlendMode(int)));
    disconnect(ui->cmbInterpolation, SIGNAL( currentIndexChanged(int)), ui->widget2, SLOT(setInterpolationMode(int)));




    ui->cmbAmbient->setCurrentColor(ui->widget2->ambientColor());
    ui->cmbBackground->setCurrentColor(ui->widget2->backgroundColor());
    ui->cmbBlend->setCurrentIndex((int)ui->widget2->blendMode());
    ui->cmbInterpolation->setCurrentIndex((int)ui->widget2->interpolationMode());

    ui->spinRotX->setValue(ui->widget2->xRotation());
    ui->spinRotY->setValue(ui->widget2->yRotation());
    ui->spinRotZ->setValue(ui->widget2->zRotation());




    connect(ui->widget2, SIGNAL(xRotationChanged(double)), ui->spinRotX, SLOT(setValue(double)));
    connect(ui->spinRotX, SIGNAL(valueChanged(double)), ui->widget2, SLOT(setXRotation(double)));
    connect(ui->widget2, SIGNAL(yRotationChanged(double)), ui->spinRotY, SLOT(setValue(double)));
    connect(ui->spinRotY, SIGNAL(valueChanged(double)), ui->widget2, SLOT(setYRotation(double)));
    connect(ui->widget2, SIGNAL(zRotationChanged(double)), ui->spinRotZ, SLOT(setValue(double)));
    connect(ui->spinRotZ, SIGNAL(valueChanged(double)), ui->widget2, SLOT(setZRotation(double)));

    connect(ui->cmbBackground, SIGNAL( currentColorChanged(const QColor &)), ui->widget2, SLOT(setBackgroundColor(QColor)));
    connect(ui->cmbAmbient, SIGNAL( currentColorChanged(const QColor &)), ui->widget2, SLOT(setAmbientColor(QColor)));
    connect(ui->cmbBlend, SIGNAL( currentIndexChanged(int)), ui->widget2, SLOT(setBlendMode(int)));
    connect(ui->cmbInterpolation, SIGNAL( currentIndexChanged(int)), ui->widget2, SLOT(setInterpolationMode(int)));


}

void QFVisualize3DGLEditor::on_cmbBlend_currentIndexChanged(int blend)
{
    if (blend==1 && ui->cmbBackground->currentColor()!=QColor("black")) {
        ui->labColHint->setText(tr("a <b>black background</b> should be used for <i>maximum intensity projection</i>"));
        //QTimer::singleShot(10000, ui->labColHint, SLOT(clear()));
    } else {
        ui->labColHint->clear();
    }
}

void QFVisualize3DGLEditor::setCurrent(int item)
{
    disconnect(ui->cmbPalette, SIGNAL(currentPaletteChanged(JKQTPMathImageColorPalette)), ui->lutEditor, SLOT(setColorPalette(JKQTPMathImageColorPalette)));
    disconnect(ui->lutEditor, SIGNAL(lutChanged(QVector<int>)), ui->widget2, SLOT(setLUT(QVector<int>)));
    disconnect(ui->lutAlphaEditor, SIGNAL(lutChanged(QVector<int>)), ui->widget2, SLOT(setAlphaLUT(QVector<int>)));
    disconnect(ui->spinAlphaLower, SIGNAL(valueChanged(double)), ui->widget2, SLOT(setTransparentLowerThreshold(double)));
    disconnect(ui->spinAlphaUpper, SIGNAL(valueChanged(double)), ui->widget2, SLOT(setTransparentUpperThreshold(double)));
    disconnect(ui->cmbPalette, SIGNAL(currentPaletteChanged(JKQTPMathImageColorPalette)), this, SLOT(storeSceneSettingsFromWidgets()));
    disconnect(ui->lutEditor, SIGNAL(lutChanged(QVector<int>)), this, SLOT(storeSceneSettingsFromWidgets()));
    disconnect(ui->lutAlphaEditor, SIGNAL(lutChanged(QVector<int>)), this, SLOT(storeSceneSettingsFromWidgets()));
    disconnect(ui->cmbVizMode, SIGNAL(currentIndexChanged(int)), this, SLOT(storeSceneSettingsFromWidgets()));
    disconnect(ui->chkVisible, SIGNAL(toggled(bool)), ui->widget2, SLOT(setGeoVisible(bool)));

    disconnect(ui->chkSingleColor, SIGNAL(toggled(bool)), this, SLOT(storeSceneSettingsFromWidgets()));
    disconnect(ui->cmbColor, SIGNAL(currentColorChanged(QColor)), this, SLOT(storeSceneSettingsFromWidgets()));
    disconnect(ui->chkSingleColor, SIGNAL(toggled(bool)), ui->widget2, SLOT(setSingleColor(bool)));
    disconnect(ui->cmbColor, SIGNAL(currentColorChanged(QColor)), ui->widget2, SLOT(setColor(QColor)));

    disconnect(ui->cmbCutPlaneType1, SIGNAL( currentIndexChanged(int)), ui->widget2, SLOT(setCutPlane1Type(int)));
    disconnect(ui->chkCutPlane1, SIGNAL( toggled(bool)), ui->widget2, SLOT(setCutPlane1Enabled(bool)));
    disconnect(ui->sliderCutPlane1, SIGNAL( valueChanged(int)), ui->widget2, SLOT(setCutPlane1Position(int)));
    disconnect(ui->cmbCutPlaneType2, SIGNAL( currentIndexChanged(int)), ui->widget2, SLOT(setCutPlane2Type(int)));
    disconnect(ui->chkCutPlane2, SIGNAL( toggled(bool)), ui->widget2, SLOT(setCutPlane2Enabled(bool)));
    disconnect(ui->sliderCutPlane2, SIGNAL( valueChanged(int)), ui->widget2, SLOT(setCutPlane2Position(int)));

    ui->widget2->setCurrentVData(item);

    ui->grpGeometry->setEnabled(item>=0);
    if (item>=0) {
        int vid=getCurrentGeometryID(item);
        ui->cmbVizMode->setCurrentIndex(mapVizMode.value(vid, QFVisualize3DGLWidget::TextureRendering));
        ui->cmbPalette->setColorPalette(mapColorPal.value(vid, JKQTPMathImageGRAY));
        ui->lutEditor->setColorPalette(mapColorPal.value(vid, JKQTPMathImageGRAY));
        ui->lutEditor->setConfig(mapLUT.value(vid, ui->lutEditor->getDefaultConfig()));
        ui->lutAlphaEditor->setConfig(mapLUTAlpha.value(vid, ui->lutAlphaEditor->getDefaultConfig()));
        ui->spinAlphaLower->setValue(ui->widget2->getTransparentLowerThreshold());
        ui->spinAlphaUpper->setValue(ui->widget2->getTransparentUpperThreshold());
        ui->chkVisible->setChecked(ui->widget2->getGeoVisible());

        ui->chkCutPlane1->setChecked(ui->widget2->cutPlane1Enabled());
        ui->cmbCutPlaneType1->setCurrentIndex(ui->widget2->cutPlane1Type());
        ui->sliderCutPlane1->setValue(ui->widget2->cutPlane1Position());
        ui->chkCutPlane2->setChecked(ui->widget2->cutPlane2Enabled());
        ui->cmbCutPlaneType2->setCurrentIndex(ui->widget2->cutPlane2Type());
        ui->sliderCutPlane2->setValue(ui->widget2->cutPlane2Position());

        ui->widCP1->setEnabled(ui->chkCutPlane1->isChecked());
        ui->widCP2->setEnabled(ui->chkCutPlane2->isChecked());
        ui->grpAlpha->setEnabled(ui->chkVisible->isChecked());
        ui->grpBasics->setEnabled(ui->chkVisible->isChecked());
        ui->grpColor->setEnabled(ui->chkVisible->isChecked());
        ui->chkSingleColor->setChecked(ui->widget2->singleColor());
        ui->cmbColor->setCurrentColor(ui->widget2->singleColorColor());

        connect(ui->cmbPalette, SIGNAL(currentPaletteChanged(JKQTPMathImageColorPalette)), ui->lutEditor, SLOT(setColorPalette(JKQTPMathImageColorPalette)));
        connect(ui->lutEditor, SIGNAL(lutChanged(QVector<int>)), ui->widget2, SLOT(setLUT(QVector<int>)));
        connect(ui->lutAlphaEditor, SIGNAL(lutChanged(QVector<int>)), ui->widget2, SLOT(setAlphaLUT(QVector<int>)));
        connect(ui->spinAlphaLower, SIGNAL(valueChanged(double)), ui->widget2, SLOT(setTransparentLowerThreshold(double)));
        connect(ui->spinAlphaUpper, SIGNAL(valueChanged(double)), ui->widget2, SLOT(setTransparentUpperThreshold(double)));
        connect(ui->chkBound, SIGNAL(toggled(bool)), ui->widget2, SLOT(setShowBounds(bool)));
        connect(ui->cmbPalette, SIGNAL(currentPaletteChanged(JKQTPMathImageColorPalette)), this, SLOT(storeSceneSettingsFromWidgets()));
        connect(ui->lutEditor, SIGNAL(lutChanged(QVector<int>)), this, SLOT(storeSceneSettingsFromWidgets()));
        connect(ui->lutAlphaEditor, SIGNAL(lutChanged(QVector<int>)), this, SLOT(storeSceneSettingsFromWidgets()));
        connect(ui->cmbVizMode, SIGNAL(currentIndexChanged(int)), this, SLOT(storeSceneSettingsFromWidgets()));
        connect(ui->chkVisible, SIGNAL(toggled(bool)), ui->widget2, SLOT(setGeoVisible(bool)));
        connect(ui->chkSingleColor, SIGNAL(toggled(bool)), this, SLOT(storeSceneSettingsFromWidgets()));
        connect(ui->cmbColor, SIGNAL(currentColorChanged(QColor)), this, SLOT(storeSceneSettingsFromWidgets()));
        connect(ui->chkSingleColor, SIGNAL(toggled(bool)), ui->widget2, SLOT(setSingleColor(bool)));
        connect(ui->cmbColor, SIGNAL(currentColorChanged(QColor)), ui->widget2, SLOT(setColor(QColor)));

        connect(ui->cmbCutPlaneType1, SIGNAL( currentIndexChanged(int)), ui->widget2, SLOT(setCutPlane1Type(int)));
        connect(ui->chkCutPlane1, SIGNAL( toggled(bool)), ui->widget2, SLOT(setCutPlane1Enabled(bool)));
        connect(ui->sliderCutPlane1, SIGNAL( valueChanged(int)), ui->widget2, SLOT(setCutPlane1Position(int)));
        connect(ui->cmbCutPlaneType2, SIGNAL( currentIndexChanged(int)), ui->widget2, SLOT(setCutPlane2Type(int)));
        connect(ui->chkCutPlane2, SIGNAL( toggled(bool)), ui->widget2, SLOT(setCutPlane2Enabled(bool)));
        connect(ui->sliderCutPlane2, SIGNAL( valueChanged(int)), ui->widget2, SLOT(setCutPlane2Position(int)));
    }

    update3DDataset();
}

void QFVisualize3DGLEditor::storeSceneSettingsFromWidgets()
{
    int item=ui->cmbCurrentGeometry->currentIndex();
    if (item>=0) {
        int vid=getCurrentGeometryID(item);
        mapVizMode[vid]=(QFVisualize3DGLWidget::VisualizationMode)ui->cmbVizMode->currentIndex();
        mapColorPal[vid]=ui->cmbPalette->currentColorPalette();
        mapLUT[vid]=ui->lutEditor->getConfig();
        mapLUTAlpha[vid]=ui->lutAlphaEditor->getConfig();
    }
}



void QFVisualize3DGLEditor::volumeData::free()
{
    if (internal) {
        if (data) ::free(data);
    }
    data=NULL;
    sizeX=0;
    sizeY=0;
    sizeZ=0;
    wx=0;
    wy=0;
    wz=0;
    internal=false;
    name="";
}
