#include "subpluginlinkdialog.h"
#include "ui_subpluginlinkdialog.h"
#include "qfpluginservices.h"
#include "qfextensionmanager.h"
#include "qffitfunctionmanager.h"
#include "qffitalgorithmmanager.h"
#include "qfrawdatarecordfactory.h"
#include "qfevaluationitemfactory.h"
#include "qfimporter.h"
#include "qfimportermanager.h"

SubPluginLinkDialog::SubPluginLinkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SubPluginLinkDialog)
{
    ui->setupUi(this);

    for (int i=0; i<QFPluginServices::getInstance()->getFitFunctionManager()->pluginCount(); i++) {
        QStringList lst=QFPluginServices::getInstance()->getFitFunctionManager()->getIDList(i);
        for (int j=0; j<lst.size(); j++) {
            QFFitFunction* f=QFPluginServices::getInstance()->getFitFunctionManager()->createFunction(lst[j]);
            if (f) {
                QStringList e;
                e<<f->id()<<"fitfunction";
                ui->cmbPlugin->addItem(QIcon(QFPluginServices::getInstance()->getFitFunctionManager()->getIconFilename(i)), f->name(), e);
                delete f;
            }
        }
    }
    for (int i=0; i<QFPluginServices::getInstance()->getFitAlgorithmManager()->pluginCount(); i++) {
        QStringList lst=QFPluginServices::getInstance()->getFitAlgorithmManager()->getIDList(i);
        for (int j=0; j<lst.size(); j++) {
            QFFitAlgorithm* f=QFPluginServices::getInstance()->getFitAlgorithmManager()->createAlgorithm(lst[j]);
            if (f) {
                QStringList e;
                e<<f->id()<<"fitalgorithm";
                ui->cmbPlugin->addItem(QIcon(QFPluginServices::getInstance()->getFitAlgorithmManager()->getIconFilename(i)), f->name(), e);
                delete f;
            }
        }
    }
    for (int i=0; i<QFPluginServices::getInstance()->getImporterManager()->pluginCount(); i++) {
        QStringList lst=QFPluginServices::getInstance()->getImporterManager()->getIDList(i);
        for (int j=0; j<lst.size(); j++) {
            QFImporter* f=QFPluginServices::getInstance()->getImporterManager()->createImporter(lst[j]);
            if (f) {
                QStringList e;
                e<<lst[j]<<"importer";
                ui->cmbPlugin->addItem(QIcon(QFPluginServices::getInstance()->getImporterManager()->getIconFilename(i)), f->formatName(), e);
                delete f;
            }
        }
    }

}

SubPluginLinkDialog::~SubPluginLinkDialog()
{
    delete ui;
}

QString SubPluginLinkDialog::insertText() const
{
    QStringList lst=ui->cmbPlugin->itemData(ui->cmbPlugin->currentIndex()).toStringList();
    QString id=lst.value(0);
    QString cl=lst.value(1);
    switch (ui->cmbType->currentIndex()) {
        case 0:
            return QString("<a href=\"$$%1:help:%2$$\">$$%1:name:%2$$</a>").arg(cl).arg(id);
        case 1:
            return QString("$$%1:help:%2$$").arg(cl).arg(id);
        case 2:
            return QString("$$%1:name:%2$$").arg(cl).arg(id);
        case 3:
            return QString("$$%1:short_name:%2$$").arg(cl).arg(id);
        default:
            return id;
    }
    return id;
}
