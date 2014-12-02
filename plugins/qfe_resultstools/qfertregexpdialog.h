#ifndef QFERTREGEXPDIALOG_H
#define QFERTREGEXPDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>

namespace Ui {
    class QFERTRegExpDialog;
}

class QFERTRegExpDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit QFERTRegExpDialog(QWidget *parent = 0);
        ~QFERTRegExpDialog();

    protected slots:
        void refresh();
        void on_btnHelp_clicked();

    private:
        Ui::QFERTRegExpDialog *ui;

        enum { MaxCaptures = 8 };
        QLabel *captureLabels[MaxCaptures];
        QLineEdit *captureEdits[MaxCaptures];
};

#endif // QFERTREGEXPDIALOG_H
