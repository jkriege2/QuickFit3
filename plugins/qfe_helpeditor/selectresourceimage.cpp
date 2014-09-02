/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#include "selectresourceimage.h"
#include "ui_selectresourceimage.h"

SelectResourceImage::SelectResourceImage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectResourceImage)
{
    ui->setupUi(this);


    QStandardItem *parentItem = model.invisibleRootItem();
    listDirectory(QDir(":/"), parentItem);
    ui->treeView->setModel(&model);
}

SelectResourceImage::~SelectResourceImage()
{
    delete ui;
}

QString SelectResourceImage::getSelectFile() const
{
    return ui->treeView->currentIndex().data(Qt::UserRole+1).toString();
}

void SelectResourceImage::listDirectory(QDir dir, QStandardItem *parentItem)
{
    QStringList dirs=dir.entryList(QDir::AllDirs,QDir::Name);
    QStringList filters;
    filters<<"*.png"<<"*.jpg"<<"*.bmp";
    QStringList files=dir.entryList(filters,QDir::Files,QDir::Name);
    for (int i = 0; i < dirs.size(); ++i) {
        QStandardItem *item = new QStandardItem(dirs[i]);
        parentItem->appendRow(item);
        QDir d=dir;
        d.cd(dirs[i]);
        listDirectory(d, item);
    }
    for (int i = 0; i < files.size(); ++i) {
        QString abspath=dir.absoluteFilePath(files[i]);
        QStandardItem *item = new QStandardItem(QIcon(abspath), files[i]);
        item->setData(abspath);
        parentItem->appendRow(item);
    }
}
