#ifndef SubPluginLinkDialog_H
#define SubPluginLinkDialog_H

#include <QDialog>

namespace Ui {
    class SubPluginLinkDialog;
}

class SubPluginLinkDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit SubPluginLinkDialog(QWidget *parent = 0);
        ~SubPluginLinkDialog();

        QString insertText() const;

    private:
        Ui::SubPluginLinkDialog *ui;
};

#endif // SubPluginLinkDialog_H
