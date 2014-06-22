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

#include "qfelidedlabel.h"
#include <QPainter>
#include <QTextLayout>
#include <QDebug>

QFElidedLabel::QFElidedLabel(QWidget *parent, Qt::WindowFlags f):
    QLabel(parent, f)
{
    setSizePolicy(QSizePolicy::Ignored, sizePolicy().verticalPolicy());
    elided=true;
    m_elidedMode=Qt::ElideMiddle;
    update();
}

QFElidedLabel::QFElidedLabel(const QString &text, QWidget *parent, Qt::WindowFlags f):
    QLabel(text, parent, f)
{
    setSizePolicy(QSizePolicy::Ignored, sizePolicy().verticalPolicy());
    elided=true;
    m_elidedMode=Qt::ElideMiddle;
    update();
}

Qt::TextElideMode QFElidedLabel::elidedMode() const
{
    return m_elidedMode;
}

void QFElidedLabel::setElided(bool elided)
{
    this->elided=elided;
    update();
}

void QFElidedLabel::setElidedMode(Qt::TextElideMode elidedMode)
{
    m_elidedMode=elidedMode;
    update();
}

void QFElidedLabel::paintEvent(QPaintEvent *event)
{
    if (elided) {
         QFrame::paintEvent(event);

         QPainter painter(this);
         QFontMetrics fontMetrics = painter.fontMetrics();

         bool didElide = false;
         int lineSpacing = fontMetrics.lineSpacing();
         int y = 0;

         QTextLayout textLayout(text(), painter.font());
         QTextOption opt=textLayout.textOption();
         opt.setAlignment(alignment());
         opt.setWrapMode((wordWrap())?QTextOption::WordWrap:QTextOption::NoWrap);

         textLayout.setTextOption(opt);
         textLayout.beginLayout();
         forever {
             QTextLine line = textLayout.createLine();

             if (!line.isValid())
                 break;

             line.setLineWidth(width());
             int nextLineY = y + lineSpacing;

             if (height() >= nextLineY + lineSpacing) {
                 line.draw(&painter, QPoint(0, y));
                 y = nextLineY;
             } else {
                 QString lastLine = text().mid(line.textStart());
                 QString elidedLastLine = fontMetrics.elidedText(lastLine, m_elidedMode, width());
                 painter.drawText(QPoint(0, y + fontMetrics.ascent()), elidedLastLine);
                 line = textLayout.createLine();
                 didElide = line.isValid();
                 break;
             }
         }
         textLayout.endLayout();


    } else {
        QLabel::paintEvent(event);
    }
}

