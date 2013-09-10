#ifndef USERFITFUNCTIONDELEGATE_H
#define USERFITFUNCTIONDELEGATE_H

#include "qfhtmldelegate.h"

class UserFitFunctionDelegate : public QFHTMLDelegate
{
        Q_OBJECT
    public:
        explicit UserFitFunctionDelegate(QObject *parent = 0);
        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
        void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    signals:
        
    public slots:
        void onEditorFinished();
        
};

#endif // USERFITFUNCTIONDELEGATE_H
