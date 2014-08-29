#ifndef FAQENTRYDIALOG_H
#define FAQENTRYDIALOG_H

#include <QDialog>

namespace Ui {
    class FAQEntryDialog;
}

class FAQEntryDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit FAQEntryDialog(QWidget *parent = 0);
        ~FAQEntryDialog();

        QString insertText() const;

    private:
        Ui::FAQEntryDialog *ui;
};

#endif // FAQENTRYDIALOG_H
