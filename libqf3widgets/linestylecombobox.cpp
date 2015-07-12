/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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



#include "linestylecombobox.h"

#include <QPixmap>
#include <QPainter>
#include <QRect>

LineStyleComboBox::LineStyleComboBox(QWidget* parent):
    QComboBox(parent)
{

    addLineStyle(Qt::SolidLine, tr("solid"));
    addLineStyle(Qt::DashLine, tr("dashed"));
    addLineStyle(Qt::DotLine, tr("dotted"));
    addLineStyle(Qt::DashDotLine, tr("dash dot"));
    addLineStyle(Qt::DashDotDotLine, tr("dash dot dot"));


    connect(this, SIGNAL(highlighted(int)), this, SLOT(emitHighlighted(int)));
    connect(this, SIGNAL(activated(int)), this, SLOT(emitActivated(int)));
}

LineStyleComboBox::~LineStyleComboBox()
{
    //disconnect(this, SIGNAL(highlighted(int)), this, SLOT(emitHighlighted(int)));
    //disconnect(this, SIGNAL(activated(int)), this, SLOT(emitActivated(int)));
}

void LineStyleComboBox::insertLineStyle(int index, Qt::PenStyle style, const QString &name) {
    QPixmap pix(16,16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    QPen pen(QColor("black"));
    pen.setStyle(style);
    p.setPen(pen);
    p.drawLine(0,8,16,8);
    p.end();

    insertItem(index, QIcon(pix), name, (int)style);
}
