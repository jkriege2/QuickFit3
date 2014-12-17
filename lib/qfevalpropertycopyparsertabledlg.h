#ifndef QFEVALPROPERTYCOPYPARSERTABLEDLG_H
#define QFEVALPROPERTYCOPYPARSERTABLEDLG_H

#include <QDialog>

namespace Ui {
    class QFEvalPropertyCopyParsertableDlg;
}

class QFEvalPropertyCopyParsertableDlg : public QDialog
{
        Q_OBJECT

    public:
        explicit QFEvalPropertyCopyParsertableDlg(QWidget *parent = 0);
        ~QFEvalPropertyCopyParsertableDlg();

        bool getCopyErrors() const;
        int dataStatistics() const;
        int errorStatistics() const;

    private:
        Ui::QFEvalPropertyCopyParsertableDlg *ui;
};

#endif // QFEVALPROPERTYCOPYPARSERTABLEDLG_H
