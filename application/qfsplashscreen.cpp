/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2015-02-13 16:56:45 +0100 (Fr, 13 Feb 2015) $  (revision $Rev: 3827 $)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

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

#include "qfsplashscreen.h"
#include <QPainter>
QFSplashScreen::QFSplashScreen(const QPixmap &pixmap, Qt::WindowFlags f) :
    QSplashScreen(pixmap, f)
{
    init();
}

QFSplashScreen::QFSplashScreen(QWidget *parent, const QPixmap &pixmap, Qt::WindowFlags f) :
    QSplashScreen(parent, pixmap, f)
{
    init();
}

void QFSplashScreen::init()
{
    m_pmax=100;
    m_pmin=0;
    m_progress=false;
    m_pvalue=0;
    m_progressColor=QColor("darkblue");
    m_progressBackground=QColor("white");
    m_progressBorder=QColor("black");
    m_progressWidth=75;
    m_progressHeight=12;
    m_progressMargin=4;
}

QFSplashScreen::~QFSplashScreen()
{

}

void QFSplashScreen::incProgress()
{
    m_pvalue++;
    repaint();
}

void QFSplashScreen::setProgressValue(int value)
{
    m_pvalue=value;
    repaint();
}

void QFSplashScreen::setProgressRange(int min, int max)
{
    m_pmax=max;
    m_pmin=min;
    repaint();
}

void QFSplashScreen::activateProgress(bool enabled)
{
    m_progress=enabled;
    repaint();
}

void QFSplashScreen::drawContents(QPainter *painter)
{
    QSplashScreen::drawContents(painter);
    if (m_progress) {
        painter->save();
        painter->setPen(QColor(Qt::transparent));
        painter->fillRect(width()-m_progressMargin-m_progressWidth, m_progressMargin, m_progressWidth, m_progressHeight, m_progressBackground);
        int w=qMin(m_progressWidth, (m_progressWidth*(m_pvalue-m_pmin))/(m_pmax-m_pmin));
        painter->fillRect(width()-m_progressMargin-m_progressWidth, m_progressMargin, w, m_progressHeight, m_progressColor);
        painter->setPen(m_progressBorder);
        painter->setBrush(QColor(Qt::transparent));
        painter->drawRect(width()-m_progressMargin-m_progressWidth, m_progressMargin, m_progressWidth, m_progressHeight);
        painter->restore();
    }
}


