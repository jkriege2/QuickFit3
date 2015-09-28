/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-26 12:40:44 +0200 (Fr, 26 Sep 2014) $  (revision $Rev: 3508 $)

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



#ifndef SPIMB040OPTIONSWIDGET_H
#define SPIMB040OPTIONSWIDGET_H

#include <QWidget>
#include "qfpluginoptionsdialog.h"

namespace Ui {
    class SPIMB040OptionsWidget;
}

class SPIMB040OptionsWidget : public QFPluginOptionsWidget
{
        Q_OBJECT
        
    public:
        explicit SPIMB040OptionsWidget(QObject* pluginObject, QWidget *parent = 0);
        virtual ~SPIMB040OptionsWidget();
        
        /** \brief read the widget contents from the program options */
        void readSettings(ProgramOptions* options);
        /** \brief write the widget contents to the program options */
        void writeSettings(ProgramOptions* options);
        QString getStylesheet();
        QString getStyle();

    signals:
        void styleChanged(const QString& style, const QString& stylesheet);
    protected slots:
        void on_cmbStylesheet_currentIndexChanged(int index);
        void on_cmbStyle_currentIndexChanged(const QString &text);
        void on_cmbStyle_highlighted(const QString &text);
        void reloadStylesheets(bool forSure=false);
    private:
        Ui::SPIMB040OptionsWidget *ui;
};

#endif // SPIMB040OPTIONSWIDGET_H
