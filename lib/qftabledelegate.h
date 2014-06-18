#ifndef QFTABLEDELEGATE_H
#define QFTABLEDELEGATE_H

#include <QItemDelegate>
#include <climits>
#include <QEvent>
#include <QKeyEvent>
#include "lib_imexport.h"

/*! \brief Special item delegate for the QuickFit tables
    \ingroup qf3lib_widgets
*/
class QFLIB_EXPORT QFTableDelegate : public QItemDelegate
{
        Q_OBJECT
    public:
        explicit QFTableDelegate(QObject *parent = 0);
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
         void closeEditorEnhanced(QWidget* editor, QFTableDelegate::SpecialEndEditHint hint);
    private slots:
         void commitAndCloseEditor();

    protected:
         virtual bool eventFilter(QObject* editor, QEvent* event);

};

#endif // QFTABLEDELEGATE_H
