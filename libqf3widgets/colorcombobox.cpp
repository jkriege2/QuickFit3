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



#include "colorcombobox.h"

#include <QPixmap>
#include <QPainter>
#include <QRect>
#include <QColorDialog>
#include <QSettings>
#include <QStringList>
#include <QString>

ColorComboBoxNotifier* ColorComboBox::m_notifier=NULL;


ColorComboBox::ColorComboBox(QWidget* parent):
    QComboBox(parent)
{
    if (m_notifier==NULL) m_notifier=new ColorComboBoxNotifier(NULL);

    connect(m_notifier, SIGNAL(doUpdate(QColor)), this, SLOT(addNewColor(QColor)));


    loadColors();
    //addColor(QColor(""), tr(""));



    actNewColor=new QAction(tr("add new color"), this);
    connect(actNewColor, SIGNAL(triggered()), this, SLOT(newUserColor()));

    addAction(actNewColor);
    setContextMenuPolicy(Qt::ActionsContextMenu);



    connect(this, SIGNAL(highlighted(int)), this, SLOT(emitHighlighted(int)));
    connect(this, SIGNAL(activated(int)), this, SLOT(emitActivated(int)));
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(emitCurrentColorChanged(int)));
}

ColorComboBox::~ColorComboBox()
{
    //disconnect(this, SIGNAL(highlighted(int)), this, SLOT(emitHighlighted(int)));
    //disconnect(this, SIGNAL(activated(int)), this, SLOT(emitActivated(int)));
    storeColors();
}

void ColorComboBox::insertColor(int index, const QColor &color, const QString &name) {
    for (int i=0; i<colorCount(); i++) {
        if (this->color(i)==color) return;
    }

    QPixmap pix(16,8);
    pix.fill(color);
    QRect r(0,0,15,7);
    QPainter p(&pix);
    p.setPen(QPen(QColor("black")));
    p.drawRect(r);
    p.end();


    if (index<=0) insertItem(0, QIcon(pix), name, color);
    else if (index>=count()-1) insertItem(count()-1, QIcon(pix), name, color);
    else insertItem(index, QIcon(pix), name, color);
}

void ColorComboBox::emitCurrentColorChanged(int i)
{
    emit currentColorChanged(color(i));
}

void ColorComboBox::loadColors()
{
    int idx=currentIndex();

    bool blocked=signalsBlocked();
    blockSignals(true);

    bool upt=updatesEnabled();
    bool widVisible=isVisible(); if (widVisible) setUpdatesEnabled(false);
    clear();
    //addItem(tr("--- new color ---"), QColor());
    //addColor(Qt::transparent, tr("transparent/none"));
    addColor(QColor("black"), tr("black"));
    addColor(QColor("darkgray"), tr("dark gray"));
    addColor(QColor("silver"), tr("silver"));
    addColor(QColor("white"), tr("white"));
    addColor(QColor("navy"), tr("navy"));
    addColor(QColor("darkblue"), tr("dark blue"));
    addColor(QColor("darkslateblue"), tr("dark slate blue"));
    addColor(QColor("blue"), tr("blue"));
    addColor(QColor("cornflowerblue"), tr("cornflower blue"));
    addColor(QColor("lightskyblue"), tr("light sky blue"));
    addColor(QColor("darkcyan"), tr("dark cyan"));
    addColor(QColor("cyan"), tr("cyan"));
    addColor(QColor("lightgreen"), tr("light green"));
    addColor(QColor("darkmagenta"), tr("dark magenta"));
    addColor(QColor("darkviolet"), tr("dark violet"));
    addColor(QColor("magenta"), tr("magenta"));
    addColor(QColor("mediumpurple"), tr("medium purple"));
    addColor(QColor("purple"), tr("purple"));
    addColor(QColor("fuchsia"), tr("fuchsia"));
    addColor(QColor("deeppink"), tr("deep pink"));
    addColor(QColor("maroon"), tr("maroon"));
    addColor(QColor("darkred"), tr("dark red"));
    addColor(QColor("red"), tr("red"));
    addColor(QColor("orangered"), tr("orangered"));
    addColor(QColor("indianred"), tr("indian red"));
    addColor(QColor("salmon"), tr("salmon"));
    addColor(QColor("firebrick"), tr("firebrick"));
    addColor(QColor("darkorange"), tr("dark orange"));
    addColor(QColor("orange"), tr("orange"));
    addColor(QColor("gold"), tr("gold"));
    addColor(QColor("yellow"), tr("yellow"));
    addColor(QColor("lightyellow"), tr("light yellow"));
    addColor(QColor("darkolivegreen"), tr("dark olive green"));
    addColor(QColor("darkgreen"), tr("dark green"));
    addColor(QColor("green"), tr("green"));
    addColor(QColor("forestgreen"), tr("forest green"));
    addColor(QColor("lime"), tr("lime"));
    addColor(QColor("limegreen"), tr("lime green"));
    addColor(QColor("lightgreen"), tr("light green"));
    addColor(QColor(0,0,0.1*255.0), tr("10% blue"));
    addColor(QColor(0,0,0.2*255.0), tr("20% blue"));
    addColor(QColor(0,0,0.3*255.0), tr("30% blue"));
    addColor(QColor(0,0,0.4*255.0), tr("40% blue"));
    addColor(QColor(0,0,0.5*255.0), tr("50% blue"));
    addColor(QColor(0,0,0.6*255.0), tr("60% blue"));
    addColor(QColor(0,0,0.7*255.0), tr("70% blue"));
    addColor(QColor(0,0,0.8*255.0), tr("80% blue"));
    addColor(QColor(0,0,0.9*255.0), tr("90% blue"));
    addColor(QColor(0,0,1.0*255.0), tr("100% blue"));
    addColor(QColor(0,0.1*255.0,0), tr("10% green"));
    addColor(QColor(0,0.2*255.0,0), tr("20% green"));
    addColor(QColor(0,0.3*255.0,0), tr("30% green"));
    addColor(QColor(0,0.4*255.0,0), tr("40% green"));
    addColor(QColor(0,0.5*255.0,0), tr("50% green"));
    addColor(QColor(0,0.6*255.0,0), tr("60% green"));
    addColor(QColor(0,0.7*255.0,0), tr("70% green"));
    addColor(QColor(0,0.8*255.0,0), tr("80% green"));
    addColor(QColor(0,0.9*255.0,0), tr("90% green"));
    addColor(QColor(0,1.0*255.0,0), tr("100% green"));
    addColor(QColor(0.1*255.0,0,0), tr("10% red"));
    addColor(QColor(0.2*255.0,0,0), tr("20% red"));
    addColor(QColor(0.3*255.0,0,0), tr("30% red"));
    addColor(QColor(0.4*255.0,0,0), tr("40% red"));
    addColor(QColor(0.5*255.0,0,0), tr("50% red"));
    addColor(QColor(0.6*255.0,0,0), tr("60% red"));
    addColor(QColor(0.7*255.0,0,0), tr("70% red"));
    addColor(QColor(0.8*255.0,0,0), tr("80% red"));
    addColor(QColor(0.9*255.0,0,0), tr("90% red"));
    addColor(QColor(1.0*255.0,0,0), tr("100% red"));
    addColor(QColor(0.1*255.0,0.1*255.0,0.1*255.0), tr("10% gray"));
    addColor(QColor(0.2*255.0,0.2*255.0,0.2*255.0), tr("20% gray"));
    addColor(QColor(0.3*255.0,0.3*255.0,0.3*255.0), tr("30% gray"));
    addColor(QColor(0.4*255.0,0.4*255.0,0.4*255.0), tr("40% gray"));
    addColor(QColor(0.5*255.0,0.5*255.0,0.5*255.0), tr("50% gray"));
    addColor(QColor(0.6*255.0,0.6*255.0,0.6*255.0), tr("60% gray"));
    addColor(QColor(0.7*255.0,0.7*255.0,0.7*255.0), tr("70% gray"));
    addColor(QColor(0.8*255.0,0.8*255.0,0.8*255.0), tr("80% gray"));
    addColor(QColor(0.9*255.0,0.9*255.0,0.9*255.0), tr("90% gray"));
    addColor(QColor(1.0*255.0,1.0*255.0,1.0*255.0), tr("100% gray"));

    QSettings settings("JKTOOLS", "ColorComboBox");
    settings.beginGroup("usercolors");
    QStringList sl=settings.allKeys();
    for (int i=0; i<sl.size(); i++) {
        if (sl[i].startsWith("user_color") && !(sl[i].endsWith("_name"))) {
            QColor col=QColor(settings.value(sl[i], "black").toString());
            addColor(col, settings.value(sl[i]+"_name", col.name()).toString());
        }
    }
    settings.endGroup();

    if (widVisible) setUpdatesEnabled(upt);
    blockSignals(blocked);

    if (idx>=0 && idx<count()-1) setCurrentIndex(idx);
    else setCurrentIndex(0);
}

void ColorComboBox::storeColors()
{
    QSettings settings("JKTOOLS", "ColorComboBox");
    settings.beginGroup("usercolors");
    for (int i=0; i<colorCount(); i++) {
        settings.setValue(QString("user_color%1").arg(i), color(i).name());
        settings.setValue(QString("user_color%1_name").arg(i), itemText(i));
    }
    settings.endGroup();

}

void ColorComboBox::addNewColor(QColor newColor)
{
    int idx=currentIndex();

    bool blocked=signalsBlocked();
    blockSignals(true);


    for (int i=0; i<colorCount(); i++) {
        if (this->color(i)==newColor) return;
    }
    addColor(newColor, newColor.name());

    blockSignals(blocked);

    if (idx>=0 && idx<count()) {
        if (currentIndex()!=idx) setCurrentIndex(idx);
    } else {
        if (currentIndex()!=0) setCurrentIndex(0);
    }
}

void ColorComboBox::newUserColor()
{
    bool blocked=signalsBlocked();
    blockSignals(true);
    hidePopup();
    QColor newColor=QColorDialog::getColor(color(colorCount()-1), this, tr("select new color"));
    if (newColor.isValid()) {
        addColor(newColor, newColor.name());
        storeColors();
        emit m_notifier->emitNewColor(newColor);
        blockSignals(blocked);
        setCurrentColor(newColor);
    } else {
        blockSignals(blocked);
    }
}

void ColorComboBox::emitActivated(int i)
{
    emit activated(color(i));
}
