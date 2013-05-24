#ifndef QFFitFunctionValueInputDELEGATE_H
#define QFFitFunctionValueInputDELEGATE_H

#include "qfhtmldelegate.h"
#include "lib_imexport.h"

class QFLIB_EXPORT QFFitFunctionValueInputDelegate : public QFHTMLDelegate {
        Q_OBJECT
    public:
        explicit QFFitFunctionValueInputDelegate(QObject *parent = 0);

        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
        void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    signals:
        
    protected slots:
        void onEditorFinished();
    protected:
        
};

#endif // QFFitFunctionValueInputDELEGATE_H
