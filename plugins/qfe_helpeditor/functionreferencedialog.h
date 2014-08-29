#ifndef FUNCTIONREFERENCEDIALOG_H
#define FUNCTIONREFERENCEDIALOG_H

#include <QDialog>

namespace Ui {
    class FunctionReferenceDialog;
}

class FunctionReferenceDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit FunctionReferenceDialog(QWidget *parent = 0);
        ~FunctionReferenceDialog();

        QString insertText() const;

    private:
        Ui::FunctionReferenceDialog *ui;
};

#endif // FUNCTIONREFERENCEDIALOG_H
