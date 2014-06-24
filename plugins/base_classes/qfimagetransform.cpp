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


QFImageTransformWidget::QFImageTransformWidget(const QString &title, QWidget *parent):
    QWidget(parent)
{
    layForm=new QFormLayout();
    setLayout(layForm);
    labHeader=new QLabel("", this);
    layForm->addRow(labHeader);
    QFont f=labHeader->font();
    f.setPointSizeF(f.pointSizeF()*1.5);
    f.setBold(true);
    labHeader->setFont(f);
    labHeader->setText(title);
}

QFImageTransformWidget::~QFImageTransformWidget()
{

}

bool QFImageTransformWidget::transform(const QVector<double> &input, int width, int height, QVector<double> &output, int &width_out, int &height_out)
{
    output=input;
    width_out=width;
    height_out=height;
    return false;
}


void QFImageTransformWidget::readSettings(QSettings &settings, const QString &prefix)
{

}

void QFImageTransformWidget::writeSettings(QSettings &settings, const QString &prefix) const
{

}


QList<QFImageTransformWidget*> QFImageTransformWidget::loadSettings(const QString &filename, QWidget* parent)
{
    QSettings set(filename, QSettings::IniFormat);
    int count=set.value("count", 0).toInt();
    QList<QFImageTransformWidget*> l;
    for (int i=0; i<count; i++) {
        set.beginGroup(QString("widget%1").arg(i));
        QString type=set.value("type", "").toString();
        QFImageTransformWidget* nw=NULL;
        if (type=="blur") {
            nw=new QFITWBlur(parent);
        }
        if (nw) {
            nw->readSettings(set, "");
            l.append(nw);
        }
        set.endGroup();
    }
    return l;
}

void QFImageTransformWidget::saveSettings(const QString &filename, const QList<QFImageTransformWidget *> &widgets)
{
    QSettings set(filename, QSettings::IniFormat);
    set.setValue("count", widgets.size());
    for (int i=0; i<widgets.size(); i++) {
        set.beginGroup(QString("widget%1").arg(i));
        widgets[i]->writeSettings(set, "");
        set.endGroup();
    }
}


QFITWBlur::QFITWBlur(QWidget *parent):
    QFImageTransformWidget(QObject::tr("Blur"), parent)
{

}

QFITWBlur::~QFITWBlur()
{

}

void QFITWBlur::readSettings(QSettings &settings, const QString &prefix)
{

}

void QFITWBlur::writeSettings(QSettings &settings, const QString &prefix) const
{
    settings.setValue(prefix+"type", "blur");
}

bool QFITWBlur::transform(const QVector<double> &input, int width, int height, QVector<double> &output, int &width_out, int &height_out)
{
    output=input;
    width_out=width;
    height_out=height;
    return true;

}
