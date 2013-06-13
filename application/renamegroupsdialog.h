#ifndef RENAMEGROUPSDIALOG_H
#define RENAMEGROUPSDIALOG_H

#include <QDialog>
#include <QStringListModel>

namespace Ui {
    class RenameGroupsDialog;
}

class RenameGroupsDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit RenameGroupsDialog(QWidget *parent = 0);
        ~RenameGroupsDialog();

        void setStrings(const QStringList& list);
        QStringList getStrings() const;
    private:
        Ui::RenameGroupsDialog *ui;

        QStringListModel model;
};

#endif // RENAMEGROUPSDIALOG_H
