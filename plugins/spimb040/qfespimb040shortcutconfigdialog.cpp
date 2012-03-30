#include "qfespimb040shortcutconfigdialog.h"
#include "ui_qfespimb040shortcutconfigdialog.h"

QFESPIMB040ShortcutConfigDialog::QFESPIMB040ShortcutConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFESPIMB040ShortcutConfigDialog)
{
    ui->setupUi(this);
}

QFESPIMB040ShortcutConfigDialog::~QFESPIMB040ShortcutConfigDialog()
{
    delete ui;
}

void QFESPIMB040ShortcutConfigDialog::addShortcut(const QString &label, const QKeySequence &current) {
    QKeySequenceEdit* edt=new QKeySequenceEdit(this);
    ui->layWidgets->addRow(label, edt);
    edt->setKeySequence(current);
    edits.append(edt);
}

QKeySequence QFESPIMB040ShortcutConfigDialog::getSequence(int i) const {
    if (i>=0 && i<edits.size()) {
        return edits[i]->getKeySequence();
    }
    return QKeySequence();
}
