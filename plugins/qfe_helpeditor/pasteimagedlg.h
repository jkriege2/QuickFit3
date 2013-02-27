#ifndef PASTEIMAGEDLG_H
#define PASTEIMAGEDLG_H

#include <QDialog>

namespace Ui {
    class PasteImageDlg;
}

class PasteImageDlg : public QDialog
{
        Q_OBJECT
        
    public:
        explicit PasteImageDlg(QString directory, QWidget *parent = 0, QString filenameTemplate=QString("./pic/pix%1.png"));
        ~PasteImageDlg();

        QString saveImage() const;


    protected slots:
        void clipboardChanged();

        
    private:
        Ui::PasteImageDlg *ui;
        QString directory;
        QString filenameTemplate;
};

#endif // PASTEIMAGEDLG_H
