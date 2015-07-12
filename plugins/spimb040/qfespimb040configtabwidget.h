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

#ifndef QFESPIMB040CONFIGTABWIDGET_H
#define QFESPIMB040CONFIGTABWIDGET_H

#include <QWidget>
#include <QSettings>
#include <QPointer>

namespace Ui {
    class QFESPIMB040ConfigTabWidget;
}

class QFESPIMB040ConfigTabWidget : public QWidget{
        Q_OBJECT
    public:
        explicit QFESPIMB040ConfigTabWidget(QWidget *parent = 0);
        ~QFESPIMB040ConfigTabWidget();
        void storeSettings(QSettings &settings, QString prefix) const;
        void loadSettings(QSettings &settings, QString prefix);





        static QFESPIMB040ConfigTabWidget* getInstance() {
            return instance;
        }
        static QString getStylesheet();
        static QString getStyle();

    signals:
        void styleChanged(const QString& style, const QString& stylesheet);
    protected slots:
        void on_cmbStylesheet_currentIndexChanged(int index);
        void on_cmbStyle_currentIndexChanged(const QString &text);
        void on_cmbStyle_highlighted(const QString &text);
        void reloadStylesheets(bool forSure=false);
        void on_btnAutosetConfigs_clicked();
    private:
        Ui::QFESPIMB040ConfigTabWidget *ui;
        static QPointer<QFESPIMB040ConfigTabWidget> instance;
};

#endif // QFESPIMB040CONFIGTABWIDGET_H
