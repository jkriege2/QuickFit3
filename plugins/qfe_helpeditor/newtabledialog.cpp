/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "newtabledialog.h"
#include "ui_newtabledialog.h"
#include "programoptions.h"
#include "qftools.h"

NewTableDialog::NewTableDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTableDialog)
{
    ui->setupUi(this);

    ui->scroll->setWidget(labPreview=new QLabel("", ui->scroll));

    ui->cmbBack->addColor(Qt::transparent, tr("transparent/none"));
    ui->cmbHCol->addColor(Qt::transparent, tr("transparent/none"));
    ui->cmbBack->setCurrentColor(Qt::transparent);
    ui->cmbHCol->setCurrentColor(Qt::gray);

    loadWidgetGeometry(ProgramOptions::getInstance(), this, "NewTableDialog");

    ProgramOptions::getConfigQCheckBox(ui->chkBack, "NewTableDialog/chkBack");
    ProgramOptions::getConfigQCheckBox(ui->chkCentered, "NewTableDialog/chkCentered");
    ProgramOptions::getConfigQCheckBox(ui->chkH, "NewTableDialog/chkH");
    ProgramOptions::getConfigQCheckBox(ui->chkHBold, "NewTableDialog/chkHBold");
    qfGetConfigColorComboBox(ui->cmbBack, "NewTableDialog/cmbBack");
    qfGetConfigColorComboBox(ui->cmbHCol, "NewTableDialog/cmbHCol");
    ProgramOptions::getConfigQSpinBox(ui->spinC, "NewTableDialog/spinC");
    ProgramOptions::getConfigQSpinBox(ui->spinR, "NewTableDialog/spinR");
    ProgramOptions::getConfigQSpinBox(ui->spinSpacing, "NewTableDialog/spinSpacing");
    ProgramOptions::getConfigQSpinBox(ui->spinPadding, "NewTableDialog/spinPadding");
    ProgramOptions::getConfigQSpinBox(ui->spinBorder, "NewTableDialog/spinBorder");

}

NewTableDialog::~NewTableDialog()
{
    saveWidgetGeometry(ProgramOptions::getInstance(), this, "NewTableDialog");
    ProgramOptions::setConfigQCheckBox(ui->chkBack, "NewTableDialog/chkBack");
    ProgramOptions::setConfigQCheckBox(ui->chkCentered, "NewTableDialog/chkCentered");
    ProgramOptions::setConfigQCheckBox(ui->chkH, "NewTableDialog/chkH");
    ProgramOptions::setConfigQCheckBox(ui->chkHBold, "NewTableDialog/chkHBold");
    qfSetConfigColorComboBox(ui->cmbBack, "NewTableDialog/cmbBack");
    qfSetConfigColorComboBox(ui->cmbHCol, "NewTableDialog/cmbHCol");
    ProgramOptions::setConfigQSpinBox(ui->spinC, "NewTableDialog/spinC");
    ProgramOptions::setConfigQSpinBox(ui->spinR, "NewTableDialog/spinR");
    ProgramOptions::setConfigQSpinBox(ui->spinSpacing, "NewTableDialog/spinSpacing");
    ProgramOptions::setConfigQSpinBox(ui->spinPadding, "NewTableDialog/spinPadding");
    ProgramOptions::setConfigQSpinBox(ui->spinBorder, "NewTableDialog/spinBorder");
    delete ui;
}

QString NewTableDialog::insertText() const
{
    QString b="";
    if (ui->chkBack->isChecked() && ui->cmbBack->currentColor()!=QColor(Qt::transparent)) {
        b=QString(" bgcolor=\"%1\"").arg(ui->cmbBack->currentColor().name());
    }
    QString res=QString("<table cellspacing=\"%1\" cellpadding=\"%2\" border=\"%3\"%4>\n").arg(ui->spinSpacing->value()).arg(ui->spinPadding->value()).arg(ui->spinBorder->value()).arg(b);
    for (int r=0; r<ui->spinR->value(); r++) {
        QString rs=QString("  <tr>\n");
        for (int c=0; c<ui->spinC->value(); c++) {
            if (ui->chkH->isChecked() && r==0) {
                QString t=tr("col%1").arg(c+1);
                if (ui->chkHBold->isChecked()) t=QString("<b>%1</b>").arg(t);
                if (ui->cmbHCol->currentColor()!=QColor(Qt::transparent)) {
                    QString c=ui->cmbHCol->currentColor().name();
                    if (c.size()==6) c="#"+c;
                    rs+=QString("    <th bgcolor=\"%2\">%1</th>\n").arg(t).arg(c);
                } else {
                    rs+=QString("    <th>%1</th>\n").arg(t);
                }
            } else {
                rs+=QString("    <td>(%1,%2)</td>\n").arg(r+1).arg(c+1);
            }
        }
        rs+=QString("  </tr>\n");
        res+=rs;
    }
    res+=QString("</table>");
    if (ui->chkCentered->isChecked()) res=QString("<center>%1</center>").arg(res);
    return res;
}

void NewTableDialog::preview()
{
    labPreview->setText(insertText());
}
