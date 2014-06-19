#include "pasteimagedlg.h"
#include "ui_pasteimagedlg.h"
#include "programoptions.h"
#include <QClipboard>

PasteImageDlg::PasteImageDlg(const QString &directory, QWidget *parent, const QString &inputImageFile, const QString &filenameTemplate) :
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

    if (!inputImageFile.isEmpty() && QFile::exists(inputImageFile)) {
        originalImage.load(inputImageFile);
        updateImage();
    } else {
        QClipboard *clipboard = QApplication::clipboard();
        connect(clipboard, SIGNAL(dataChanged()), this, SLOT(clipboardChanged()));
        clipboardChanged();
    }
}

PasteImageDlg::~PasteImageDlg()
{
    ProgramOptions::setConfigValue("qfe_helpeditor/PasteImageDlg/centered", ui->chkCentered->isChecked());
    delete ui;
}

QString PasteImageDlg::saveImage() const
{
    QDir d(directory);
    if (!d.exists()) {
        QDir dd;
        dd.mkpath(d.absolutePath());
    }
    ui->labImage->pixmap()->save(d.absoluteFilePath(ui->edtFilename->text()));
    QString result;
    result=QString("<img src=\"%1\" border=\"1\">").arg(ui->edtFilename->text());
    if (!ui->edtAnchor->text().isEmpty()) result=QString("<a name=\"%2\">%1").arg(result).arg(ui->edtAnchor->text());
    if (!ui->edtCaption->text().isEmpty()) result=QString("%1<br/>\n<i>%2</i>").arg(result).arg(ui->edtCaption->text());
    if (ui->chkCentered->isChecked()) {
        return QString("\n<center>%1\n</center>\n").arg(result);
    }
    return result;
}

void PasteImageDlg::clipboardChanged()
{
    QClipboard *clipboard = QApplication::clipboard();
    originalImage=clipboard->pixmap();
    updateImage();
}

void PasteImageDlg::updateImage()
{
    int x=ui->spinCutLeft->value();
    int y=ui->spinCutTop->value();
    int w=originalImage.width()-x-ui->spinCutRight->value();
    int h=originalImage.height()-y-ui->spinCutBottom->value();
    double s=ui->spinScale->value()/100.0;
    //qDebug()<<x<<y<<w<<h<<s;
    if (w>0 && h>0) {
        QPixmap pix=originalImage.copy(x, y, w, h).scaled(w*s, h*s, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        ui->labImage->setPixmap(pix);
    }
}

