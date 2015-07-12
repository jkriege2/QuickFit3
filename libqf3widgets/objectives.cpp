/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU LesserGeneral Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "objectives.h"
#include "ui_qf3objectiveeditor.h"

ObjectiveDescription::ObjectiveDescription() {
    manufacturer="";
    name="invalid";
    NA=1;
    magnification=1;
}


QF3ObjectiveEditor::QF3ObjectiveEditor(const ObjectiveDescription& objective, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QF3ObjectiveEditor)
{
    ui->setupUi(this);
    init(objective);
}

QF3ObjectiveEditor::~QF3ObjectiveEditor()
{
    delete ui;
}

void QF3ObjectiveEditor::init(const ObjectiveDescription& objective) {
    ui->edtManufacturer->setText(objective.manufacturer);
    ui->edtName->setText(objective.name);
    ui->spinMagnification->setValue(objective.magnification);
    ui->spinNA->setValue(objective.NA);
}

ObjectiveDescription QF3ObjectiveEditor::getData() {
    ObjectiveDescription o;
    o.name=ui->edtName->text();
    o.manufacturer=ui->edtManufacturer->text();
    o.magnification=ui->spinMagnification->value();
    o.NA=ui->spinNA->value();
    return o;
}

















QF3ObjectiveComboboxNotifier* QF3ObjectiveCombobox::m_notifier=NULL;


QF3ObjectiveCombobox::QF3ObjectiveCombobox(QWidget* parent):
    QWidget(parent)
{
    if (m_notifier==NULL) m_notifier=new QF3ObjectiveComboboxNotifier(NULL);
    globalobjectives="";
    localobjectives="";

    hbl=new QHBoxLayout();
    setLayout(hbl);
    hbl->setContentsMargins(0,0,0,0);
    hbl->setSpacing(1);
    cmbObjective=new QEnhancedComboBox(this);
    cmbObjective->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(cmbObjective, SIGNAL(currentIndexChanged(int)), this, SLOT(currentObjectiveChanged(int)));
    hbl->addWidget(cmbObjective);

    btnAddObjective=new QToolButton(this);
    btnAddObjective->setToolTip(tr("add a new objective"));
    btnAddObjective->setIcon(QIcon(":/libqf3widgets/objective_add.png"));
    btnAddObjective->setEnabled(true);
    connect(btnAddObjective, SIGNAL(clicked()), this, SLOT(addObjective()));
    hbl->addWidget(btnAddObjective);
    btnDeleteObjective=new QToolButton(this);
    btnDeleteObjective->setToolTip(tr("delete an objective"));
    btnDeleteObjective->setIcon(QIcon(":/libqf3widgets/objective_delete.png"));
    btnDeleteObjective->setEnabled(true);
    connect(btnDeleteObjective, SIGNAL(clicked()), this, SLOT(deleteObjective()));
    hbl->addWidget(btnDeleteObjective);
    btnEditObjective=new QToolButton(this);
    btnEditObjective->setToolTip(tr("edit an objective"));
    btnEditObjective->setIcon(QIcon(":/libqf3widgets/objective_rename.png"));
    btnEditObjective->setEnabled(true);
    connect(btnEditObjective, SIGNAL(clicked()), this, SLOT(editObjective()));
    hbl->addWidget(btnEditObjective);
    labObjectiveDescription=new QLabel(this);
    hbl->addWidget(labObjectiveDescription, 1);


    connect(m_notifier, SIGNAL(doUpdate()), this, SLOT(loadObjectives()));

}

QF3ObjectiveCombobox::~QF3ObjectiveCombobox() {

}

void QF3ObjectiveCombobox::currentObjectiveChanged(int /*idx*/) {
    int iD=cmbObjective->currentIndex();
    if (iD>=0 && iD<objectives.size()) labObjectiveDescription->setText(tr("<small>magn.: %1x<br>NA: %2</small>").arg(objectives[iD].magnification).arg(objectives[iD].NA));
    else labObjectiveDescription->setText("");

    emit objectiveChanged();
}


void QF3ObjectiveCombobox::loadObjectives() {
    objectives.clear();
    QString currentO=cmbObjective->currentText();
    bool updt=updatesEnabled();
    bool widVisible=isVisible(); if (widVisible) setUpdatesEnabled(false);
    cmbObjective->clear();
    QSettings inifileg(globalobjectives, QSettings::IniFormat);
    QSettings inifile(localobjectives, QSettings::IniFormat);
    // first read global config
    QStringList groups=inifile.childGroups();
    QStringList groupsg=inifile.childGroups();
    for (int i=0; i<groupsg.size(); i++) {
        if (!groups.contains(groupsg[i])) groups.append(groupsg[i]);
    }
    for (int i=0; i<groups.size(); i++) {
        QString g=groups[i];
        ObjectiveDescription o;
        o.name=inifile.value(g+"/name", inifileg.value(g+"/name", "")).toString();
        o.manufacturer=inifile.value(g+"/manufacturer", inifileg.value(g+"/manufacturer", "")).toString();
        o.magnification=inifile.value(g+"/magnification", inifileg.value(g+"/magnification", 1)).toDouble();
        o.NA=inifile.value(g+"/na", 1).toDouble();
        objectives.append(o);

    }

    qSort(objectives);
    for (int i=0; i<objectives.size(); i++) {
        cmbObjective->addItem(QIcon(":/libqf3widgets/objective.png"), objectives[i].name);
    }

    int i=cmbObjective->findText(currentO);
    if (i<0) i=0;
    cmbObjective->setCurrentIndex(i);
    //hbl->update();
    if (widVisible) setUpdatesEnabled(updt);

}

void QF3ObjectiveCombobox::setObjectivesINI(QString globalobjectives, QString localobjectives) {
    this->globalobjectives=globalobjectives;
    this->localobjectives=localobjectives;
    loadObjectives();
}

void QF3ObjectiveCombobox::storeObjectives() {
    QSettings inifileg(globalobjectives, QSettings::IniFormat);
    QSettings inifilel(localobjectives, QSettings::IniFormat);
    QSettings* inifile=&inifileg;
    if (!localobjectives.isEmpty()) {
        inifile=&inifilel;
    }
    if (inifile->isWritable()) {
        inifile->clear();
        for (int i=0; i<objectives.size(); i++) {
            ObjectiveDescription o=objectives[i];
            QString g="objective"+QString::number(i);
            inifile->setValue(g+"/name", o.name);
            inifile->setValue(g+"/manufacturer", o.manufacturer);
            inifile->setValue(g+"/magnification", o.magnification);
            inifile->setValue(g+"/na", o.NA);
        }
        emit m_notifier->emitUpdate();
    }
    loadObjectives();
}

void QF3ObjectiveCombobox::deleteObjective() {
    int i=cmbObjective->currentIndex();
    if (i>=0 && i<objectives.size()) {
        objectives.removeAt(i);
    }
    storeObjectives();
    loadObjectives();
}


void QF3ObjectiveCombobox::editObjective() {
    int i=cmbObjective->currentIndex();
    if (i>=0 && i<objectives.size()) {
        ObjectiveDescription d=objectives[i];

        QF3ObjectiveEditor dlg(d, NULL);
        if (dlg.exec()==QDialog::Accepted) {
            objectives[i]=dlg.getData();
        }

    }
    storeObjectives();
    loadObjectives();
}

void QF3ObjectiveCombobox::addObjective() {
    ObjectiveDescription d;
    d.name=tr("new objective");
    QF3ObjectiveEditor dlg(d, NULL);
    if (dlg.exec()==QDialog::Accepted) {
        d=dlg.getData();
        while (objectiveExists(d.name)) {
            QMessageBox::critical(this, "Objective Editor", "An objective with the same name already exists. Please rename!", QMessageBox::Ok, QMessageBox::Ok);
            if (dlg.exec()!=QDialog::Accepted) return;
            d=dlg.getData();
        }
        //qDebug()<<d.name;
        objectives.append(d);
    }
    storeObjectives();
    loadObjectives();
}

ObjectiveDescription QF3ObjectiveCombobox::getObjectiveDescription(int i) {
    return objectives[i];
}

ObjectiveDescription QF3ObjectiveCombobox::objective() {
    ObjectiveDescription d;
    int i=cmbObjective->currentIndex();
    if (i>=0 && i<objectives.size()) return objectives[i];
    return d;
}

bool QF3ObjectiveCombobox::objectiveExists(QString name) {
    for (int i=0; i<objectives.size(); i++) {
        if (name==objectives[i].name) return true;
    }
    return false;

}


void QF3ObjectiveCombobox::loadSettings(QSettings& settings, QString property) {
    QString obj=settings.value(property+"/id", settings.value(property, "").toString()).toString();

    if (obj.isEmpty()) cmbObjective->setCurrentIndex(0);
    else {
        int idx=cmbObjective->findText(obj);
        if (idx<0) {
            ObjectiveDescription d;
            d.name=settings.value(property+"/name", "").toString();
            d.manufacturer=settings.value(property+"/manufacturer", "").toString();
            d.NA=settings.value(property+"/na", 1).toDouble();
            d.magnification=settings.value(property+"/magnification", 1).toDouble();
            objectives.append(d);
            qSort(objectives);
            storeObjectives();
            loadObjectives();
        }
        idx=cmbObjective->findText(obj);
        cmbObjective->setCurrentIndex(idx);
    }
}

void QF3ObjectiveCombobox::loadSettings(QFManyFilesSettings &settings, QString property)
{
    QString obj=settings.value(property+"/id", settings.value(property, "").toString()).toString();

     if (obj.isEmpty()) cmbObjective->setCurrentIndex(0);
     else {
         int idx=cmbObjective->findText(obj);
         if (idx<0) {
             ObjectiveDescription d;
             d.name=settings.value(property+"/name", "").toString();
             d.manufacturer=settings.value(property+"/manufacturer", "").toString();
             d.NA=settings.value(property+"/na", 1).toDouble();
             d.magnification=settings.value(property+"/magnification", 1).toDouble();
             objectives.append(d);
             qSort(objectives);
             storeObjectives();
             loadObjectives();
         }
         idx=cmbObjective->findText(obj);
         cmbObjective->setCurrentIndex(idx);
     }
}

void QF3ObjectiveCombobox::saveSettings(QSettings& settings, QString property) {
    settings.setValue(property+"/id", cmbObjective->currentText());
    settings.setValue(property+"/manufacturer", objective().manufacturer);
    settings.setValue(property+"/name", objective().name);
    settings.setValue(property+"/magnification", objective().magnification);
    settings.setValue(property+"/na", objective().NA);
}

void QF3ObjectiveCombobox::saveSettings(QFManyFilesSettings &settings, QString property)
{
    settings.setValue(property+"/id", cmbObjective->currentText());
    settings.setValue(property+"/manufacturer", objective().manufacturer);
    settings.setValue(property+"/name", objective().name);
    settings.setValue(property+"/magnification", objective().magnification);
    settings.setValue(property+"/na", objective().NA);
}

void QF3ObjectiveCombobox::setReadOnly(bool readonly) {
    cmbObjective->setReadOnly(readonly);
    btnAddObjective->setEnabled(!readonly);
    btnEditObjective->setEnabled(!readonly);
    btnDeleteObjective->setEnabled(!readonly);
}
