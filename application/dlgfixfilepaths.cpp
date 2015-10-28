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

#include "dlgfixfilepaths.h"
#include "ui_dlgfixfilepaths.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include "qfpluginservices.h"

DlgFixFilepaths::DlgFixFilepaths(const QString& oldName, const QString& lastDir, QWidget *parent) :
    QFDialog(parent),
    ui(new Ui::DlgFixFilepaths)
{
    ui->setupUi(this);
    init(oldName, lastDir);
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}

DlgFixFilepaths::~DlgFixFilepaths()
{
    delete ui;
}

void DlgFixFilepaths::selectFile()
{
    QString fn=QFileInfo(ui->edtOldname->text()).fileName();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select File"), lastDir, tr("File (%1)").arg(fn));
    if (!fileName.isEmpty()) {
        ui->edtNewname->setText(fileName);
    }
}

void DlgFixFilepaths::on_btnApply_clicked() {
    bool ok=true;
    if (!QFile::exists(getNewFilename())) {
        ok=QMessageBox::warning(this, tr("Error"), tr("You selected a non-existent file! Replace directory anyways?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes;
    }
    if (ok) {
        accept();
    }
}

void DlgFixFilepaths::on_btnIgnore_clicked() {
    done(100);
}

void DlgFixFilepaths::on_btnCancel_clicked() {
    reject();
}

QString DlgFixFilepaths::getNewFilename() const
{
    return ui->edtNewname->text();
}

QString DlgFixFilepaths::getLastDir() const
{
    return lastDir;
}

void DlgFixFilepaths::init(const QString &oldName, const QString &lastDir)
{
    ui->edtOldname->setText(oldName);
    ui->labFilename->setText(QFileInfo(oldName).fileName());
    ui->edtNewname->setText("");
    if (lastDir.isEmpty()) {
        this->lastDir=QFileInfo(oldName).absolutePath();
    } else {
        this->lastDir=lastDir;
    }

}

void DlgFixFilepaths::showHelp()
{
    QFPluginServices::getInstance()->displayMainHelpWindow("dlgfixfilepaths.html");

}
