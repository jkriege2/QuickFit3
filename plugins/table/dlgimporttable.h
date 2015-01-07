#ifndef DLGIMPORTTABLE_H
#define DLGIMPORTTABLE_H

#include <QDialog>
#include <QItemSelectionModel>
#include <QAbstractItemModel>
#include "qftablepluginmodel.h"

namespace Ui {
    class DlgIMportTable;
}

class DlgIMportTable : public QDialog
{
        Q_OBJECT

    public:
        explicit DlgIMportTable(QWidget *parent = 0);
        explicit DlgIMportTable(bool pastAgainEnabled, QWidget *parent = 0);
        ~DlgIMportTable();

        bool importAll() const;
        bool importExpressions() const;
        void setModel(QAbstractItemModel* model);
        QItemSelectionModel* getSelection() const;
        int importMode() const;

        void doImport(QFTablePluginModel* model_out, int currentRow, int currentCol);

    protected slots:
        void on_radImportAll_toggled(bool checked);
        void selectionCHanged();
        void on_btnPasteAgain_clicked();

    private:
        Ui::DlgIMportTable *ui;
};

#endif // DLGIMPORTTABLE_H
