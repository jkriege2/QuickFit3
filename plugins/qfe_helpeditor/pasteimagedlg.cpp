/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
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
    QString result;
    if (ui->labImage->pixmap()){
        ui->labImage->pixmap()->save(d.absoluteFilePath(ui->edtFilename->text()));
        result=QString("<img src=\"%1\" border=\"1\">").arg(ui->edtFilename->text());
        if (!ui->edtAnchor->text().isEmpty()) result=QString("<a name=\"%2\">%1").arg(result).arg(ui->edtAnchor->text());
        if (!ui->edtCaption->text().isEmpty()) result=QString("%1<br/>\n<i>%2</i>").arg(result).arg(ui->edtCaption->text());
        if (ui->chkCentered->isChecked()) {
            return QString("\n<center>%1\n</center>\n").arg(result);
        }
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

void PasteImageDlg::on_chkFine_toggled(bool enabled)
{
    ui->spinCutBottom->setSingleStep(enabled?1:10);
    ui->spinCutLeft->setSingleStep(enabled?1:10);
    ui->spinCutTop->setSingleStep(enabled?1:10);
    ui->spinCutRight->setSingleStep(enabled?1:10);
}

