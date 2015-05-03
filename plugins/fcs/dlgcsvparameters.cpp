/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#include "dlgcsvparameters.h"
#include<QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui>
#endif


#include <QtDebug>
#include "qfpluginservices.h"
#include "programoptions.h"

dlgCSVParameters::dlgCSVParameters(QWidget* parent, int mode, QString startswith, QString endswith, QString columnSeparator, QString commentStart, double timefactor, int startInLine):
    QDialog(parent)
{
    updating=false;
    setupUi(this);
    edtStartswith->setText(startswith);
    edtEndsWith->setText(endswith);
    QDoubleValidator* validator=new QDoubleValidator(1e-100,1e100,9,edtTimefactor);
    validator->setNotation(QDoubleValidator::ScientificNotation);
    //validator->setDecimals(9);
    edtTimefactor->setValidator(validator);
    edtTimefactor->setText(QLocale::system().toString(timefactor));
    edtColumn->setText(QString(columnSeparator));
    edtComment->setText(QString(commentStart));
    cmbMode->setCurrentIndex(mode);
    spinStartInLine->setValue(startInLine);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(checkValues()));
    loadConfigs();
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}

dlgCSVParameters::~dlgCSVParameters()
{
    //dtor
}


void dlgCSVParameters::setFileContents(const QString& filename) {
    QString preview="";
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly/* | QIODevice::Text*/)) {
        /*QString data1=file.readAll();
        for (int i=0; i<qMin(100, data1.size()); i++) {
            int x=data1[i].toLatin1();
            qDebug()<<QString("%1 ").arg(x, 2, 16);
        }
        qDebug()<<"\n\n";*/

        file.seek(0);
        QString data=file.readAll().replace("\n\r", "\n").replace("\r\n", "\n").replace("\r", "\n").replace("\n\n", "\n").replace(QChar(0x0D), "\n").trimmed();
        /*for (int i=0; i<qMin(100, data.size()); i++) {
            int x=data[i].toLatin1();
            qDebug()<<QString("%1 ").arg(x, 2, 16);
        }
        qDebug()<<"\n\n";*/

        QTextStream in(&data);
        while (!in.atEnd()) {
            preview=preview + in.readLine() +"\n";
        }
        file.close();
    }
    //txtContents->setPlainText(preview);
    txtContents->getEditor()->setPlainText(preview);
}

void dlgCSVParameters::checkValues() {
    bool ok1=(edtColumn->text().size()>0) && (edtColumn->text()!=edtComment->text());
    bool ok3=(edtComment->text().size()>0) && (edtComment->text()!=edtColumn->text());
    if (!(ok1 && ok3)) {
        (void) QMessageBox::critical(this, tr("CSV Import Properties"), tr("The first two properties (column and comment separator) may not be set to the same character! Please correct the indicated fields."), QMessageBox::Ok);
        if (!ok1) {
            QPalette p=edtColumn->palette();
            p.setColor(QPalette::Background, QColor("salmon"));
            edtColumn->setPalette(p);
        } else edtColumn->setPalette(QPalette());
        if (!ok3) {
            QPalette p=edtComment->palette();
            p.setColor(QPalette::Background, QColor("salmon"));
            edtComment->setPalette(p);
        } else edtComment->setPalette(QPalette());
    } else {
        QString s=edtColumn->text();
        column_separator=(s.size()>0)?s[0].toLatin1():',';
        s=edtComment->text();
        comment_start=(s.size()>0)?s[0].toLatin1():'#';
        startswith=edtStartswith->text();
        endswith=edtEndsWith->text();
        timefactor=edtTimefactor->text().toDouble();
        firstline=spinStartInLine->value();
        mode=qMax(0, cmbMode->currentIndex());
        accept();
    }
}

void dlgCSVParameters::showHelp()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelpDirectory("fcs")+"/fcs_fromfiles.html#CSV");
}



void dlgCSVParameters::saveConfig() {
    QString name=tr("my import config name");
    QStringList items=getNames();
    bool ok=false;
    name=QInputDialog::getItem(NULL, tr("save CSV import configuration"), tr("name for new configuration:"), items, items.size()-1, true, &ok);
    if (ok) {

        setProperty(name, "name", name);
        setProperty(name, "csv_separator", edtColumn->text());
        setProperty(name, "csv_comment", edtComment->text());
        setProperty(name, "csv_startswith", edtStartswith->text());
        setProperty(name, "csv_endswith", edtEndsWith->text());
        setProperty(name, "csv_timefactor", edtTimefactor->text().toDouble());
        setProperty(name, "csv_firstline", spinStartInLine->value());
        setProperty(name, "csv_mode", cmbMode->currentIndex());


        loadConfigs();
    }
}

void dlgCSVParameters::deleteConfig() {
    QString index=cmbParamSetName->currentText();
    if (!index.isEmpty()) {
        QDir d;
        d.mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/qfrdrfcs/");
        QSettings set(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/qfrdrfcs/imports.ini", QSettings::IniFormat);
        QStringList groups=set.childGroups();
        for (int i=0; i<groups.size(); i++) {
            set.beginGroup(groups[i]);
            QString n=set.value("name", "").toString();
            if (!n.isEmpty() && (n==index)) set.remove("");
            set.endGroup();
        }
        loadConfigs();
    }
}

void dlgCSVParameters::currentConfigChanged(const QString &name) {
    if (name.isEmpty()) return;
    updating=true;
    edtColumn->setText(getProperty(name, "csv_separator", edtColumn->text()).toString());
    edtComment->setText(getProperty(name, "csv_comment", edtComment->text()).toString());
    edtStartswith->setText(getProperty(name, "csv_startswith", edtStartswith->text()).toString());
    edtEndsWith->setText(getProperty(name, "csv_endswith", edtEndsWith->text()).toString());
    edtTimefactor->setText(getProperty(name, "csv_timefactor", edtTimefactor->text().toDouble()).toString());
    spinStartInLine->setValue(getProperty(name, "csv_firstline", spinStartInLine->value()).toInt());
    cmbMode->setCurrentIndex(getProperty(name, "csv_mode", cmbMode->currentIndex()).toInt());
    updating=false;
}

void dlgCSVParameters::dataChanged()
{
    if (!updating) cmbParamSetName->setCurrentIndex(-1);
}

QStringList dlgCSVParameters::getNames()
{
    QStringList names;
    if (QFile::exists(ProgramOptions::getInstance()->getAssetsDirectory()+"/plugins/qfrdrfcs/imports.ini")) {
        QSettings set(ProgramOptions::getInstance()->getAssetsDirectory()+"/plugins/qfrdrfcs/imports.ini", QSettings::IniFormat);
        QStringList groups=set.childGroups();
        for (int i=0; i<groups.size(); i++) {
            set.beginGroup(groups[i]);
            QString n=set.value("name", "").toString();
            if (!n.isEmpty() && !names.contains(n)) names<<n;
            set.endGroup();
        }

    }
    QDir d;
    d.mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/qfrdrfcs/");
    QSettings set(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/qfrdrfcs/imports.ini", QSettings::IniFormat);
    QStringList groups=set.childGroups();
    for (int i=0; i<groups.size(); i++) {
        set.beginGroup(groups[i]);
        QString n=set.value("name", "").toString();
        if (!n.isEmpty() && !names.contains(n)) names<<n;
        set.endGroup();
    }
    return names;

}

QVariant dlgCSVParameters::getProperty(QString index, QString property, QVariant defaultValue)
{
    QVariant value;
    if (QFile::exists(ProgramOptions::getInstance()->getAssetsDirectory()+"/plugins/qfrdrfcs/imports.ini")) {
        QSettings set(ProgramOptions::getInstance()->getAssetsDirectory()+"/plugins/qfrdrfcs/imports.ini", QSettings::IniFormat);
        QStringList groups=set.childGroups();
        for (int i=0; i<groups.size(); i++) {
            set.beginGroup(groups[i]);
            QString n=set.value("name", "").toString();
            if (n==index) value=set.value(property, defaultValue);
            set.endGroup();
        }

    }
    QDir d;
    d.mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/qfrdrfcs/");
    QSettings set(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/qfrdrfcs/imports.ini", QSettings::IniFormat);
    QStringList groups=set.childGroups();
    for (int i=0; i<groups.size(); i++) {
        set.beginGroup(groups[i]);
        QString n=set.value("name", "").toString();
        if (n==index) value=set.value(property, defaultValue);
        set.endGroup();
    }
    return value;
}

void dlgCSVParameters::setProperty(QString index, QString property, QVariant value) {
    QDir d;
    d.mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/qfrdrfcs/");
    QSettings set(ProgramOptions::getInstance()->getConfigFileDirectory()+"/plugins/qfrdrfcs/imports.ini", QSettings::IniFormat);
    QStringList groups=set.childGroups();
    bool setVal=false;
    int maxGroup=-1;
    for (int i=0; i<groups.size(); i++) {
        set.beginGroup(groups[i]);
        QString n=set.value("name", "").toString();
        if (n==index) {
            set.setValue(property, value);
            setVal=true;
        }
        QRegExp rx("group(\\d+)");
        if (rx.indexIn(n, 0)>=0) {
            maxGroup=qMax(maxGroup, rx.cap(1).toInt());
        }
        set.endGroup();
    }
    if (!setVal) {
        set.setValue(QString("group%1/%2").arg(maxGroup+1).arg(property), value);
    }
}

void dlgCSVParameters::loadConfigs()
{
    QString name=cmbParamSetName->currentText();
    disconnect(cmbParamSetName, SIGNAL(currentIndexChanged(QString)), this, SLOT(currentConfigChanged(QString)));
    cmbParamSetName->clear();
    cmbParamSetName->addItems(getNames());
    cmbParamSetName->setCurrentIndex(cmbParamSetName->findText(name));
    connect(cmbParamSetName, SIGNAL(currentIndexChanged(QString)), this, SLOT(currentConfigChanged(QString)));

}

void dlgCSVParameters::on_btnTab_clicked()
{
    edtColumn->setText("\n");

}
