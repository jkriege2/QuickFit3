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
    ui->labCite->setText(tr("JW. Krieger, J. Langowski (2011): <b>QuickFit %1 (compiled: %2, SVN: %3): A data evaluation application for biophysics</b>, <i>[web page] <a href=\"%4\">%4 [Accessed on %5]</a></i>\n"
                            "<br><code><pre>\n"
                            "@misc{quickfit3,\n"
                            "  author={Jan Wolfgang Krieger and Jörg Langowski},\n"
                            "  title={QuickFit %1 (compiled: %2, SVN: %3): A data evaluation application for biophysics},\n"
                            "  howpublished={[web page] \verb+%4+},\n"
                            "  year={%2},\n"
                            "  note={[Accessed on %5]},\n"
                            "}</pre></code>").arg(qfInfoVersion()).arg(qfInfoCompiler()).arg(qfInfoSVNVersion()).arg(qfInfoWeblink()).arg(QDate::currentDate().toString("yyyy-MM-dd")));
    ui->labMailinglist->setText(tr("<a href=\"mailto:%1\">%1</a> (<a href=\"%2\">subscribe</a>)<br><a href=\"%3\">%3</a>").arg(qfInfoMaillist()).arg(qfInfoMaillistRequest()).arg(qfInfoWeblink()));
    ui->labLicense->setText(qfInfoLicense());
    QFile f(":/quickfit3/releasenotes.html");
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString text=f.readAll();
        text=text.replace("$$SVN$$", qfInfoSVNVersion());
        text=text.replace("$$qfInfoCompileDate()$$", qfInfoCompiler());
        ui->edtReleaseNotes->setText(text);
    } else {
        ui->edtReleaseNotes->setPlainText(tr("none available :-((("));
    }
    ui->labVersion->setText(tr("version %1 (SVN: %2 qfInfoCompileDate(): %3)").arg(qfInfoVersion()).arg(qfInfoSVNVersion()).arg(qfInfoCompileDate()));
}

DlgNewVersion::~DlgNewVersion()
{
    delete ui;
}
