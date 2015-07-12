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

#include "selectresourceimage.h"
#include "ui_selectresourceimage.h"
#include "programoptions.h"

SelectResourceImage::SelectResourceImage(const QString &directory, QWidget *parent, const QString& pathPrefix) :
    QDialog(parent),
    ui(new Ui::SelectResourceImage)
{
    ui->setupUi(this);
    connect(ui->treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(treeClicked(QModelIndex)));
    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(listDoubleClicked(QModelIndex)));


    QStandardItem *parentItem = model.invisibleRootItem();
    listDirectory(QDir(directory), parentItem, pathPrefix);
    ui->treeView->setModel(&model);
    ui->listView->setModel(&model);
    ui->listView->setRootIndex(model.index(0,0));
    ui->spinIconSize->setValue(16);


    isMulti=false;
    ProgramOptions::getConfigWindowGeometry(this, "SelectResourceImage/window");
    ProgramOptions::getConfigQSplitter(ui->splitter, "SelectResourceImage/splitter");
    ProgramOptions::getConfigQSpinBox(ui->spinIconSize, "SelectResourceImage/spinIconSize");
    ProgramOptions::getConfigQToolButton(ui->btnLarge, "SelectResourceImage/btnLarge");
    ProgramOptions::getConfigQToolButton(ui->btnList, "SelectResourceImage/btnList");
    ProgramOptions::getConfigQToolButton(ui->btnTreeOnly, "SelectResourceImage/btnTreeOnly");
    updateView();
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}

SelectResourceImage::SelectResourceImage(const QStringList &directories, QWidget *parent, const QStringList &pathPrefixes, const QStringList &baseNodeNames) :
    QDialog(parent),
    ui(new Ui::SelectResourceImage)
{
    ui->setupUi(this);
    connect(ui->treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(treeClicked(QModelIndex)));
    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(listDoubleClicked(QModelIndex)));


    if (baseNodeNames.isEmpty()) {
        QStandardItem *parentItem = model.invisibleRootItem();
        for (int i=0; i<directories.size(); i++) {
            listDirectory(QDir(directories[i]), parentItem, pathPrefixes.value(i, ""));
        }
    } else {
        QStandardItem *inv = model.invisibleRootItem();
        for (int i=0; i<directories.size(); i++) {
            QStandardItem *parentItem=new QStandardItem(baseNodeNames.value(i, tr("group %1").arg(i+1)));
            parentItem->setData(false, Qt::UserRole+2);
            parentItem->setData(QDir(directories[i]).absolutePath());
            inv->appendRow(parentItem);
            listDirectory(QDir(directories[i]), parentItem, pathPrefixes.value(i, ""));
        }
    }
    ui->treeView->setModel(&model);
    ui->listView->setModel(&model);
    ui->listView->setRootIndex(model.index(0,0));
    ui->btnLarge->setChecked(true);
    ui->spinIconSize->setValue(64);


    isMulti=true;
    ProgramOptions::getConfigWindowGeometry(this, "SelectResourceImageMulti/window");
    ProgramOptions::getConfigQSplitter(ui->splitter, "SelectResourceImageMulti/splitter");
    ProgramOptions::getConfigQSpinBox(ui->spinIconSize, "SelectResourceImageMulti/spinIconSize");
    ProgramOptions::getConfigQToolButton(ui->btnLarge, "SelectResourceImageMulti/btnLarge");
    ProgramOptions::getConfigQToolButton(ui->btnList, "SelectResourceImageMulti/btnList");
    ProgramOptions::getConfigQToolButton(ui->btnTreeOnly, "SelectResourceImageMulti/btnTreeOnly");
    updateView();
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}

SelectResourceImage::~SelectResourceImage()
{
    if (!isMulti) {
        ProgramOptions::setConfigQSpinBox(ui->spinIconSize, "SelectResourceImage/spinIconSize");
        ProgramOptions::setConfigWindowGeometry(this, "SelectResourceImage/window");
        ProgramOptions::setConfigQSplitter(ui->splitter, "SelectResourceImage/splitter");
        ProgramOptions::setConfigQToolButton(ui->btnLarge, "SelectResourceImage/btnLarge");
        ProgramOptions::setConfigQToolButton(ui->btnList, "SelectResourceImage/btnList");
        ProgramOptions::setConfigQToolButton(ui->btnTreeOnly, "SelectResourceImage/btnTreeOnly");
    } else {
        ProgramOptions::setConfigQSpinBox(ui->spinIconSize, "SelectResourceImageMulti/spinIconSize");
        ProgramOptions::setConfigWindowGeometry(this, "SelectResourceImageMulti/window");
        ProgramOptions::setConfigQSplitter(ui->splitter, "SelectResourceImageMulti/splitter");
        ProgramOptions::setConfigQToolButton(ui->btnLarge, "SelectResourceImageMulti/btnLarge");
        ProgramOptions::setConfigQToolButton(ui->btnList, "SelectResourceImageMulti/btnList");
        ProgramOptions::setConfigQToolButton(ui->btnTreeOnly, "SelectResourceImageMulti/btnTreeOnly");
    }
    delete ui;
}

QString SelectResourceImage::getSelectFile() const
{
    return ui->listView->currentIndex().data(Qt::UserRole+1).toString();
}

void SelectResourceImage::listDirectory(QDir dir, QStandardItem *parentItem, const QString& pathPrefix)
{
    QStringList dirs=dir.entryList(QDir::AllDirs|QDir::NoDotAndDotDot,QDir::Name);
    QStringList filters;
    filters<<"*.png"<<"*.jpg"<<"*.bmp"<<"*.svg"<<"*.tif";
    QStringList files=dir.entryList(filters,QDir::Files,QDir::Name);
    for (int i = 0; i < dirs.size(); ++i) {
        QStandardItem *item = new QStandardItem(dirs[i]);
        item->setData(dir.absolutePath()+"/"+dirs[i]+"/");
        item->setData(false, Qt::UserRole+2);
        item->setIcon(QIcon(":/lib/file_opendir.png"));
        parentItem->appendRow(item);
        QDir d=dir;
        d.cd(dirs[i]);
        if (pathPrefix.isEmpty()) {
            listDirectory(d, item, "");
        } else {
            listDirectory(d, item, pathPrefix+dirs[i]+"/");
        }
    }
    for (int i = 0; i < files.size(); ++i) {
        QString abspath=dir.absoluteFilePath(files[i]);
        QStandardItem *item = new QStandardItem(QIcon(abspath), files[i]);
        item->setData(true, Qt::UserRole+2);
        if (pathPrefix.isEmpty()) {
            item->setData(abspath);
        } else {
            item->setData(pathPrefix+files[i]);
        }
        parentItem->appendRow(item);
    }
}

void SelectResourceImage::updateView()
{
    if (ui->btnTreeOnly->isChecked()) {
        ui->listView->setVisible(false);
    } else {
        ui->listView->setVisible(true);        
        if (ui->btnLarge->isChecked()) {
            ui->listView->clearPropertyFlags();
            ui->listView->setViewMode(QListView::IconMode);
            ui->listView->setIconSize(QSize(ui->spinIconSize->value(), ui->spinIconSize->value()));
            ui->listView->setGridSize(QSize(ui->spinIconSize->value(),ui->spinIconSize->value())+QSize(5,21));
        } else if (ui->btnList->isChecked()) {
            ui->listView->clearPropertyFlags();
            ui->listView->setViewMode(QListView::ListMode);
            ui->listView->setIconSize(QSize(16,16));
            ui->listView->setGridSize(QSize(0,0));
        }
        ui->listView->setMovement(QListView::Static);
    }
    ui->spinIconSize->setEnabled(ui->listView->isVisible());
    ui->label->setEnabled(ui->listView->isVisible());
}

void  SelectResourceImage::treeClicked(const QModelIndex& index)
{
    QStandardItem* itm=model.itemFromIndex(index);
    if (!itm) return;
    bool isFile=itm->data(Qt::UserRole+2).toBool();
    if (!isFile) {
        ui->listView->setRootIndex(index);
    } else {
        ui->listView->setRootIndex(index.parent());
        ui->listView->setCurrentIndex(index);
    }

}

void SelectResourceImage::listDoubleClicked(const QModelIndex& index)
{
    QStandardItem* itm=model.itemFromIndex(index);
    if (!itm) return;
    bool isFile=itm->data(Qt::UserRole+2).toBool();
    if (!isFile) {
        ui->listView->setRootIndex(index);
    } else {
        ui->listView->setCurrentIndex(index);
        accept();
    }
    ui->treeView->setCurrentIndex(index);
}
