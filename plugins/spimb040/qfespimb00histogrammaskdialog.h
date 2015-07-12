/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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

#ifndef QFESPIMB00HISTOGRAMMASKDIALOG_H
#define QFESPIMB00HISTOGRAMMASKDIALOG_H

#include <QDialog>

namespace Ui {
    class QFESPIMB00HistogramMaskDialog; // forward
}

/*! \brief SPIM Control Extension (B040, DKFZ Heidelberg) dialog to set mask creation by histogram in QFESPIMB040CameraView
    \ingroup qf3ext_spimb040


 */
class QFESPIMB00HistogramMaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QFESPIMB00HistogramMaskDialog(QWidget *parent = 0);
    ~QFESPIMB00HistogramMaskDialog();

    int mode() const;
    void setMode(int m);
    int pixels() const;
    void setPixels(int pixels, int maxPixels);

private:
    Ui::QFESPIMB00HistogramMaskDialog *ui;
};

#endif // QFESPIMB00HISTOGRAMMASKDIALOG_H
