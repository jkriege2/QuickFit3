#ifndef QFEVALPROPERTYCOPYPARSERTABLEDLG_H
#define QFEVALPROPERTYCOPYPARSERTABLEDLG_H

#include "qfdialog.h"

namespace Ui {
    class QFEvalPropertyCopyParsertableDlg;
}

class QFEvalPropertyCopyParsertableDlg : public QFDialog
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
