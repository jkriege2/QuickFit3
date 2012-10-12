#include "dlgcontactauthors.h"
#include "ui_dlgcontactauthors.h"
#include "qfversion.h"
#include "programoptions.h"
#include "qftools.h"
#include "qfpluginservices.h"

DlgContactAuthors::DlgContactAuthors(QWidget *parent, bool contactMailinglist) :
    QDialog(parent),
    ui(new Ui::DlgContactAuthors)
{
    ui->setupUi(this);
    ui->labWebpage->setText(tr("for further information and new versions, go to: <a href=\"%1\">%1</a>.</small>").arg(QF_WEBLINK));
    ui->edtVersion->setText(QString("QuickFit v%1 (SVN%2 COMPILEDATE: %3), %4 bits").arg(QF_VERSION).arg(SVNVERSION).arg(COMPILEDATE).arg(getApplicationBitDepth()));
    if (contactMailinglist) ui->cmbSendTo->setCurrentIndex(1);
    ui->edtEMail->setText(ProgramOptions::getConfigValue("DlgContactAuthors/email", "").toString());
    ui->edtName->setText(ProgramOptions::getConfigValue("DlgContactAuthors/name", "").toString());
    on_cmbSendTo_currentIndexChanged(ui->cmbSendTo->currentIndex());
}

DlgContactAuthors::~DlgContactAuthors()
{
    delete ui;
}

void DlgContactAuthors::acceptDialog() {
    QString subject=ui->cmbSubject->currentText();
    QByteArray email=QF_EMAIL;
    if (ui->cmbSendTo->currentIndex()==1) email=QF_MAILLIST;
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
        ui->labMailingList->setText(tr("<small><b>Note:</b> You have to first <a href=\"%1\">subscribe to the mailinglist</a> to be allowed to send messages to it!</small>").arg(QF_MAILLIST_REQUEST));
    } else if (index==0) {
        ui->labMailingList->setText(tr("<small>mail goes to: <a href=\"mailto:%1\">%1</a>.</small>").arg(QF_EMAIL));
    } else {
        ui->labMailingList->setText("");
    }
}
