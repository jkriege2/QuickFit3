/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#ifndef QFRDRIMAGESTACK3DVIEWER_H
#define QFRDRIMAGESTACK3DVIEWER_H

#include <QWidget>
#include <QPointer>

class QFRDRImageStackData; // forward


namespace Ui {
    class QFRDRImageStack3DViewer;
}

class QFRDRImageStack3DViewer : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFRDRImageStack3DViewer(QWidget *parent = 0);
        ~QFRDRImageStack3DViewer();
        void init(QFRDRImageStackData* rdr, int stack);
    public slots:
        void showHelp();
        void copyImage();
        void printImage(QPrinter *p=0);
        void saveImage(const QString &filename=QString());
    protected:
        void saveSettings();
        void closeEvent(QCloseEvent *event);
    private:
        Ui::QFRDRImageStack3DViewer *ui;
        QPointer<QFRDRImageStackData> rdr;
        int stack;
};

#endif // QFRDRIMAGESTACK3DVIEWER_H
