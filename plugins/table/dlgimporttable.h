#ifndef DLGIMPORTTABLE_H
#define DLGIMPORTTABLE_H

#include <QDialog>
#include <QItemSelectionModel>
#include <QAbstractItemModel>

namespace Ui {
    class DlgIMportTable;
}

class DlgIMportTable : public QDialog
{
        Q_OBJECT

    public:
        explicit DlgIMportTable(QWidget *parent = 0);
        ~DlgIMportTable();

        bool importAll() const;
        void setModel(QAbstractItemModel* model);
        QItemSelectionModel* getSelection() const;
        int importMode() const;

    protected slots:
        void on_radImportAll_toggled(bool checked);
        void selectionCHanged();

    private:
        Ui::DlgIMportTable *ui;
};

#endif // DLGIMPORTTABLE_H
