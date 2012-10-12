#ifndef DLGCONTACTAUTHORS_H
#define DLGCONTACTAUTHORS_H

#include <QDialog>

namespace Ui {
    class DlgContactAuthors;
}

class DlgContactAuthors : public QDialog
{
        Q_OBJECT
        
    public:
        explicit DlgContactAuthors(QWidget *parent = 0, bool contactMailinglist=false);
        ~DlgContactAuthors();

    protected slots:
        void acceptDialog();
        void openHelp();

    protected slots:
        void on_cmbSendTo_currentIndexChanged(int index);
    private:
        Ui::DlgContactAuthors *ui;
};

#endif // DLGCONTACTAUTHORS_H
