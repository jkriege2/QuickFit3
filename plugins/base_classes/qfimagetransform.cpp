/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#include "qfimagetransform.h"
#include "CImg.h"
#include <QHBoxLayout>
#include "qftools.h"
#include <QToolButton>
#include <QInputDialog>
#include "qfpluginservices.h"
#include <QDebug>
#include "qfmathparser.h"

QFImageTransformWidget::QFImageTransformWidget(QWidget *parent)
{
    QGridLayout* lay=new QGridLayout();
    setLayout(lay);

    int cnt=1;
    QToolButton* btn;
    actAdd=createActionAndButton(btn, QIcon(":/lib/file_add.png"), tr("add"), this);
    connect(actAdd, SIGNAL(triggered()), this, SLOT(addWidget()));
    lay->addWidget(btn);
    lay->addWidget(btn, 0, cnt++);
    actClear=createActionAndButton(btn, QIcon(":/lib/exit.png"), tr("clear"), this);
    connect(actClear, SIGNAL(triggered()), this, SLOT(clear()));
    lay->addWidget(btn);
    lay->addWidget(btn, 0, cnt++);
    actLoad=createActionAndButton(btn, QIcon(":/lib/file_open.png"), tr("load"), this);
    connect(actLoad, SIGNAL(triggered()), this, SLOT(load()));
    lay->addWidget(btn);
    lay->addWidget(btn, 0, cnt++);
    actSave=createActionAndButton(btn, QIcon(":/lib/save16.png"), tr("save"), this);
    connect(actSave, SIGNAL(triggered()), this, SLOT(save()));
    lay->addWidget(btn);
    lay->addWidget(btn, 0, cnt++);


    area=new QScrollArea(this);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(area->sizePolicy().hasHeightForWidth());
    area->setSizePolicy(sizePolicy);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    area->setWidgetResizable(true);
    area->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

    lay->addWidget(area, 1,0,1,cnt);
    lay->setColumnStretch(0,1);
    lay->setColumnStretch(1,0);
    lay->setRowStretch(0,0);
    lay->setRowStretch(1,1);
    widMain=new QWidget(area);
    layItems=new QVBoxLayout();
    widMain->setLayout(layItems);
    area->setWidget(widMain);
}

QFImageTransformWidget::~QFImageTransformWidget()
{
    clear();
}

bool QFImageTransformWidget::transform(const QVector<double> &input, int width, int height, QVector<double> &output, int &width_out, int &height_out)
{
    if (items.size()<=0) {
        output=input;
        width_out=width;
        height_out=height;
        while (output.size()<width_out*height_out) output.append(0.0);
        return true;
    } else {
        QVector<double> invec=input;
        int iwidth=width;
        int iheight=height;
        while (invec.size()<iwidth*iheight) invec.append(0.0);
        bool ok=true;
        for (int i=0; i<items.size(); i++) {
            ok=ok&&items[i]->transform(invec, iwidth, iheight, output, width_out, height_out);
            qDebug()<<"  i="<<i+1<<"/"<<items.size()<<":    ("<<iwidth<<iheight<<")  ->  ("<<width_out<<height_out<<")";
            if (i+1<items.size()) {
                invec=output;
                iwidth=width_out;
                iheight=height_out;
            }
        }
        return ok;
    }
}

void QFImageTransformWidget::clear()
{
    for (int i=0; i<items.size(); i++) {
        connectWidget(items[i], false);
        layItems->removeWidget(items[i]);
        items[i]->hide();
        items[i]->deleteLater();
    }
    items.clear();
    emit propertiesChanged();
}

void QFImageTransformWidget::load(const QString &filename)
{
    QString fn=filename;
    if (fn.isEmpty()) {
        QString dir=QFPluginServices::getInstance()->getConfigFileDirectory()+"image_transforms/";
        QString filter=tr("image transform configuration (*.itc)");
        fn=qfGetOpenFileNameSet(QString("QFImageTransformWidget/"), this, tr("select image-transform item configuration ..."), dir, filter);
    }
    if (!fn.isEmpty()) {
        clear();
        items=QFImageTransformItemWidget::loadSettings(fn, widMain);
        for (int i=0; i<items.size(); i++) {
            connectWidget(items[i], true);
            layItems->addWidget(items[i]);
        }
        emit propertiesChanged();
    }
}

void QFImageTransformWidget::save(const QString &filename)
{
    QString fn=filename;
    if (fn.isEmpty()) {
        QString dir=QFPluginServices::getInstance()->getConfigFileDirectory()+"image_transforms/";
        QString filter=tr("image transform configuration (*.itc)");
        fn=qfGetSaveFileNameSet(QString("QFImageTransformWidget/"), this, tr("save image-transform item configuration ..."), dir, filter);
    }
    if (!fn.isEmpty()) {
        QFImageTransformItemWidget::saveSettings(fn, items);
    }

}

void QFImageTransformWidget::emitPropsChanged(QFImageTransformItemWidget *widget)
{
    emit propertiesChanged();
}

void QFImageTransformWidget::deleteWidget(QFImageTransformItemWidget *widget)
{
    int idx=items.indexOf(widget);
    if (idx>=0) {
        items.removeAt(idx);
        connectWidget(widget, false);
        layItems->removeWidget(widget);
        widget->hide();
        widget->deleteLater();
        emit propertiesChanged();
    }
}

void QFImageTransformWidget::moveWidgetsUp(QFImageTransformItemWidget *widget)
{
    int idx=items.indexOf(widget);
    if (idx>=1) {
        for (int i=0; i<items.size(); i++) {
            layItems->removeWidget(items[i]);
        }
        items.swap(idx, idx-1);
        for (int i=0; i<items.size(); i++) {
            layItems->addWidget(items[i]);
        }
        layItems->update();
        emit propertiesChanged();
    }
}

void QFImageTransformWidget::moveWidgetsDown(QFImageTransformItemWidget *widget)
{
    int idx=items.indexOf(widget);
    if (idx>=0 && idx+1<items.size()) {
        for (int i=0; i<items.size(); i++) {
            layItems->removeWidget(items[i]);
        }
        items.swap(idx, idx+1);
        for (int i=0; i<items.size(); i++) {
            layItems->addWidget(items[i]);
        }
        layItems->update();
        emit propertiesChanged();
    }
}

void QFImageTransformWidget::addWidget()
{
    bool ok=false;
    QStringList itemNames=QFImageTransformItemWidget::getItemIDs();
    int item=itemNames.indexOf(QInputDialog::getItem(this, tr("add new image processing item"), tr("item type:"), itemNames, 0, false, &ok));
    if (ok && item>=0 && item<itemNames.size()) {
        QFImageTransformItemWidget* wid=QFImageTransformItemWidget::createItem(itemNames[item], widMain);
        if (wid) {
            items.append(wid);
            layItems->addWidget(wid);
            connectWidget(wid, true);
            emit propertiesChanged();
        }
    }
}

void QFImageTransformWidget::connectWidget(QFImageTransformItemWidget *widget, bool conn)
{
    if (conn) {
        connect(widget, SIGNAL(deleteWidget(QFImageTransformItemWidget*)), this, SLOT(deleteWidget(QFImageTransformItemWidget*)));
        connect(widget, SIGNAL(moveWidgetsUp(QFImageTransformItemWidget*)), this, SLOT(moveWidgetsUp(QFImageTransformItemWidget*)));
        connect(widget, SIGNAL(moveWidgetsDown(QFImageTransformItemWidget*)), this, SLOT(moveWidgetsDown(QFImageTransformItemWidget*)));
        connect(widget, SIGNAL(propertiesChanged(QFImageTransformItemWidget*)), this, SLOT(emitPropsChanged(QFImageTransformItemWidget*)));
    } else {
        disconnect(widget, SIGNAL(deleteWidget(QFImageTransformItemWidget*)), this, SLOT(deleteWidget(QFImageTransformItemWidget*)));
        disconnect(widget, SIGNAL(moveWidgetsUp(QFImageTransformItemWidget*)), this, SLOT(moveWidgetsUp(QFImageTransformItemWidget*)));
        disconnect(widget, SIGNAL(moveWidgetsDown(QFImageTransformItemWidget*)), this, SLOT(moveWidgetsDown(QFImageTransformItemWidget*)));
        disconnect(widget, SIGNAL(propertiesChanged(QFImageTransformItemWidget*)), this, SLOT(emitPropsChanged(QFImageTransformItemWidget*)));
    }
}










QFImageTransformItemWidget::QFImageTransformItemWidget(const QString &title, QWidget *parent):
    QGroupBox(title, parent)
{
    doEmit=true;
    layForm=new QFormLayout();
    QHBoxLayout* lay=new QHBoxLayout();
    setLayout(layForm);
    layForm->addRow(lay);

    lay->addStretch(1);
    QToolButton* btn;
    actDelete=createActionAndButton(btn, QIcon(":/lib/file_delete.png"), tr("delete"), this);
    connect(actDelete, SIGNAL(triggered()), this, SLOT(deleteClicked()));
    lay->addWidget(btn);
    actMoveUp=createActionAndButton(btn, QIcon(":/lib/file_up.png"), tr("move up"), this);
    connect(actMoveUp, SIGNAL(triggered()), this, SLOT(upClicked()));
    lay->addWidget(btn);
    actMoveDown=createActionAndButton(btn, QIcon(":/lib/file_down.png"), tr("move down"), this);
    connect(actMoveDown, SIGNAL(triggered()), this, SLOT(downClicked()));
    lay->addWidget(btn);
}

QFImageTransformItemWidget::~QFImageTransformItemWidget()
{

}

bool QFImageTransformItemWidget::transform(const QVector<double> &input, int width, int height, QVector<double> &output, int &width_out, int &height_out)
{
    output=input;
    width_out=width;
    height_out=height;
    return false;
}


void QFImageTransformItemWidget::readSettings(QSettings &settings, const QString &prefix)
{

}

void QFImageTransformItemWidget::writeSettings(QSettings &settings, const QString &prefix) const
{

}


QList<QFImageTransformItemWidget*> QFImageTransformItemWidget::loadSettings(const QString &filename, QWidget* parent)
{
    QSettings set(filename, QSettings::IniFormat);
    int count=set.value("count", 0).toInt();
    QList<QFImageTransformItemWidget*> l;
    for (int i=0; i<count; i++) {
        set.beginGroup(QString("widget%1").arg(i));
        QString type=set.value("type", "").toString();
        QFImageTransformItemWidget* nw=NULL;
        nw=createItem(type, parent);
        if (nw) {
            nw->readSettings(set, "");
            l.append(nw);
        }
        set.endGroup();
    }
    return l;
}

void QFImageTransformItemWidget::saveSettings(const QString &filename, const QList<QFImageTransformItemWidget *> &widgets)
{
    QSettings set(filename, QSettings::IniFormat);
    set.setValue("count", widgets.size());
    for (int i=0; i<widgets.size(); i++) {
        set.beginGroup(QString("widget%1").arg(i));
        widgets[i]->writeSettings(set, "");
        set.endGroup();
    }
}

QStringList QFImageTransformItemWidget::getItemIDs()
{
    QStringList sl;
    sl<<QString("blur");
    sl<<QString("median_blur");
    sl<<QString("resize");
    sl<<QString("expression");
    sl<<QString("average_x");
    sl<<QString("average_y");
    sl<<QString("homogenize_x");
    sl<<QString("homogenize_y");
    return sl;
}

QFImageTransformItemWidget *QFImageTransformItemWidget::createItem(const QString &id, QWidget* parent)
{
    QFImageTransformItemWidget* it=NULL;
    if (id=="blur") {
        it=new QFITWBlur(parent);
    } else if (id=="median_blur") {
        it=new QFITWMedianFilter(parent);
    } else if (id=="resize") {
        it=new QFITWResize(parent);
    } else if (id=="average_x") {
        it=new QFITWAverageDir(0, false, parent);
    } else if (id=="average_y") {
        it=new QFITWAverageDir(1, false, parent);
    } else if (id=="homogenize_x") {
        it=new QFITWAverageDir(0, true, parent);
    } else if (id=="homogenize_y") {
        it=new QFITWAverageDir(1, true, parent);
    } else if (id=="expression") {
        it=new QFITWExpression(parent);
    }
    return it;
}

void QFImageTransformItemWidget::upClicked()
{
    emit moveWidgetsUp(this);
}

void QFImageTransformItemWidget::downClicked()
{
    emit moveWidgetsDown(this);
}

void QFImageTransformItemWidget::deleteClicked()
{
    emit deleteWidget(this);
}

void QFImageTransformItemWidget::emitPropChange()
{
    if (doEmit) emit propertiesChanged(this);
}


QFITWBlur::QFITWBlur(QWidget *parent):
    QFImageTransformItemWidget(QObject::tr("gaussian blur"), parent)
{
    spinSigma=new QDoubleSpinBox(this);
    spinSigma->setRange(0.00001, 10000000);
    spinSigma->setValue(1);
    spinSigma->setDecimals(3);
    layForm->addRow(tr("sigma = "), spinSigma);
    connect(spinSigma, SIGNAL(editingFinished()), this, SLOT(emitPropChange()));
}

QFITWBlur::~QFITWBlur()
{

}

void QFITWBlur::readSettings(QSettings &settings, const QString &prefix)
{
    doEmit=false;
    spinSigma->setValue(settings.value(prefix+"sigma", 1).toDouble());
    doEmit=true;
    emitPropChange();
}

void QFITWBlur::writeSettings(QSettings &settings, const QString &prefix) const
{
    settings.setValue(prefix+"type", "blur");
    settings.setValue(prefix+"sigma", spinSigma->value());
}

bool QFITWBlur::transform(const QVector<double> &input, int width, int height, QVector<double> &output, int &width_out, int &height_out)
{
    cimg_library::CImg<double> img(input.data(), width, height);
    img.blur(spinSigma->value());
    output=arrayToVector(img.data(), img.width()*img.height());
    width_out=img.width();
    height_out=img.height();
    return true;

}




QFITWMedianFilter::QFITWMedianFilter(QWidget *parent):
    QFImageTransformItemWidget(QObject::tr("median filter"), parent)
{
    spinSigma=new QSpinBox(this);
    spinSigma->setRange(2, 10000000);
    spinSigma->setValue(3);
    layForm->addRow(tr("sigma = "), spinSigma);
    connect(spinSigma, SIGNAL(editingFinished()), this, SLOT(emitPropChange()));
}

void QFITWMedianFilter::readSettings(QSettings &settings, const QString &prefix)
{
    doEmit=false;
    spinSigma->setValue(settings.value(prefix+"sigma", 1).toInt());
    doEmit=true;
    emitPropChange();

}

void QFITWMedianFilter::writeSettings(QSettings &settings, const QString &prefix) const
{
    settings.setValue(prefix+"type", "median_blur");
    settings.setValue(prefix+"sigma", spinSigma->value());

}

bool QFITWMedianFilter::transform(const QVector<double> &input, int width, int height, QVector<double> &output, int &width_out, int &height_out)
{
    cimg_library::CImg<double> img(input.data(), width, height);
    img.blur_median(spinSigma->value());
    output=arrayToVector(img.data(), img.width()*img.height());
    width_out=img.width();
    height_out=img.height();
    return true;
}


QFITWResize::QFITWResize(QWidget *parent):
    QFImageTransformItemWidget(QObject::tr("resize"), parent)
{
    spinWidth=new QSpinBox(this);
    spinWidth->setRange(1, 10000000);
    spinWidth->setValue(100);
    layForm->addRow(tr("width = "), spinWidth);
    connect(spinWidth, SIGNAL(editingFinished()), this, SLOT(emitPropChange()));
    spinHeight=new QSpinBox(this);
    spinHeight->setRange(1, 10000000);
    spinHeight->setValue(100);
    layForm->addRow(tr("height = "), spinHeight);
    connect(spinHeight, SIGNAL(editingFinished()), this, SLOT(emitPropChange()));
    cmbInterpolation=new QComboBox(this);
    cmbInterpolation->addItem(tr("no interpolation"));
    cmbInterpolation->addItem(tr("nearest-neighbor interpolation"));
    cmbInterpolation->addItem(tr("moving average interpolation"));
    cmbInterpolation->addItem(tr("linear interpolation"));
    cmbInterpolation->addItem(tr("grid interpolation"));
    cmbInterpolation->addItem(tr("cubic interpolation"));
    cmbInterpolation->addItem(tr("lanczos interpolation"));
    cmbInterpolation->setCurrentIndex(3);
    layForm->addRow(tr("interpolation = "), cmbInterpolation);
    connect(cmbInterpolation, SIGNAL(currentIndexChanged(int)), this, SLOT(emitPropChange()));
}

void QFITWResize::readSettings(QSettings &settings, const QString &prefix)
{
    doEmit=false;
    spinWidth->setValue(settings.value(prefix+"width", 100).toInt());
    spinHeight->setValue(settings.value(prefix+"height", 100).toInt());
    cmbInterpolation->setCurrentIndex(settings.value(prefix+"interpolation", 3).toInt());
    doEmit=true;
    emitPropChange();
}

void QFITWResize::writeSettings(QSettings &settings, const QString &prefix) const
{
    settings.setValue(prefix+"type", "resize");
    settings.setValue(prefix+"width", spinWidth->value());
    settings.setValue(prefix+"height", spinHeight->value());
    settings.setValue(prefix+"interpolation", cmbInterpolation->currentIndex());

}

bool QFITWResize::transform(const QVector<double> &input, int width, int height, QVector<double> &output, int &width_out, int &height_out)
{
    cimg_library::CImg<double> img(input.data(), width, height);
    img.resize(spinWidth->value(), spinHeight->value(), -100,-100,cmbInterpolation->currentIndex());
    output=arrayToVector(img.data(), img.width()*img.height());
    width_out=img.width();
    height_out=img.height();
    return true;

}


QFITWAverageDir::QFITWAverageDir(int dir, bool samesize, QWidget *parent):
    QFImageTransformItemWidget(QObject::tr("average"), parent)
{
    this->dir=dir;
    this->samesize=samesize;
    if (dir==0 && samesize) setTitle(tr("homogenize X"));
    if (dir==1 && samesize) setTitle(tr("homogenize Y"));
    if (dir==0 && !samesize) setTitle(tr("average X"));
    if (dir==1 && !samesize) setTitle(tr("average Y"));
}

void QFITWAverageDir::readSettings(QSettings &settings, const QString &prefix)
{

}

void QFITWAverageDir::writeSettings(QSettings &settings, const QString &prefix) const
{
    if (dir==0) {
        if (samesize) settings.setValue(prefix+"type", "homogenize_x");
        else settings.setValue(prefix+"type", "average_x");
    } else if (dir==1) {
        if (samesize) settings.setValue(prefix+"type", "homogenize_y");
        else settings.setValue(prefix+"type", "average_y");
    }
}

bool QFITWAverageDir::transform(const QVector<double> &input, int width, int height, QVector<double> &output, int &width_out, int &height_out)
{

    if (dir==0) {
        output=QVector<double>(height, 0.0);
        for (int i=0; i<height; i++) {
            double avg=0;
            double cnt=0;
            for (int j=0; j<width; j++) {
                int idx=i*width+j;
                if (idx<input.size()) {
                    avg+=input[idx];
                    cnt++;
                }
            }
            output[i]=avg/cnt;
        }
        width_out=1;
        height_out=height;
        if (samesize) {
            QVector<double> dat(width*height, 0.0);
            for (int y=0; y<height; y++) {
                for (int x=0; x<width; x++) {
                    dat[y*width+x]=output[y];
                }
            }
            width_out=width;
            height_out=height;
            output=dat;
        }
    } else if (dir==1) {
        output=QVector<double>(width, 0.0);
        for (int i=0; i<width; i++) {
            double avg=0;
            double cnt=0;
            for (int j=0; j<height; j++) {
                int idx=j*width+i;
                if (idx<input.size()) {
                    avg+=input[idx];
                    cnt++;
                }
            }
            output[i]=avg/cnt;
        }
        width_out=width;
        height_out=1;
        if (samesize) {
            width_out=width;
            height_out=height;
            QVector<double> dat=output;
            for (int y=0; y<height; y++) {
                output<<dat;
            }
        }
    } else {
        output=input;
        width_out=width;
        height_out=height;
    }
    return true;

}


QFITWExpression::QFITWExpression(QWidget *parent):
    QFImageTransformItemWidget(QObject::tr("expression"), parent)
{
    edtExpression=new QLineEdit(this);
    edtExpression->setText("Ixy");
    layForm->addRow(tr("expression = "), edtExpression);
    QLabel* lab;
    layForm->addRow("", lab=new QLabel(tr("<i>available variables:</i> <tt>x</tt>, <tt>y</tt>: coordinates, <tt>Ixy</tt>: pixel intensity, <tt>I</tt>: image as vector, <tt>width</tt>, <tt>height</tt>: image width/height"), this));
    lab->setWordWrap(true);
    connect(edtExpression, SIGNAL(editingFinished()), this, SLOT(emitPropChange()));
}


void QFITWExpression::readSettings(QSettings &settings, const QString &prefix)
{
    doEmit=false;
    edtExpression->setText(settings.value(prefix+"expression", "Ixy").toString());
    doEmit=true;
    emitPropChange();

}

void QFITWExpression::writeSettings(QSettings &settings, const QString &prefix) const
{
    settings.setValue(prefix+"type", "expression");
    settings.setValue(prefix+"expression", edtExpression->text());
}

bool QFITWExpression::transform(const QVector<double> &input, int width, int height, QVector<double> &output, int &width_out, int &height_out)
{
    output=input;
    width_out=width;
    height_out=height;

    QFMathParser parser;
    parser.resetErrors();
    parser.addVariableDouble("x",0.0);
    parser.addVariableDouble("y",0.0);
    parser.addVariableDouble("Ixy",0.0);
    parser.addVariableDoubleVector("I",input);
    parser.addVariableDouble("width",(double)width);
    parser.addVariableDouble("height",(double)height);
    QFMathParser::qfmpNode* n=parser.parse(edtExpression->text());
    QFMathParser::ByteCodeProgram bprog;
    QFMathParser::ByteCodeEnvironment bcenv(&parser);
    bcenv.init(&parser);
    bool bc=n->createByteCode(bprog, &bcenv);

    for (int i=0; i<width*height; i++) {
        parser.setVariableDouble("x",double(i%width));
        parser.setVariableDouble("y",double(i/width));
        parser.setVariableDouble("Ixy",input[i]);
        qfmpResult r;
        if (bc) {
            output[i]=parser.evaluateBytecode(bprog);
        } else {
            n->evaluate(r);
            output[i]=r.asNumber();
        }
    }
    return true;
}


