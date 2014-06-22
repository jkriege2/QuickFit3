/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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


#include "qfselectionlistdialog.h"
#include "ui_qfselectionlistdialog.h"
#include <QInputDialog>
#include "qfhtmldelegate.h"

QFSelectionListDialog::QFSelectionListDialog(QWidget *parent, bool selection_saveable) :
    QDialog(parent),
    ui(new Ui::QFSelectionListDialog)
{
    this->selection_saveable=selection_saveable;
    ui->setupUi(this);
    ui->btnSave->setVisible(selection_saveable);
    ui->comboBox->setVisible(selection_saveable);
}

QFSelectionListDialog::~QFSelectionListDialog()
{
    delete ui;
}

void QFSelectionListDialog::init(QStringList itemNames, QList<QVariant> itemData)
{
    for (int i=0; i<itemNames.size(); i++) {
        QListWidgetItem* item=new QListWidgetItem(itemNames[i], ui->listWidget);
        item->setCheckState(Qt::Checked);
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
        item->setData(Qt::UserRole, itemData.value(i, itemNames.value(i, "")));
        ui->listWidget->addItem(item);
    }
    ui->listWidget->setItemDelegate(new QFHTMLDelegate(ui->listWidget));
}
void QFSelectionListDialog::init(QStringList itemNames, QList<QVariant> itemData, QList<QColor> colors)
{
    init(itemNames, itemData);
    for (int i=0; i<itemNames.size(); i++) {
        QListWidgetItem* item=ui->listWidget->item(i);
        item->setTextColor(colors.value(i, item->textColor()));
    }
}


void QFSelectionListDialog::init(QStringList itemNames, QList<QVariant> itemData, QList<QColor> colors, QSettings &settings, const QString &prefix)
{
    init(itemNames, itemData, settings, prefix);
    for (int i=0; i<itemNames.size(); i++) {
        QListWidgetItem* item=ui->listWidget->item(i);
        item->setTextColor(colors.value(i, item->textColor()));
    }
}

void QFSelectionListDialog::init(QStringList itemNames, QStringList itemData, QList<QColor> colors)
{
    QList<QVariant> var;
    for (int i=0; i<itemData.size(); i++) var<<itemData[i];
    init(itemNames, var, colors);
}

void QFSelectionListDialog::init(QStringList itemNames, QStringList itemData)
{
    QList<QVariant> var;
    for (int i=0; i<itemData.size(); i++) var<<itemData[i];
    init(itemNames, var);
}

void QFSelectionListDialog::init(QStringList itemNames, QStringList itemData, QSettings &settings, const QString &prefix)
{
    QList<QVariant> var;
    for (int i=0; i<itemData.size(); i++) var<<itemData[i];
    init(itemNames, var, settings, prefix);
}

void QFSelectionListDialog::init(QStringList itemNames, QStringList itemData, QList<QColor> colors, QSettings &settings, const QString &prefix)
{
    QList<QVariant> var;
    for (int i=0; i<itemData.size(); i++) var<<itemData[i];
    init(itemNames, var, colors, settings, prefix);
}

void QFSelectionListDialog::init(QStringList itemNames, QList<QVariant> itemData, QSettings &settings, const QString &prefix)
{
     init(itemNames, itemData);

     settings.beginGroup(prefix+"selections");
     int count=settings.value("count", 0).toInt();
     getDataColumnsByUserSaved.clear();
     ui->comboBox->clear();
     for (int i=0; i<count; i++) {
         settings.beginGroup(QString("item%1").arg(i));
         QString n=settings.value("name", "").toString();
         QStringList item=settings.value("items", QStringList()).toStringList();
         if (!n.isEmpty()) {
             getDataColumnsByUserSaved[n]=item;
         }
         ui->comboBox->addItem(n);
         settings.endGroup();
     }
     ui->comboBox->setCurrentIndex(settings.value("lastActivated", -1).toInt());
     settings.endGroup();


}

void QFSelectionListDialog::setLabel(const QString &text) {
    ui->label->setText(text);
}

void QFSelectionListDialog::writeList(QSettings &settings, const QString &prefix) {
    settings.beginGroup(prefix+"selections");
    QStringList keys=getDataColumnsByUserSaved.keys();
    settings.setValue("count", keys.size());
    for (int i=0; i<keys.size(); i++) {
        settings.beginGroup(QString("item%1").arg(i));
        settings.setValue("name", keys[i]);
        settings.setValue("items", getDataColumnsByUserSaved[keys[i]]);
        settings.endGroup();
    }
    settings.setValue("lastActivated", ui->comboBox->currentIndex());
    settings.endGroup();
}

QStringList QFSelectionListDialog::getSelectedDataStrings() const {
    QStringList data;
    for (int i=0; i<ui->listWidget->count(); i++) {
        if (ui->listWidget->item(i)->checkState()==Qt::Checked) data.append(ui->listWidget->item(i)->data(Qt::UserRole).toString());
    }
    return data;
}

QList<QVariant> QFSelectionListDialog::getSelected() const
{
    QList<QVariant> data;
    for (int i=0; i<ui->listWidget->count(); i++) {
        if (ui->listWidget->item(i)->checkState()==Qt::Checked) data.append(ui->listWidget->item(i)->data(Qt::UserRole));
    }
    return data;
}

QList<bool> QFSelectionListDialog::getSelectedBoolList() const
{
    QList<bool> data;
    for (int i=0; i<ui->listWidget->count(); i++) {
        if (ui->listWidget->item(i)->checkState()==Qt::Checked) data.append(true);
        else data.append(false);
    }
    return data;
}

QList<int> QFSelectionListDialog::getSelectedIndexes() const {
    QList<int> data;
    for (int i=0; i<ui->listWidget->count(); i++) {
        if (ui->listWidget->item(i)->checkState()==Qt::Checked) data.append(i);
    }
    return data;
}

void QFSelectionListDialog::addWidget(const QString& label, QWidget *widget)
{
    ui->formLayout->addRow(label, widget);
}

void QFSelectionListDialog::selectItems(QList<bool> items)
{    
    for (int i=0; i<ui->listWidget->count(); i++) {
        ui->listWidget->item(i)->setCheckState(items.value(i, ui->listWidget->item(i)->checkState()==Qt::Checked)?(Qt::Checked):(Qt::Unchecked));
    }
}

void QFSelectionListDialog::on_btnSave_clicked() {
    if (!selection_saveable) return;
    QString name=tr("my selection name");
    QStringList items=getDataColumnsByUserSaved.keys();
    items<<name;
    bool ok=false;
    name=QInputDialog::getItem(NULL, tr("save selection"), tr("name for new selection:"), items, items.size()-1, true, &ok);
    if (ok) {
        QStringList data;
        for (int i=0; i<ui->listWidget->count(); i++) {
            if (ui->listWidget->item(i)->checkState()==Qt::Checked) data.append(ui->listWidget->item(i)->data(Qt::UserRole).toString());
        }
        data.sort();
        getDataColumnsByUserSaved[name]=data;
        disconnect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(getDataColumnsByUserComboBoxSelected(int)));
        ui->comboBox->clear();
        ui->comboBox->addItems(getDataColumnsByUserSaved.keys());
        ui->comboBox->setCurrentIndex(ui->comboBox->findText(name));
        connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(getDataColumnsByUserComboBoxSelected(int)));
    }
}

void QFSelectionListDialog::on_btnSelectNone_clicked() {
    for (int i=0; i<ui->listWidget->count(); i++) {
        ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
    }
}

void QFSelectionListDialog::on_btnSelectAll_clicked() {
    for (int i=0; i<ui->listWidget->count(); i++) {
        ui->listWidget->item(i)->setCheckState(Qt::Checked);
    }
}

void QFSelectionListDialog::getDataColumnsByUserComboBoxSelected(int index) {
    if (!selection_saveable) return;
    QString name=ui->comboBox->itemText(index);
    QStringList newItems=getDataColumnsByUserSaved.value(name, QStringList());
    if (getDataColumnsByUserSaved.contains(name)) {
        for (int i=0; i<ui->listWidget->count(); i++) {
            if (newItems.contains(ui->listWidget->item(i)->data(Qt::UserRole).toString()) && (!ui->listWidget->item(i)->data(Qt::UserRole).toString().isEmpty())) {
                ui->listWidget->item(i)->setCheckState(Qt::Checked);
            } else {
                ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
            }
        }
    }
}


void QFSelectionListDialog::getDataColumnsByUserItemChanged(QListWidgetItem *widgetitem) {
    if (!selection_saveable) return;
    if (!ui->listWidget) return;
    QStringList data;
    for (int i=0; i<ui->listWidget->count(); i++) {
        if (ui->listWidget->item(i)->checkState()==Qt::Checked) data.append(ui->listWidget->item(i)->data(Qt::UserRole).toString());
    }
    data.sort();

    QMapIterator<QString, QStringList> it(getDataColumnsByUserSaved);
    QString item="";
    while (it.hasNext()) {
        it.next();
        QStringList ld=it.value();
        ld.sort();
        if (data==ld) {
            item=it.key();
        }
    }

    disconnect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(getDataColumnsByUserComboBoxSelected(int)));
    ui->comboBox->setCurrentIndex(ui->comboBox->findText(item));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(getDataColumnsByUserComboBoxSelected(int)));
}
