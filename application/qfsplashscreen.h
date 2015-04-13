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

#ifndef QFSPLASHSCREEN_H
#define QFSPLASHSCREEN_H
#include <QColor>
#include <QSplashScreen>

class QFSplashScreen : public QSplashScreen
{
        Q_OBJECT
    public:
        explicit QFSplashScreen(const QPixmap & pixmap = QPixmap(), Qt::WindowFlags f = 0);
        explicit QFSplashScreen(QWidget * parent, const QPixmap & pixmap = QPixmap(), Qt::WindowFlags f = 0);
        ~QFSplashScreen();

    signals:

    public slots:
        void incProgress();
        void setProgressValue(int value);
        void setProgressRange(int min, int max);
        void activateProgress(bool enabled=true);
    protected:
        int m_pmin;
        int m_pmax;
        int m_pvalue;
        bool m_progress;
        QColor m_progressColor;
        QColor m_progressBackground;
        QColor m_progressBorder;
        int m_progressWidth;
        int m_progressHeight;
        int m_progressMargin;


        virtual void drawContents(QPainter *painter);
    private:
        void init();
};

#endif // QFSPLASHSCREEN_H
