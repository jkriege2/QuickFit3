#include "qfsimpletreemodel.h"
#include <QStringList>

QFSimpleTreeModel::QFSimpleTreeModel(const QString &data, QObject *parent) :
    QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "Title" << "Summary";
    rootItem = new QFSimpleTreeModelItem(rootData);
    setupModelData(data.split(QString("\n")), rootItem);
}


 QFSimpleTreeModel::~QFSimpleTreeModel()
 {
     delete rootItem;
 }

 int QFSimpleTreeModel::columnCount(const QModelIndex &parent) const
 {
     if (parent.isValid())
         return static_cast<QFSimpleTreeModelItem*>(parent.internalPointer())->columnCount();
     else
         return rootItem->columnCount();
 }

 QVariant QFSimpleTreeModel::data(const QModelIndex &index, int role) const
 {
     if (!index.isValid())
         return QVariant();

     if (role != Qt::DisplayRole)
         return QVariant();

     QFSimpleTreeModelItem *item = static_cast<QFSimpleTreeModelItem*>(index.internalPointer());

     return item->data(index.column());
 }

 Qt::ItemFlags QFSimpleTreeModel::flags(const QModelIndex &index) const
 {
     if (!index.isValid())
         return 0;

     return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
 }

 QVariant QFSimpleTreeModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
 {
     if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
         return rootItem->data(section);

     return QVariant();
 }

 QModelIndex QFSimpleTreeModel::index(int row, int column, const QModelIndex &parent)
             const
 {
     if (!hasIndex(row, column, parent))
         return QModelIndex();

     QFSimpleTreeModelItem *parentItem;

     if (!parent.isValid())
         parentItem = rootItem;
     else
         parentItem = static_cast<QFSimpleTreeModelItem*>(parent.internalPointer());

     QFSimpleTreeModelItem *childItem = parentItem->child(row);
     if (childItem)
         return createIndex(row, column, childItem);
     else
         return QModelIndex();
 }

 QModelIndex QFSimpleTreeModel::parent(const QModelIndex &index) const
 {
     if (!index.isValid())
         return QModelIndex();

     QFSimpleTreeModelItem *childItem = static_cast<QFSimpleTreeModelItem*>(index.internalPointer());
     QFSimpleTreeModelItem *parentItem = childItem->parent();

     if (parentItem == rootItem)
         return QModelIndex();

     return createIndex(parentItem->row(), 0, parentItem);
 }

 int QFSimpleTreeModel::rowCount(const QModelIndex &parent) const
 {
     QFSimpleTreeModelItem *parentItem;
     if (parent.column() > 0)
         return 0;

     if (!parent.isValid())
         parentItem = rootItem;
     else
         parentItem = static_cast<QFSimpleTreeModelItem*>(parent.internalPointer());

     return parentItem->childCount();
 }

 void QFSimpleTreeModel::setupModelData(const QStringList &lines, QFSimpleTreeModelItem *parent)
 {
     QList<QFSimpleTreeModelItem*> parents;
     QList<int> indentations;
     parents << parent;
     indentations << 0;

     int number = 0;

     while (number < lines.count()) {
         int position = 0;
         while (position < lines[number].length()) {
             if (lines[number].mid(position, 1) != " ")
                 break;
             position++;
         }

         QString lineData = lines[number].mid(position).trimmed();

         if (!lineData.isEmpty()) {
             // Read the column data from the rest of the line.
             QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
             QList<QVariant> columnData;
             for (int column = 0; column < columnStrings.count(); ++column)
                 columnData << columnStrings[column];

             if (position > indentations.last()) {
                 // The last child of the current parent is now the new parent
                 // unless the current parent has no children.

                 if (parents.last()->childCount() > 0) {
                     parents << parents.last()->child(parents.last()->childCount()-1);
                     indentations << position;
                 }
             } else {
                 while (position < indentations.last() && parents.count() > 0) {
                     parents.pop_back();
                     indentations.pop_back();
                 }
             }

             // Append a new item to the current parent's list of children.
             parents.last()->appendChild(new QFSimpleTreeModelItem(columnData, parents.last()));
         }

         number++;
     }
 }






















 QFSimpleTreeModelItem::QFSimpleTreeModelItem(const QList<QVariant> &data, QFSimpleTreeModelItem *parent)
 {
     parentItem = parent;
     itemData = data;
 }

 QFSimpleTreeModelItem::~QFSimpleTreeModelItem()
 {
     qDeleteAll(childItems);
 }

 void QFSimpleTreeModelItem::appendChild(QFSimpleTreeModelItem *item)
 {
     childItems.append(item);
 }

 QFSimpleTreeModelItem *QFSimpleTreeModelItem::child(int row)
 {
     return childItems.value(row);
 }

 int QFSimpleTreeModelItem::childCount() const
 {
     return childItems.count();
 }

 int QFSimpleTreeModelItem::columnCount() const
 {
     return itemData.count();
 }

 QVariant QFSimpleTreeModelItem::data(int column) const
 {
     return itemData.value(column);
 }

 QFSimpleTreeModelItem *QFSimpleTreeModelItem::parent()
 {
     return parentItem;
 }

 int QFSimpleTreeModelItem::row() const
 {
     if (parentItem)
         return parentItem->childItems.indexOf(const_cast<QFSimpleTreeModelItem*>(this));

     return 0;
 }
