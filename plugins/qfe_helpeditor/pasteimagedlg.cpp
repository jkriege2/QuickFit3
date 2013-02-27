#include "pasteimagedlg.h"
#include "ui_pasteimagedlg.h"
#include "programoptions.h"
#include <QClipboard>

PasteImageDlg::PasteImageDlg(QString directory, QWidget *parent, QString filenameTemplate) :
    QDialog(parent),
    ui(new Ui::PasteImageDlg)
{
    ui->setupUi(this);
    this->directory=directory;
    this->filenameTemplate=filenameTemplate;

    QDir d(directory);
    int i=0;
    while (i<100 && d.exists(filenameTemplate.arg(i,3,10,QLatin1Char('0')))) {
        i++;
    }
    ui->edtFilename->setText(filenameTemplate.arg(i,3,10,QLatin1Char('0')));
    ui->edtAnchor->setText(QFileInfo(ui->edtFilename->text()).baseName());
    ui->edtCaption->setText("");

    ui->chkCentered->setChecked(ProgramOptions::getConfigValue("qfe_helpeditor/PasteImageDlg/centered", false).toBool());
    QClipboard *clipboard = QApplication::clipboard();
    connect(clipboard, SIGNAL(dataChanged()), this, SLOT(clipboardChanged()));
    clipboardChanged();
}

PasteImageDlg::~PasteImageDlg()
{
    ProgramOptions::setConfigValue("qfe_helpeditor/PasteImageDlg/centered", ui->chkCentered->isChecked());
    delete ui;
}

QString PasteImageDlg::saveImage() const
{
    QDir d(directory);
    ui->labImage->pixmap()->save(d.absoluteFilePath(ui->edtFilename->text()));
    QString result;
    result=QString("<img src=\"%1\" border=\"1\">").arg(ui->edtFilename->text());
    if (!ui->edtAnchor->text().isEmpty()) result=QString("<a name=\"%2\">%1").arg(result).arg(ui->edtAnchor->text());
    if (!ui->edtCaption->text().isEmpty()) result=QString("%1<br/>\n<i>%2</i>").arg(result).arg(ui->edtCaption->text());
    if (ui->chkCentered->isChecked()) {
        return QString("<center>\n\t%1\n</center>").arg(result);
    }
    return result;
}

void PasteImageDlg::clipboardChanged()
{
    QClipboard *clipboard = QApplication::clipboard();
    QPixmap pix=clipboard->pixmap();
    if (!pix.isNull()) {
        ui->labImage->setPixmap(pix);
    }
}

