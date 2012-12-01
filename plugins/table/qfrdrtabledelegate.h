#ifndef QFRDRTABLEDELEGATE_H
#define QFRDRTABLEDELEGATE_H

#include <QItemDelegate>
#include <climits>
#include "qfrdrtable.h"
#include <QEvent>
#include <QKeyEvent>


/*! \brief Special item delegate for the QuickFit table plugin
    \ingroup qf3rdrdp_table

    \note This only will work correctly when used together with QFRDRTableEnhancedTableView !!!
*/

class QFRDRTableDelegate : public QItemDelegate
{
        Q_OBJECT
    public:
        explicit QFRDRTableDelegate(QObject *parent = 0);
        
        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,  const QModelIndex &index) const;
         void setEditorData(QWidget *editor, const QModelIndex &index) const;
         void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

         void paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;

         enum SpecialEndEditHint {
             EditOneRowUp=20,
             EditOneRowDown=21,
             EditNextRow=22,
             EditPreviousRow=23,
             EditClearExpression=24,
             EditEditExpression=25
         };

    signals:
         void closeEditorEnhanced(QWidget* editor, QFRDRTableDelegate::SpecialEndEditHint hint);
    private slots:
         void commitAndCloseEditor();
         void doClearExpression();
         void doEditExpression();

    protected:
         virtual bool eventFilter(QObject* editor, QEvent* event);
};

#endif // QFRDRTABLEDELEGATE_H
