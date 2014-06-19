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
        explicit PasteImageDlg(const QString& directory, QWidget *parent = 0, const QString& inputImageFile=QString(), const QString& filenameTemplate=QString("./pic/pix%1.png"));
        ~PasteImageDlg();

        QString saveImage() const;


    protected slots:
        void clipboardChanged();
        void updateImage();

        
    private:
        Ui::PasteImageDlg *ui;
        QString directory;
        QString filenameTemplate;
        QPixmap originalImage;
};

#endif // PASTEIMAGEDLG_H
