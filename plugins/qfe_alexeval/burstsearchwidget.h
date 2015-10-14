/*
Copyright (c) 2012-2015 by Sebastian Isbaner

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

#ifndef BURSTSEARCHWIDGET_H
#define BURSTSEARCHWIDGET_H

#include <QWidget>
#include<QSettings>
#include "parameters.h"
#include "alexdata.h"
#include "jkstyledbutton.h"
#include"jkqtplotter.h"


namespace Ui {
class BurstSearchWidget;
}

class BurstSearchWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit BurstSearchWidget(QWidget *parent = 0, AlexData *data=NULL);
    ~BurstSearchWidget();
    
    /// store the settings of this widget in \a settings with prefix \a prefix (e.g. group)
    void storeSettings(QSettings& settings, const QString& prefix=QString("")) const;
    /// load the widget's settings from \a settings. the \a prefix must be the same as for storeSettings
    void loadSettings(QSettings& settings, const QString& prefix=QString(""));
    void automatedRun();

private:
    Ui::BurstSearchWidget *ui;
    AlexData* ad;
    JKStyledButton* edt;

    void getParameters();
    void setParameters();
//    JKQtPlotter* plot;

private slots:
    void setMethodSingle(int index);
    void setMethodDual(int /*index*/) {setMethod();}
    void setMethod();
    void setDisabledMethodLowerFrame(bool toggle);

    void on_pushButton_clicked();
    void on_pushButtonSaveIni_clicked();
    void on_pushButtonBurstView_clicked();

    void runBurstSearch();

    void loadIni();



};


#endif // BURSTSEARCHWIDGET_H
