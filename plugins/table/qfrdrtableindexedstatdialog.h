#ifndef QFRDRTABLEINDEXEDSTATDIALOG_H
#define QFRDRTABLEINDEXEDSTATDIALOG_H

#include <QDialog>

namespace Ui {
    class QFRDRTableIndexedStatDialog;
}

class QFRDRTableIndexedStatDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit QFRDRTableIndexedStatDialog(QWidget *parent = 0);
        ~QFRDRTableIndexedStatDialog();

        void setColumns(QList<QPair<int, QString> > cols);
        int indexColumn() const;
        int dataColumn() const;
        bool avg() const;
        bool std() const;
        bool median() const;
        bool q25() const;
        bool q75() const;
        bool min() const;
        bool max() const;

    private:
        Ui::QFRDRTableIndexedStatDialog *ui;
};

#endif // QFRDRTABLEINDEXEDSTATDIALOG_H
