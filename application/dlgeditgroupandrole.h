#ifndef DLGEDITGROUPANDROLE_H
#define DLGEDITGROUPANDROLE_H

#include "qfdialog.h"
#include "qfproject.h"


namespace Ui {
    class DlgEditGroupAndRole;
}

class DlgEditGroupAndRole : public QFDialog
{
        Q_OBJECT

    public:
        explicit DlgEditGroupAndRole(QFProject* project, QWidget *parent = 0);
        ~DlgEditGroupAndRole();

    protected slots:
        void on_btnHelp_clicked();
        void on_btnSetSelected_clicked();
        void selChanged();
        void currentIndexChanged(int index);
    private:
        Ui::DlgEditGroupAndRole *ui;
        QFProject* project;
};

#endif // DLGEDITGROUPANDROLE_H
