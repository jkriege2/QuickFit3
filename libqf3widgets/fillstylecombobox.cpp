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



#include "fillstylecombobox.h"

#include <QPixmap>
#include <QPainter>
#include <QRect>

FillStyleComboBox::FillStyleComboBox(QWidget* parent):
    QComboBox(parent)
{

    /*

    HorPattern,
    VerPattern,
    CrossPattern,
    BDiagPattern,
    FDiagPattern,
    DiagCrossPattern,
    LinearGradientPattern,
    RadialGradientPattern,
    ConicalGradientPattern,
*/
    addFillStyle(Qt::SolidPattern, tr("solid"));
    addFillStyle(Qt::HorPattern, tr("horizontal"));
    addFillStyle(Qt::VerPattern, tr("vertical"));
    addFillStyle(Qt::CrossPattern, tr("cross"));
    addFillStyle(Qt::BDiagPattern, tr("diagonal B"));
    addFillStyle(Qt::FDiagPattern, tr("diagonal F"));
    addFillStyle(Qt::DiagCrossPattern, tr("diagonal cross"));
    /*addFillStyle(Qt::LinearGradientPattern, tr("lin. gradient"));
    addFillStyle(Qt::RadialGradientPattern, tr("rad. gradient"));
    addFillStyle(Qt::ConicalGradientPattern, tr("con. gradient"));*/
    addFillStyle(Qt::Dense1Pattern, tr("dense 1"));
    addFillStyle(Qt::Dense2Pattern, tr("dense 2"));
    addFillStyle(Qt::Dense3Pattern, tr("dense 3"));
    addFillStyle(Qt::Dense4Pattern, tr("dense 4"));
    addFillStyle(Qt::Dense5Pattern, tr("dense 5"));
    addFillStyle(Qt::Dense6Pattern, tr("dense 6"));
    addFillStyle(Qt::Dense7Pattern, tr("dense 7"));
    addFillStyle(Qt::NoBrush, tr("none"));


    connect(this, SIGNAL(highlighted(int)), this, SLOT(emitHighlighted(int)));
    connect(this, SIGNAL(activated(int)), this, SLOT(emitActivated(int)));
}

FillStyleComboBox::~FillStyleComboBox()
{
    //disconnect(this, SIGNAL(highlighted(int)), this, SLOT(emitHighlighted(int)));
    //disconnect(this, SIGNAL(activated(int)), this, SLOT(emitActivated(int)));
}

void FillStyleComboBox::insertFillStyle(int index, Qt::BrushStyle style, const QString &name) {
    QPixmap pix(32,16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    QBrush pen(QColor("black"));
    pen.setStyle(style);
    p.setBrush(pen);
    p.fillRect(QRect(0,0,32,16), pen);
    p.end();

    insertItem(index, QIcon(pix), name, (int)style);
}
