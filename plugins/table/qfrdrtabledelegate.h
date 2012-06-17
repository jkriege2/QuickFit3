#ifndef QFRDRTABLEDELEGATE_H
#define QFRDRTABLEDELEGATE_H

#include <QItemDelegate>
#include <climits>

class QFRDRTableDelegate : public QItemDelegate
{
        Q_OBJECT
    public:
        explicit QFRDRTableDelegate(QObject *parent = 0);
        
        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,  const QModelIndex &index) const;
         void setEditorData(QWidget *editor, const QModelIndex &index) const;
         void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

     private slots:
         void commitAndCloseEditor();
};

#endif // QFRDRTABLEDELEGATE_H
