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

#ifndef QFESPIMB040ACQUISITIONDESCRIPTION_H
#define QFESPIMB040ACQUISITIONDESCRIPTION_H

#include <QWidget>
#include <QSettings>
#include <QMap>
#include <QVariant>
#include "qfespimb040filenametool.h"

namespace Ui {
    class QFESPIMB040AcquisitionDescription;
};

class QFESPIMB040AcquisitionDescription : public QWidget, public QFESPIMB040FilenameTool {
        Q_OBJECT
        
    public:
        explicit QFESPIMB040AcquisitionDescription(QWidget *parent = 0);
        ~QFESPIMB040AcquisitionDescription();

        void loadSettings(QSettings &settings, QString prefix);
        void storeSettings(QSettings &settings, QString prefix) const;
        QMap<QString, QVariant> getDescription() const;

        int getCell() const;
        int getWell() const;
        int getPlate() const;
        QString getComment() const;
        QString getSample() const;
        QString getPrefix() const;

        void setCurrentSample(const QString& name);
        void setPrefix(const QString& name);
        void setPlate(int p);
        void setWell(int p);
        void setCell(int p);
        void setComment(const QString& name);


        void setOtherSettingWidgets(QFESPIMB040OpticsSetupBase *setup, QFESPIMB040ExperimentDescription *exp);
    public slots:
        void nextCell();
    protected slots:
        void on_btnClearAll_clicked();
        void on_btnNextCell_clicked();
        void updateTime();
        void spinChanged();
        void updateReplaces();
    private:
        Ui::QFESPIMB040AcquisitionDescription *ui;
        QFESPIMB040OpticsSetupBase *opticsSetup;
        QFESPIMB040ExperimentDescription* expDescription;
};

#endif // QFESPIMB040ACQUISITIONDESCRIPTION_H
