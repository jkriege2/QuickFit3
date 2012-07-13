#ifndef QFRDRTABLECOLUMNVALUESDIALOG_H
#define QFRDRTABLECOLUMNVALUESDIALOG_H

#include <QDialog>
#include <QList>


namespace Ui {
    class QFRDRTableColumnValuesDialog;
}

class QFRDRTableColumnValuesDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFRDRTableColumnValuesDialog(QWidget *parent = 0);
        ~QFRDRTableColumnValuesDialog();

        bool addAsColumn() const;
        void setCount(int count);
        void setColumn(bool column=true);

        QList<double> getValues() const;

    protected slots:
        void updatePreview();

    private:
        Ui::QFRDRTableColumnValuesDialog *ui;
};

#endif // QFRDRTABLECOLUMNVALUESDIALOG_H
