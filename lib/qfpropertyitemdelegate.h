#ifndef QFPROPERTYITEMDELEGATE_H
#define QFPROPERTYITEMDELEGATE_H

#include "qfstyleditemdelegate.h"
#include "lib_imexport.h"
#include "qfproject.h"

/*! \brief QStyledItemDelegate specialized for QFRDRPropertyModel, it displays a proper combobox, if the user edits the first or second column (editable property name, non-editable property type)
    \ingroup qf3lib_widgets

*/
class QFLIB_EXPORT QFPropertyItemDelegate : public QFItemDelegate {
        Q_OBJECT
    public:
        QFPropertyItemDelegate( QObject * parent = 0);

        void setProject(QFProject* project);
        QFProject* project() const;

        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    protected:
        QFProject* m_project;
};

#endif // QFPROPERTYITEMDELEGATE_H
