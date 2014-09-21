/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#ifndef PASTEIMAGEDLG_H
#define PASTEIMAGEDLG_H

#include <QDialog>

namespace Ui {
    class PasteImageDlg;
}

class PasteImageDlg : public QDialog
{
        Q_OBJECT
        
    public:
        explicit PasteImageDlg(const QString& directory, QWidget *parent = 0, const QString& inputImageFile=QString(), const QString& filenameTemplate=QString("./pic/pix%1.png"));
        ~PasteImageDlg();

        QString saveImage() const;


    protected slots:
        void clipboardChanged();
        void updateImage();
        void on_chkFine_toggled(bool enabled);

        
    private:
        Ui::PasteImageDlg *ui;
        QString directory;
        QString filenameTemplate;
        QPixmap originalImage;
};

#endif // PASTEIMAGEDLG_H
