/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfertregexpdialog.h"
#include "ui_qfertregexpdialog.h"
#include <QRegExp>
#include "programoptions.h"
#include "qfpluginservices.h"

QFERTRegExpDialog::QFERTRegExpDialog(QWidget *parent) :
    QFDialog(parent),
    ui(new Ui::QFERTRegExpDialog)
{
    ui->setupUi(this);

    ui->syntaxComboBox->addItem(tr("Regular expression v1"), QRegExp::RegExp);
    ui->syntaxComboBox->addItem(tr("Regular expression v2"), QRegExp::RegExp2);
    ui->syntaxComboBox->addItem(tr("Wildcard"), QRegExp::Wildcard);
    ui->syntaxComboBox->addItem(tr("Fixed string"), QRegExp::FixedString);
    ui->syntaxComboBox->addItem(tr("W3C Xml Schema 1.1"), QRegExp::W3CXmlSchema11);

    QPalette palette = ui->escapedPatternLineEdit->palette();
    palette.setBrush(QPalette::Base,
                     palette.brush(QPalette::Disabled, QPalette::Base));
    ui->escapedPatternLineEdit->setPalette(palette);

    for (int i = 0; i < MaxCaptures; ++i) {
        captureLabels[i] = new QLabel(tr("Capture %1:").arg(i));
        captureEdits[i] = new QLineEdit;
        captureEdits[i]->setReadOnly(true);
    }
    captureLabels[0]->setText(tr("Match:"));

    for (int j = 0; j < MaxCaptures; ++j) {
        ui->mainLayout->addRow(captureLabels[j], captureEdits[j]);
    }

    ProgramOptions::getConfigQCheckBox(ui->caseSensitiveCheckBox, "qfe_resultstools/QFERTRegExpDialog/caseSensitiveCheckBox");
    ProgramOptions::getConfigQCheckBox(ui->minimalCheckBox, "qfe_resultstools/QFERTRegExpDialog/minimalCheckBox");
    ProgramOptions::getConfigQComboBox(ui->syntaxComboBox, "qfe_resultstools/QFERTRegExpDialog/syntaxComboBox");
    ui->patternComboBox->addItems(ProgramOptions::getConfigValue("qfe_resultstools/QFERTRegExpDialog/patternComboBoxItems", QStringList()).toStringList());
    ui->textComboBox->addItems(ProgramOptions::getConfigValue("qfe_resultstools/QFERTRegExpDialog/textComboBoxItems", QStringList()).toStringList());
    ui->patternComboBox->setEditText(ProgramOptions::getConfigValue("qfe_resultstools/QFERTRegExpDialog/patternComboBox", "[A-Za-z_]+([A-Za-z_0-9]*)").toString());
    ui->textComboBox->setEditText(ProgramOptions::getConfigValue("qfe_resultstools/QFERTRegExpDialog/textComboBox", "(10 + delta4) * 32").toString());


    connect(ui->patternComboBox, SIGNAL(editTextChanged(QString)),
            this, SLOT(refresh()));
    connect(ui->textComboBox, SIGNAL(editTextChanged(QString)),
            this, SLOT(refresh()));
    connect(ui->caseSensitiveCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(refresh()));
    connect(ui->minimalCheckBox, SIGNAL(toggled(bool)), this, SLOT(refresh()));
    connect(ui->syntaxComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(refresh()));
    refresh();
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}

QFERTRegExpDialog::~QFERTRegExpDialog()
{
    delete ui;
}

void QFERTRegExpDialog::refresh()
{
    setUpdatesEnabled(false);

    QString pattern = ui->patternComboBox->currentText();
    QString text = ui->textComboBox->currentText();

    QString escaped = pattern;
    escaped.replace("\\", "\\\\");
    escaped.replace("\"", "\\\"");
    escaped.prepend("\"");
    escaped.append("\"");
    ui->escapedPatternLineEdit->setText(escaped);

    QRegExp rx(pattern);
    Qt::CaseSensitivity cs = Qt::CaseInsensitive;
    if (ui->caseSensitiveCheckBox->isChecked())
        cs = Qt::CaseSensitive;
    rx.setCaseSensitivity(cs);
    rx.setMinimal(ui->minimalCheckBox->isChecked());
    QRegExp::PatternSyntax syntax = QRegExp::PatternSyntax(
            ui->syntaxComboBox->itemData(ui->syntaxComboBox->currentIndex()).toInt());
    rx.setPatternSyntax(syntax);

    QPalette palette = ui->patternComboBox->palette();
    if (rx.isValid()) {
        palette.setColor(QPalette::Text,
                         ui->textComboBox->palette().color(QPalette::Text));
    } else {
        palette.setColor(QPalette::Text, Qt::red);
    }
    ui->patternComboBox->setPalette(palette);

    ui->indexEdit->setText(QString::number(rx.indexIn(text)));
    ui->matchedLengthEdit->setText(QString::number(rx.matchedLength()));
    for (int i = 0; i < MaxCaptures; ++i) {
        captureLabels[i]->setEnabled(i <= rx.captureCount());
        captureEdits[i]->setEnabled(i <= rx.captureCount());
        captureEdits[i]->setText(rx.cap(i));
    }

    setUpdatesEnabled(true);


    ProgramOptions::setConfigQCheckBox(ui->caseSensitiveCheckBox, "qfe_resultstools/QFERTRegExpDialog/caseSensitiveCheckBox");
    ProgramOptions::setConfigQCheckBox(ui->minimalCheckBox, "qfe_resultstools/QFERTRegExpDialog/minimalCheckBox");
    ProgramOptions::setConfigQComboBox(ui->syntaxComboBox, "qfe_resultstools/QFERTRegExpDialog/syntaxComboBox");
    QStringList items;
    items.clear();
    for (int i=0; i<ui->patternComboBox->count(); i++) items<<ui->patternComboBox->itemText(i);
    ProgramOptions::setConfigValue("qfe_resultstools/QFERTRegExpDialog/patternComboBoxItems", items);
    items.clear();
    for (int i=0; i<ui->textComboBox->count(); i++) items<<ui->textComboBox->itemText(i);
    ProgramOptions::setConfigValue("qfe_resultstools/QFERTRegExpDialog/textComboBoxItems", items);

    ProgramOptions::setConfigValue("qfe_resultstools/QFERTRegExpDialog/patternComboBox", ui->patternComboBox->currentText());
    ProgramOptions::setConfigValue("qfe_resultstools/QFERTRegExpDialog/textComboBox", ui->textComboBox->currentText());

}

void QFERTRegExpDialog::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayMainHelpWindow("qf3_qtregexp.html");
}
