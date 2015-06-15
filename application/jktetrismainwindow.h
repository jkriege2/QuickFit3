#ifndef JKTETRISMAINWINDOW_H
#define JKTETRISMAINWINDOW_H

#include <QWidget>
#include <QMessageBox>
namespace Ui {
    class JKTetrisMainWindow;
}

class JKTetrisMainWindow : public QWidget
{
        Q_OBJECT

    public:
        explicit JKTetrisMainWindow(QWidget *parent = 0);
        ~JKTetrisMainWindow();

    protected slots:
        void gameOver(int score, int level);
        void on_btnClose_clicked();
        void closeEvent(QCloseEvent* event);
        void focusOutEvent(QFocusEvent * event);
    private:
        Ui::JKTetrisMainWindow *ui;
};

#endif // JKTETRISMAINWINDOW_H
