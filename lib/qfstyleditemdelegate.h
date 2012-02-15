#ifndef QFSTYLEDITEMDELEGATE_H
#define QFSTYLEDITEMDELEGATE_H

#include <QItemDelegate>
#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <QSize>
#include <QSpinBox>
#include "lib_imexport.h"

/*! \brief QStyledItemDelegate descendent which displays proper editor widgets for some types
    \ingroup qf3lib_widgets

*/
class QFLIB_EXPORT QFItemDelegate : public QItemDelegate {
        Q_OBJECT
    public:
        QFItemDelegate( QObject * parent = 0);

        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
        void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

#endif // QFSTYLEDITEMDELEGATE_H
