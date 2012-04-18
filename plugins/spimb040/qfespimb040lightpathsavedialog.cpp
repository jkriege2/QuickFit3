#include "qfespimb040lightpathsavedialog.h"
#include "ui_qfespimb040lightpathsavedialog.h"

QFESPIMB040LightPathSaveDialog::QFESPIMB040LightPathSaveDialog(QString name, QStringList names, QFTableModel *model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFESPIMB040LightPathSaveDialog)
{
    m_model=model;
    ui->setupUi(this);
    ui->cmbName->clear();
    ui->cmbName->addItems(names);
    ui->cmbName->setEditable(true);
    ui->cmbName->setEditText(name);
    if (names.contains(name)) ui->cmbName->setCurrentIndex(names.indexOf(name));
    ui->tableView->setModel(model);
}

QFESPIMB040LightPathSaveDialog::~QFESPIMB040LightPathSaveDialog()
{
    delete ui;
}

QString QFESPIMB040LightPathSaveDialog::getName() const {
    return ui->cmbName->currentText();
}

QList<bool> QFESPIMB040LightPathSaveDialog::getCheckedItems() const {
    QList<bool> result;
    for (int i=0; i<m_model->rowCount(); i++) {
        result.append(m_model->data(m_model->index(i, 0), Qt::CheckStateRole).toInt()==Qt::Checked);
    }
    return result;
}
