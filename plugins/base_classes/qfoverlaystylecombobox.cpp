/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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



#include "qfoverlaystylecombobox.h"
#include "jkqtpimageelements.h"

QFOverlayStyleCombobox::QFOverlayStyleCombobox(QWidget *parent) :
    QComboBox(parent)
{
    QPixmap selPix(15,15);

    QColor ovlSelCol=QColor("red");
    ovlSelCol.setAlphaF(0.5);
    selColors<<ovlSelCol;
    selPix.fill(ovlSelCol);
    addItem(QIcon(selPix), tr("red overlay image"));

    ovlSelCol=QColor("green");
    ovlSelCol.setAlphaF(0.5);
    selColors<<ovlSelCol;
    selPix.fill(ovlSelCol);
    addItem(QIcon(selPix), tr("green overlay image"));

    ovlSelCol=QColor("blue");
    ovlSelCol.setAlphaF(0.5);
    selColors<<ovlSelCol;
    selPix.fill(ovlSelCol);
    addItem(QIcon(selPix), tr("blue overlay image"));

    ovlSelCol=QColor("yellow");
    ovlSelCol.setAlphaF(0.5);
    selColors<<ovlSelCol;
    selPix.fill(ovlSelCol);
    addItem(QIcon(selPix), tr("yellow overlay image"));


    selPix.fill(QColor(Qt::transparent));
    selColors<<QColor("red");
    plotSymbol(selPix, 7, 7, JKQTPtarget, 15, 1, QColor("red"), QColor("red").lighter());
    addItem(QIcon(selPix), tr("red symbol"));


    selPix.fill(QColor(Qt::transparent));
    selColors<<QColor("green");
    plotSymbol(selPix, 7, 7, JKQTPtarget, 15, 1, QColor("green"), QColor("green").lighter());
    addItem(QIcon(selPix), tr("blue symbol"));


    selPix.fill(QColor(Qt::transparent));
    selColors<<QColor("blue");
    plotSymbol(selPix, 7, 7, JKQTPtarget, 15, 1, QColor("blue"), QColor("blue").lighter());
    addItem(QIcon(selPix), tr("green symbol"));


    selPix.fill(QColor(Qt::transparent));
    selColors<<QColor("yellow");
    plotSymbol(selPix, 7, 7, JKQTPtarget, 15, 1, QColor("yellow"), QColor("yellow").lighter());
    addItem(QIcon(selPix), tr("yellow symbol"));

    selPix.fill(QColor(Qt::transparent));
    selColors<<QColor("black");
    plotSymbol(selPix, 7, 7, JKQTPtarget, 15, 1, QColor("black"), QColor("black").lighter());
    addItem(QIcon(selPix), tr("black symbol"));

    selPix.fill(QColor(Qt::transparent));
    selColors<<QColor("white");
    plotSymbol(selPix, 7, 7, JKQTPtarget, 15, 1, QColor("white"), QColor("white").lighter());
    addItem(QIcon(selPix), tr("white symbol"));

    selPix.fill(QColor(Qt::transparent));
    selColors<<QColor("grey");
    plotSymbol(selPix, 7, 7, JKQTPtarget, 15, 1, QColor("grey"), QColor("grey").lighter());
    addItem(QIcon(selPix), tr("grey symbol"));

}

void QFOverlayStyleCombobox::setSelectedOverlayStyle(JKQTPOverlayImageEnhanced *plot) const
{
    QColor col=selColors.value(currentIndex(), QColor("red"));
    plot->set_trueColor(col);
    plot->set_symbolSizeFactor(0.7);
    plot->set_drawAsRectangles(currentIndex()<=3);
    plot->set_symbol(JKQTPtarget);
    plot->set_falseColor(Qt::transparent);
}
