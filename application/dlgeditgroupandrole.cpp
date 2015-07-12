/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2015-02-13 16:56:45 +0100 (Fr, 13 Feb 2015) $  (revision $Rev: 3827 $)

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

#include "dlgeditgroupandrole.h"
#include "ui_dlgeditgroupandrole.h"
#include "qfgroupandrolemodel.h"
#include <QInputDialog>
#include "qfrawdatarecord.h"

DlgEditGroupAndRole::DlgEditGroupAndRole(QFProject *project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgEditGroupAndRole)
{
    this->project=project;
    ui->setupUi(this);
    QFGroupAndRoleModel* m;
    ui->tableView->setModel(m=new QFGroupAndRoleModel(project, this));
    connect(ui->chkAlwaysEditable, SIGNAL(toggled(bool)), m, SLOT(setRoleAlwaysEditable(bool)));
    connect(ui->chkNameEditable, SIGNAL(toggled(bool)), m, SLOT(setNameEditable(bool)));
    ui->tableView->setItemDelegate(new QFGroupAndRoleDelegate(project, this));
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setMinimumSectionSize(120);
    ui->tableView->setTextElideMode(Qt::ElideMiddle);
    for (int i=0; i<ui->tableView->horizontalHeader()->count(); i++) {
        if (ui->tableView->horizontalHeader()->sectionSize(i)>500) {
            ui->tableView->horizontalHeader()->resizeSection(i, 500);
        }

    }
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selChanged()));
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}

DlgEditGroupAndRole::~DlgEditGroupAndRole()
{
    delete ui;
}

void DlgEditGroupAndRole::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayMainHelpWindow("dlgeditgroupandrole.html");
}

void DlgEditGroupAndRole::on_btnSetSelected_clicked()
{
    QModelIndexList l=ui->tableView->selectionModel()->selectedIndexes();
    qDebug()<<l;
    if (ui->chkGroup->isChecked()) {
        if (ui->cmbGroup->count()-1>project->getRDRGroupCount()) {
            for (int i=project->getRDRGroupCount(); i<ui->cmbGroup->count()-1; i++) {
                project->addRDRGroup(ui->cmbGroup->itemText(i));
            }
        }
        for (int i=0; i<l.size(); i++) {
            QFRawDataRecord* rdr=project->getRawDataByNum(l[i].row());
            if (rdr) {
                rdr->setGroup(ui->cmbGroup->currentIndex());
            }
        }
    }
    if (ui->chkRole->isChecked()) {
        for (int i=0; i<l.size(); i++) {
            QFRawDataRecord* rdr=project->getRawDataByNum(l[i].row());
            if (rdr && (rdr->isRoleUserEditable() || ui->chkAlwaysEditable->isChecked())) {
                rdr->setRole(ui->edtRole->text());
            }
        }
    }
    if (ui->chkFolder->isChecked()) {
        for (int i=0; i<l.size(); i++) {
            QFRawDataRecord* rdr=project->getRawDataByNum(l[i].row());
            if (rdr) {
                rdr->setFolder(ui->edtFolder->text());
            }
        }
    }

}

void DlgEditGroupAndRole::selChanged()
{
    bool en=ui->tableView->selectionModel()->selectedIndexes().size()>1;
    bool wasEn=ui->grpSelected->isEnabled();
    if (project && !wasEn && en) {
        ui->cmbGroup->clear();
        ui->cmbGroup->setEditable(false);
        QColor groupBaseColor=QColor("aliceblue");
        QColor gc=groupBaseColor;
        double grpCount=qMax(10,project->getRDRGroupCount());
        for (int i=0; i<project->getRDRGroupCount(); i++) {
            int colFactor=100+i/grpCount*80.0;
            gc=groupBaseColor.darker(colFactor);
            ui->cmbGroup->addItem(project->getRDRGroupName(i));
            ui->cmbGroup->setItemData(i, gc, Qt::BackgroundRole);
        }
        ui->cmbGroup->addItem("--- new group ---");
        ui->cmbGroup->setCurrentIndex(-1);

        ui->chkGroup->setChecked(false);
        ui->chkRole->setChecked(false);
    }
    ui->grpSelected->setEnabled(en);
    if (!en && wasEn) {
        disconnect(ui->cmbGroup, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
    } else if (en && !wasEn) {
        connect(ui->cmbGroup, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
    }

}

void DlgEditGroupAndRole::currentIndexChanged(int index)
{
    QComboBox* cmb=qobject_cast<QComboBox*>(sender());
    if (cmb&&project) {
        if (index>=0 && index==cmb->count()-1) {
            bool ok=false;
            disconnect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
            QString newgroup=QInputDialog::getItem(cmb, tr("new group"), tr("name of new group:"), project->getRDRGroupNames(), -1, true, &ok);
            //qDebug()<<cmb<<newgroup<<ok;
            if (ok) {
                //qDebug()<<cmb->count()<<newgroup;
                cmb->insertItem(cmb->count()-1, newgroup);
                //qDebug()<<cmb->count()<<newgroup;
                cmb->setCurrentIndex(cmb->count()-2);
                //qDebug()<<cmb->count()<<newgroup;
            }
            connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
        }
    }
}
