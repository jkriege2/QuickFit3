#ifndef QFESPIMB040SHORTCUTCONFIGDIALOG_H
#define QFESPIMB040SHORTCUTCONFIGDIALOG_H

#include <QDialog>
#include "qkeysequenceedit.h"

namespace Ui {
    class QFESPIMB040ShortcutConfigDialog;
}

class QFESPIMB040ShortcutConfigDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFESPIMB040ShortcutConfigDialog(QWidget *parent = 0);
        ~QFESPIMB040ShortcutConfigDialog();

        void addShortcut(const QString&label, const QKeySequence& current);
        QKeySequence getSequence(int i) const;
        
    private:
        Ui::QFESPIMB040ShortcutConfigDialog *ui;
        QList<QKeySequenceEdit*> edits;
};

#endif // QFESPIMB040SHORTCUTCONFIGDIALOG_H
