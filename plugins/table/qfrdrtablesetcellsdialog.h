#ifndef QFRDRTABLESETCELLSDIALOG_H
#define QFRDRTABLESETCELLSDIALOG_H

#include <QDialog>
#include <QVariant>

namespace Ui {
    class QFRDRTableSetCellsDialog;
}

class QFRDRTableSetCellsDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit QFRDRTableSetCellsDialog(const QVariant& init=QVariant(), QWidget *parent = 0);
        ~QFRDRTableSetCellsDialog();
        QVariant getValue() const;
    protected slots:
        void on_cmbType_currentIndexChanged(int index);
    private:
        Ui::QFRDRTableSetCellsDialog *ui;
};

#endif // QFRDRTABLESETCELLSDIALOG_H
