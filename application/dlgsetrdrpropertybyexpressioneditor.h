/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#ifndef DLGSETRDRPROPERTYBYEXPRESSIONEDITOR_H
#define DLGSETRDRPROPERTYBYEXPRESSIONEDITOR_H

#include <QWidget>
#include "qconfigcomboboxwidget.h"
#include "qfproject.h"
#include "qfrawdatarecord.h"

namespace Ui {
    class DlgSetRDRPropertyByExpressionEditor;
}

class DlgSetRDRPropertyByExpression; // forward
class DlgSetRDRPropertyByExpressionEditor : public QFrame, public QConfigComboboxWidgetSaveable
{
        Q_OBJECT

    public:
        explicit DlgSetRDRPropertyByExpressionEditor(DlgSetRDRPropertyByExpression* par, QWidget *parent = 0);
        ~DlgSetRDRPropertyByExpressionEditor();

        QString getExpression() const;
        int getType() const;
        QString getPropertyName() const;

        void setExpression(const QString& exp);
        void setType(int t);
        void setPropertyName(const QString& pn);

        void setProject(QFProject *project);

        void setID(const QString &id);

        virtual bool widgetChanged(QSettings* settings, const QString& prefix);
        virtual bool saveWidget(QSettings* settings, const QString& prefix);
        virtual bool loadWidget(QSettings* settings, const QString& prefix);
    public slots:
        void updateTest();
    protected slots:
        void on_btnDelete_clicked();
        void emitWidgetContentsChanged();
    signals:
        void removeMe(DlgSetRDRPropertyByExpressionEditor* editor);
        void widgetContentsChanged();

    private:
        Ui::DlgSetRDRPropertyByExpressionEditor *ui;
        DlgSetRDRPropertyByExpression* par;
        QFProject *project;
};

#endif // DLGSETRDRPROPERTYBYEXPRESSIONEDITOR_H
