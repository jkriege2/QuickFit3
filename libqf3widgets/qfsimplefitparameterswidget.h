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





#ifndef QFSIMPLEFITPARAMETERSWIDGET_H
#define QFSIMPLEFITPARAMETERSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QList>
#include <QPointer>
#include "qfdoubleedit.h"
#include "libwid_imexport.h"
#include "qfsimplefitparameterinterface.h"
#include <QStringList>
#include <QSpacerItem>

class QFWIDLIB_EXPORT QFSimpleFitParametersWidget : public QWidget {
        Q_OBJECT
    public:
        explicit QFSimpleFitParametersWidget(QFSimpleFitParameterInterface* paraminterface, QWidget *parent = 0);

        void clearWidgets();
        void setParameterInterface(QFSimpleFitParameterInterface* paraminterface);
        void addParameter(const QString& id, const QString& label);
        void setParameters(const QStringList& ids, const QStringList& labels);

    signals:
        void parameterChanged(QString id, double value);
        
    public slots:
        void updateWidgetValues();

    protected:
        bool updating;

        QFSimpleFitParameterInterface* paraminterface;

        struct subWidget {
            QString id;
            QPointer<QLabel> label;
            QPointer<QFDoubleEdit> edit;
        };
        QList<subWidget> widgets;

        QGridLayout* mainLayout;
        QSpacerItem* spacer;

        QString getIDForWidget(QObject *edit);


    protected slots:
        void valueChanged(double value);
        
};

#endif // QFSIMPLEFITPARAMETERSWIDGET_H
