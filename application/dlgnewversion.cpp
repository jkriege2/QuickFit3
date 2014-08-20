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
        text=text.replace("$$SVN$$", qfInfoSVNVersion());
        text=text.replace("$$COMPILEDATE$$", qfInfoCompileDate());
        ui->edtReleaseNotes->setText(text);
    } else {
        ui->edtReleaseNotes->setPlainText(tr("none available :-((("));
    }
    ui->labVersion->setText(tr("version %1 (SVN: %2 compiledate: %3)").arg(qfInfoVersion()).arg(qfInfoSVNVersion()).arg(qfInfoCompileDate()));
}

DlgNewVersion::~DlgNewVersion()
{
    delete ui;
}
