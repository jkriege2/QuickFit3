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

#include "qftreeviewcombobox.h"
#include <QHeaderView>
#include <QDebug>

QFTreeViewComboBox::QFTreeViewComboBox(QWidget *parent) :
    QFEnhancedComboBox(parent), _treeView(NULL)
{
    _treeView = new QTreeView(this);
    _treeView->setFrameShape(QFrame::NoFrame);
    _treeView->setEditTriggers(QTreeView::NoEditTriggers);
    _treeView->setAlternatingRowColors(true);
    _treeView->setSelectionBehavior(QTreeView::SelectRows);
    _treeView->setRootIsDecorated(false);
    _treeView->setWordWrap(true);
    _treeView->setAllColumnsShowFocus(true);
    _treeView->header()->setVisible(false);
    connect(_treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(indexSelected(QModelIndex)));
    connect(_treeView, SIGNAL(activated(QModelIndex)), this, SLOT(indexSelected(QModelIndex)));
    connect(_treeView, SIGNAL(pressed(QModelIndex)), this, SLOT(indexSelected(QModelIndex)));
    setView(_treeView);
    currentItemIndex=QModelIndex();
}

QVariant QFTreeViewComboBox::getCurrentIndexData(int role) const
{
    /*QModelIndex parent=rootModelIndex();
    QModelIndex index=model()->index(currentIndex(), 0, parent);
    return index.data(role);*/
    //qDebug()<<"getCurrentIndexData("<<role<<") curItIdx="<<currentItemIndex;
    return model()->data(currentItemIndex, role);
}

void QFTreeViewComboBox::setCurrentFromModelData(const QVariant &data, int role)
{
    //qDebug()<<"setCurrentFromModelData("<<data<<", "<<role<<")";
    if (!setCurrentFromModelData(data, QModelIndex()))//model()->index(0,0)))
        setCurrentIndex(-1);
}

bool QFTreeViewComboBox::setCurrentFromModelData(const QVariant &data, const QModelIndex &parent, int role)
{
    for (int i=0; i<model()->rowCount(parent); i++) {
        QModelIndex idx=model()->index(i,0,parent);
        if (model()->hasChildren(idx)){
            if (setCurrentFromModelData(data, idx, role)) return true;
        }
        if (model()->data(idx, role)==data) {
            selectIndex(idx);
            return true;
        }
    }
    return false;
}

int getMaxTextWidth(QAbstractItemModel* model, QModelIndex parent, const QFontMetrics& fm) {
    int width=0;
    for (int i=0; i<model->rowCount(parent); i++) {
        QModelIndex idx=model->index(i,0,parent);
        if (idx.isValid()) {
            int w=fm.width(idx.data().toString());
            if (w>width) width=w;
            if (model->hasChildren(idx)) {
                int w=getMaxTextWidth(model, idx, fm)+16;
                if (w>width) width=w;
            }
        }
    }
    return width;
}

void QFTreeViewComboBox::showPopup()
{
    setRootModelIndex(QModelIndex());

    for(int i=1;i<model()->columnCount();++i)
    _treeView->hideColumn(i);

    _treeView->expandAll();
    _treeView->setItemsExpandable(false);

    QFontMetrics fm(view()->font());
    int width=this->width();
    int w=getMaxTextWidth(model(), _treeView->rootIndex(), fm);
    if (w>width) width=w;

    view()->setMinimumWidth(qMax(width, this->width()));
    view()->setGeometry(0,0,width, view()->size().height());


    QComboBox::showPopup();
}

void QFTreeViewComboBox::selectIndex(const QModelIndex & index)
{
    //qDebug()<<"selectIndex("<<index<<")";
    setRootModelIndex(index.parent());
    setCurrentIndex(index.row());
    currentItemIndex=index;
}

void QFTreeViewComboBox::indexSelected(const QModelIndex &idx)
{
    //qDebug()<<"indexSelected("<<idx<<")";
    currentItemIndex=idx;
}


