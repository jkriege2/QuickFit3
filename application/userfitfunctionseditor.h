/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#ifndef USERFITFUNCTIONSEDITOR_H
#define USERFITFUNCTIONSEDITOR_H

#include <QDialog>
#include <QListWidgetItem>
#include "userfitfunctionmodel.h"
#include "qffunctionreferencetool.h"
#include "qfsimpleplotwidget.h"
#include "qftools.h"

namespace Ui {
    class UserFitFunctionsEditor;
}

class UserFitFunctionsEditor : public QDialog
{
        Q_OBJECT
        
    public:
        explicit UserFitFunctionsEditor(QWidget *parent = 0);
        ~UserFitFunctionsEditor();

    protected:
        void clearFF();
        void showFF(const QString& id);
        bool storeCurrentFF(const QString &filename=QString());
        void init();
        QString getFFFilename();
    protected slots:
        void on_btnNewF_clicked();
        void on_btnDeleteF_clicked();
        void on_btnSaveEdit_clicked();
        void on_btnCancelEdit_clicked();
        void on_lstFunctions_itemDoubleClicked(QListWidgetItem* item);
        void on_btnUp_clicked();
        void on_btnDown_clicked();
        void on_btnAdd_clicked();
        void on_btnDelete_clicked();
        void on_edtExpression_textChanged(const QString &text);
        void on_btnFunctionHelp_clicked();
        void on_btnUpdatePreview_clicked();
        void showHelp();
    private:
        Ui::UserFitFunctionsEditor *ui;
        QString currentID;
        UserFitFunctionModel model;
        QFFunctionReferenceTool* functionRef;
        QPointer<QFSimplePlotDialog> simplePlot;

};


#endif // USERFITFUNCTIONSEDITOR_H
