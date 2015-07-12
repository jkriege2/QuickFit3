/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#ifndef SYSCAMCONFIGDIALOG_H
#define SYSCAMCONFIGDIALOG_H

#include <QDialog>
#include <QSettings>
#include "qfecamsystemcamera.h"
#include <QMap>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>

class VideoCapture; //forward
namespace Ui {
    class SyscamConfigDialog;
}

class SyscamConfigDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit SyscamConfigDialog(VideoCapture* vid, int cam, QWidget *parent = 0);
        ~SyscamConfigDialog();

        void loadFromQSettings(QSettings& data);
        void saveToQSettings(QSettings& data);
        
    private:
        Ui::SyscamConfigDialog *ui;
        VideoCapture* vid;
        int cam;
        QMap<int, QSpinBox*> spins;
        QMap<int, QCheckBox*> checks;
        QMap<int, QComboBox*> combos;
};

#endif // SYSCAMCONFIGDIALOG_H
