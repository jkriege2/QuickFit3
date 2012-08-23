#ifndef QFRDRTABLESORTDIALOG_H
#define QFRDRTABLESORTDIALOG_H

#include <QDialog>

namespace Ui {
    class QFRDRTableSortDialog;
}

class QFRDRTableSortDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFRDRTableSortDialog(QWidget *parent = 0);
        ~QFRDRTableSortDialog();
        
        void setColumns(QList<QPair<int, QString> > cols);
        void setCurrentColumn(int col);
        int sortColumn() const;

        bool sortAscending() const;

    private:
        Ui::QFRDRTableSortDialog *ui;
};

#endif // QFRDRTABLESORTDIALOG_H
