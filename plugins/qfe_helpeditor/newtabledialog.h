#ifndef NEWTABLEDIALOG_H
#define NEWTABLEDIALOG_H

#include <QDialog>
#include <QLabel>
namespace Ui {
    class NewTableDialog;
}

class NewTableDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit NewTableDialog(QWidget *parent = 0);
        ~NewTableDialog();
        QString insertText() const;
    protected slots:
        void preview();

    private:
        QLabel* labPreview;
        Ui::NewTableDialog *ui;
};

#endif // NEWTABLEDIALOG_H
