#include "qftreeviewcombobox.h"
#include <QHeaderView>
#include <QDebug>

QFTreeViewComboBox::QFTreeViewComboBox(QWidget *parent) :
    QComboBox(parent), _treeView(NULL)
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

void QFTreeViewComboBox::showPopup()
{
    setRootModelIndex(QModelIndex());

    for(int i=1;i<model()->columnCount();++i)
    _treeView->hideColumn(i);

    _treeView->expandAll();
    _treeView->setItemsExpandable(false);
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
