#ifndef QFFITFUNCTIONSELECTDIALOG_H
#define QFFITFUNCTIONSELECTDIALOG_H

#include <QDialog>
#include "libwid_imexport.h"
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

namespace Ui {
    class QFFitFunctionSelectDialog;
}

class QFWIDLIB_EXPORT QFFitFunctionSelectDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit QFFitFunctionSelectDialog(QWidget *parent = 0);
        ~QFFitFunctionSelectDialog();

        QString getSelected() const;
        void init(const QString& filter, const QString& current);

    protected slots:
        void currentRowChanged(const QModelIndex &current, const QModelIndex &previous);
        void on_edtFilter_textChanged( const QString & text );
    private:
        Ui::QFFitFunctionSelectDialog *ui;
        QStandardItemModel* model;
        QSortFilterProxyModel filterModel;
};

#endif // QFFITFUNCTIONSELECTDIALOG_H
