#ifndef PLUGINLINKDIALOG_H
#define PLUGINLINKDIALOG_H

#include <QDialog>

namespace Ui {
    class PluginLinkDialog;
}

class PluginLinkDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit PluginLinkDialog(QWidget *parent = 0);
        ~PluginLinkDialog();

        QString insertText() const;

    private:
        Ui::PluginLinkDialog *ui;
};

#endif // PLUGINLINKDIALOG_H
