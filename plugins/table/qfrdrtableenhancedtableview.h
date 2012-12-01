#ifndef QFRDRTABLEENHANCEDTABLEVIEW_H
#define QFRDRTABLEENHANCEDTABLEVIEW_H

#include "qenhancedtableview.h"
#include <QWidget>
#include <QAbstractItemDelegate>
#include "qfrdrtabledelegate.h"

/*! \brief Special version of QEnhancedTableView that works together with advanced editing options implemented in QFRDRTableDelegate
    \ingroup qf3rdrdp_table


*/
class QFRDRTableEnhancedTableView : public QEnhancedTableView {
        Q_OBJECT
    public:
        QFRDRTableEnhancedTableView(QWidget* parent=NULL);

    signals:
        void editExpression(const QModelIndex& index);

    protected slots:
        virtual void closeEditorEnhanced(QWidget* editor, QFRDRTableDelegate::SpecialEndEditHint hint);
};

#endif // QFRDRTABLEENHANCEDTABLEVIEW_H
