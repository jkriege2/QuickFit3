#ifndef DLGWELCOMESCREEN_H
#define DLGWELCOMESCREEN_H

#include <QDialog>

namespace Ui {
    class DlgWelcomeScreen;
}

class DlgWelcomeScreen : public QDialog
{
        Q_OBJECT

    public:
        explicit DlgWelcomeScreen(QWidget *parent = 0);
        ~DlgWelcomeScreen();

        enum Screens {
            scFirst,
            scRecent,
            scTutorials
        };

        void addAction(Screens s, QAction* act, int pixmapSize=16, bool closeOnClick=true);
        void addSeparator(Screens s);

        void finalizeLayout();

    protected slots:
        void on_chkShow_toggled(bool enabled);
        void on_btnClose_clicked();

    private:
        Ui::DlgWelcomeScreen *ui;
};

#endif // DLGWELCOMESCREEN_H
