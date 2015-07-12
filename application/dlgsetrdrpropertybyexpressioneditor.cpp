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

#include "dlgsetrdrpropertybyexpressioneditor.h"
#include "ui_dlgsetrdrpropertybyexpressioneditor.h"
#include "dlgsetrdrpropertybyexpression.h"

DlgSetRDRPropertyByExpressionEditor::DlgSetRDRPropertyByExpressionEditor(DlgSetRDRPropertyByExpression *par, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::DlgSetRDRPropertyByExpressionEditor)
{
    ui->setupUi(this);
    project=NULL;
    this->par=par;

    ui->edtExpression->getEdit()->addInsertContextMenuEntry("name", "name");
    ui->edtExpression->getEdit()->addInsertContextMenuEntry("description", "description");
    ui->edtExpression->getEdit()->addInsertContextMenuEntry("group", "group");
    ui->edtExpression->getEdit()->addInsertContextMenuEntry("groupid", "groupid");
    ui->edtExpression->getEdit()->addInsertContextMenuEntry("role", "role");
    ui->edtExpression->getEdit()->addInsertContextMenuEntry("folder", "folder");
    ui->edtExpression->getEdit()->addInsertContextMenuEntry("files", "files");
    ui->edtExpression->getEdit()->addInsertContextMenuEntry("filecount", "filecount");
    ui->edtExpression->getEdit()->addInsertContextMenuEntry("rdrproperty(id)", "rdrproperty(id)");
    ui->edtExpression->getEdit()->addInsertContextMenuEntry("rdrproperty(id, default)", "rdrproperty(id, default)");
    ui->edtExpression->getEdit()->addInsertContextMenuEntry("rdrfile(id)", "rdrfile(id)");
    ui->edtExpression->getEdit()->addInsertContextMenuEntry("rdrfiles(id)", "rdrfiles(id)");
    ui->edtExpression->getEdit()->addInsertContextMenuEntry("rdrfiletype(id)", "rdrfiletype(id)");
}

DlgSetRDRPropertyByExpressionEditor::~DlgSetRDRPropertyByExpressionEditor()
{
    delete ui;
}

QString DlgSetRDRPropertyByExpressionEditor::getExpression() const
{
    return ui->edtExpression->getExpression();
}

int DlgSetRDRPropertyByExpressionEditor::getType() const
{
    return ui->cmbOutput->currentIndex();
}

QString DlgSetRDRPropertyByExpressionEditor::getPropertyName() const
{
    return ui->cmbPropertyName->currentText();
}

void DlgSetRDRPropertyByExpressionEditor::setExpression(const QString &exp)
{
    ui->edtExpression->setExpression(exp);
}

void DlgSetRDRPropertyByExpressionEditor::setType(int t)
{
    ui->cmbOutput->setCurrentIndex(t);
}

void DlgSetRDRPropertyByExpressionEditor::setPropertyName(const QString &pn)
{
    ui->cmbPropertyName->setEditText(pn);
}


void DlgSetRDRPropertyByExpressionEditor::on_btnDelete_clicked()
{
    emit removeMe(this);
}

void DlgSetRDRPropertyByExpressionEditor::emitWidgetContentsChanged()
{
    emit widgetContentsChanged();
}

void DlgSetRDRPropertyByExpressionEditor::setProject(QFProject *project)
{
    this->project=project;

    ui->cmbPropertyName->clear();
    if (project) {
        QString n=ui->cmbPropertyName->currentText();
        ui->cmbPropertyName->addItems(project->getAllPropertyNames());
        ui->cmbPropertyName->setEditText(n);
    }
}

void DlgSetRDRPropertyByExpressionEditor::setID(const QString &/*id*/)
{
    ui->labID->setText(""); //id);
}

void DlgSetRDRPropertyByExpressionEditor::updateTest()
{
    ui->cmbPropertyName->setEnabled(ui->cmbOutput->currentIndex()==4);
    QFRawDataRecord* rdr=par->currentRDR();
    QString error;
    if (rdr) {
        QVariant txt=par->getResult(getExpression(), rdr, NULL, &error);
        ui->labTest->setText(txt.toString());//QString("<tt>%1</tt>").arg(getResult(rdr)));
        if (error.isEmpty()) ui->labError->setText(QString("<font color=\"darkgreen\"><b>OK</b></font>"));
            else ui->labError->setText(QString("<font color=\"red\"><b>ERROR: %1</b></font>").arg(error));

    }
}

bool DlgSetRDRPropertyByExpressionEditor::widgetChanged(QSettings *settings, const QString &prefix)
{
    return (settings->value(prefix+"expression").toString()!=getExpression())||(getType() != settings->value(prefix+"type").toInt()) || (getPropertyName()!= settings->value(prefix+"property").toString());
}

bool DlgSetRDRPropertyByExpressionEditor::saveWidget(QSettings *settings, const QString &prefix)
{
    settings->setValue(prefix+"expression", getExpression());
    settings->setValue(prefix+"type", getType());
    settings->setValue(prefix+"property", getPropertyName());
    return true;
}

bool DlgSetRDRPropertyByExpressionEditor::loadWidget(QSettings *settings, const QString &prefix)
{
    setExpression(settings->value(prefix+"expression").toString());
    setType(settings->value(prefix+"type").toInt());
    setPropertyName(settings->value(prefix+"property").toString());
    return true;
}
