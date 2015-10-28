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

#include "dlgcontactauthors.h"
#include "ui_dlgcontactauthors.h"
#include "qfversion.h"
#include "programoptions.h"
#include "qftools.h"
#include "qfpluginservices.h"

DlgContactAuthors::DlgContactAuthors(QWidget *parent, bool contactMailinglist) :
    QFDialog(parent),
    ui(new Ui::DlgContactAuthors)
{
    ui->setupUi(this);
    ui->labWebpage->setText(tr("for further information and new versions, go to: <a href=\"%1\">%1</a> or for the source-code to <a href=\"%2\">%2</a>.</small>").arg(qfInfoWeblink()).arg(qfInfoSourceWeblink()));
    ui->edtVersion->setText(QString("QuickFit v%1 (SVN%2 COMPILEDATE: %3), %4 bits").arg(qfInfoVersion()).arg(qfInfoGITVersion()).arg(qfInfoCompileDate()).arg(getApplicationBitDepth()));
    if (contactMailinglist) ui->cmbSendTo->setCurrentIndex(1);
    ui->edtEMail->setText(ProgramOptions::getConfigValue("DlgContactAuthors/email", "").toString());
    ui->edtName->setText(ProgramOptions::getConfigValue("DlgContactAuthors/name", "").toString());
    on_cmbSendTo_currentIndexChanged(ui->cmbSendTo->currentIndex());
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}

DlgContactAuthors::~DlgContactAuthors()
{
    delete ui;
}

void DlgContactAuthors::acceptDialog() {
    QString subject=ui->cmbSubject->currentText();
    QByteArray email=qfInfoEmail().toLocal8Bit();
    if (ui->cmbSendTo->currentIndex()==1) email=qfInfoMaillist().toLocal8Bit();
    QString mailcontents=tr("%1\n\n\nsender: %2 <%3>\nQuickFit version: %4").arg(ui->edtMessage->toPlainText()).arg(ui->edtName->text()).arg(ui->edtEMail->text()).arg(ui->edtVersion->text());

    QUrl url=QUrl(QByteArray("mailto:")+email+"?subject="+QUrl::toPercentEncoding(subject)+
                  "&body="+QUrl::toPercentEncoding(mailcontents));
    QDesktopServices::openUrl(url);

    ProgramOptions::setConfigValue("DlgContactAuthors/email", ui->edtEMail->text());
    ProgramOptions::setConfigValue("DlgContactAuthors/name", ui->edtName->text());

    QFile f(ProgramOptions::getInstance()->getConfigFileDirectory()+"/lastmail.txt");
    if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
         QTextStream out(&f);
         out<<tr("subject: ")<<subject<<"\nrecipient: %2"<<email<<"\n\ncontents:\n"<<mailcontents;
    }

    accept();
}

void DlgContactAuthors::openHelp()
{
    QFPluginServices::getInstance()->displayMainHelpWindow("qf3_contact.html");
}

void DlgContactAuthors::on_cmbSendTo_currentIndexChanged(int index)
{
    if (index==1) {
        ui->labMailingList->setText(tr("<small><b>Note:</b> You have to first <a href=\"%1\">subscribe to the mailinglist</a> to be allowed to send messages to it!</small>").arg(qfInfoMaillistRequest()));
    } else if (index==0) {
        ui->labMailingList->setText(tr("<small>mail goes to: <a href=\"mailto:%1\">%1</a>.</small>").arg(qfInfoEmail()));
    } else {
        ui->labMailingList->setText("");
    }
}
