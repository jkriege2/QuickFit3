/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#include "dlgnewversion.h"
#include "ui_dlgnewversion.h"
#include "qfversion.h"
#include <QDate>
#include <QFile>

DlgNewVersion::DlgNewVersion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgNewVersion)
{
    ui->setupUi(this);
    ui->labCite->setText(tr("%1<br><pre>%2</pre>").arg(qfInfoCitationHTML()).arg(qfInfoCitationBiBTeX()));
    ui->labMailinglist->setText(tr("<a href=\"mailto:%1\">%1</a> (<a href=\"%2\">subscribe</a>)<br><a href=\"%3\">%3</a>").arg(qfInfoMaillist()).arg(qfInfoMaillistRequest()).arg(qfInfoWeblink()));
    ui->labLicense->setText(qfInfoLicense());
    QFile f(":/quickfit3/releasenotes.html");
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString text=f.readAll();
        text=text.replace("$$SVN$$", qfInfoGITVersion());
        text=text.replace("$$COMPILEDATE$$", qfInfoCompileDate());
        ui->edtReleaseNotes->setText(text);
    } else {
        ui->edtReleaseNotes->setPlainText(tr("none available :-((("));
    }
    ui->labVersion->setText(tr("version %1 (SVN: %2 compiledate: %3)").arg(qfInfoVersion()).arg(qfInfoGITVersion()).arg(qfInfoCompileDate()));
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}

DlgNewVersion::~DlgNewVersion()
{
    delete ui;
}
