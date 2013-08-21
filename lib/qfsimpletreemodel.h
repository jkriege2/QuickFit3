#ifndef QFSIMPLETREEMODEL_H
#define QFSIMPLETREEMODEL_H

#include <QAbstractItemModel>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "lib_imexport.h"
#include <QList>
#include <QVariant>

class  QFSimpleTreeModelItem; // forward
/*! \brief this class implements a very simple tree model for Qt's model-view-framework, which may contain nodes (selectable) and selectable, as well as non-selectable folders
    \ingroup tools_qt


 */
class QFLIB_EXPORT  QFSimpleTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit QFSimpleTreeModel(const QString &data, QObject *parent = 0);
    ~QFSimpleTreeModel();
    
    QVariant data(const QModelIndex &index, int role) const;
     Qt::ItemFlags flags(const QModelIndex &index) const;
     QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
     QModelIndex index(int row, int column,  const QModelIndex &parent = QModelIndex()) const;
     QModelIndex parent(const QModelIndex &index) const;
     int rowCount(const QModelIndex &parent = QModelIndex()) const;
     int columnCount(const QModelIndex &parent = QModelIndex()) const;

 private:
     void setupModelData(const QStringList &lines, QFSimpleTreeModelItem *parent);

     QFSimpleTreeModelItem *rootItem;

};









 class QFLIB_EXPORT QFSimpleTreeModelItem
 {
 public:
     QFSimpleTreeModelItem(const QList<QVariant> &data, QFSimpleTreeModelItem *parent = 0);
     ~QFSimpleTreeModelItem();

     void appendChild(QFSimpleTreeModelItem *child);

     QFSimpleTreeModelItem *child(int row);
     int childCount() const;
     int columnCount() const;
     QVariant data(int column) const;
     int row() const;
     QFSimpleTreeModelItem *parent();

 private:
     QList<QFSimpleTreeModelItem*> childItems;
     QList<QVariant> itemData;
     QFSimpleTreeModelItem *parentItem;
 };

#endif // QFSIMPLETREEMODEL_H
