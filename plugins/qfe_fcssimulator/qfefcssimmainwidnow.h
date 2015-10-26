#ifndef QFEFCSSIMMAINWIDNOW_H
#define QFEFCSSIMMAINWIDNOW_H

#include <QWidget>
#include "qfefcssimscripttab.h"
#include <QList>
namespace Ui {
    class QFEFCSSimMainWidnow;
}

class QFEFCSSimMainWidnow : public QWidget
{
        Q_OBJECT

    public:
        explicit QFEFCSSimMainWidnow(QWidget *parent = 0);
        ~QFEFCSSimMainWidnow();

        void loadSettings();
        void saveSettings();
        bool mayStartProcess() const;
        QString getWorkingDir() const;
        QString getSimulator() const;
        QString getSpectraDir() const;
        bool getSaveLog() const;
    protected slots:
        void on_btnOpen_clicked();
        void on_btnNew_clicked();
        void on_btnNewFromTemplate_clicked();
        void on_btnSave_clicked();
        void on_btnSaveAs_clicked();
        void on_btnClose_clicked();
        void on_tabWidget_tabCloseRequested(int index);
        void on_btnExecute_clicked();
        void on_btnStop_clicked();
        bool closeTab(int i);
        void on_btnHelp_clicked();
        void on_btnManual_clicked();

        void textChanged(bool changed);

    protected:
        void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

    private:
        Ui::QFEFCSSimMainWidnow *ui;
        QList<QFEFCSSimScriptTab*> tabs;
        QFEFCSSimScriptTab* lastTab() const ;
        QFEFCSSimScriptTab* currentTab() const ;
};

#endif // QFEFCSSIMMAINWIDNOW_H
