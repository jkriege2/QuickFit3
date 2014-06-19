#include "pluginlinkdialog.h"
#include "ui_pluginlinkdialog.h"
#include "qfpluginservices.h"
#include "qfextensionmanager.h"
#include "qffitfunctionmanager.h"
#include "qffitalgorithmmanager.h"
#include "qfrawdatarecordfactory.h"
#include "qfevaluationitemfactory.h"

PluginLinkDialog::PluginLinkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PluginLinkDialog)
{
    ui->setupUi(this);
    QStringList ids;
    ids=QFPluginServices::getInstance()->getRawDataRecordFactory()->getIDList();
    for (int i=0; i<ids.size(); i++) {
        ui->cmbPlugin->addItem(QIcon(QFPluginServices::getInstance()->getRawDataRecordFactory()->getIconFilename(ids[i])), QFPluginServices::getInstance()->getRawDataRecordFactory()->getName(ids[i]), ids[i]);
    }
    ids=QFPluginServices::getInstance()->getEvaluationItemFactory()->getIDList();
    for (int i=0; i<ids.size(); i++) {
        ui->cmbPlugin->addItem(QIcon(QFPluginServices::getInstance()->getEvaluationItemFactory()->getIconFilename(ids[i])), QFPluginServices::getInstance()->getEvaluationItemFactory()->getName(ids[i]), ids[i]);
    }
    ids=QFPluginServices::getInstance()->getExtensionManager()->getIDList();
    for (int i=0; i<ids.size(); i++) {
        ui->cmbPlugin->addItem(QIcon(QFPluginServices::getInstance()->getExtensionManager()->getIconFilename(ids[i])), QFPluginServices::getInstance()->getExtensionManager()->getName(ids[i]), ids[i]);
    }

    for (int i=0; i<QFPluginServices::getInstance()->getFitFunctionManager()->pluginCount(); i++) {
        ui->cmbPlugin->addItem(QIcon(QFPluginServices::getInstance()->getFitFunctionManager()->getIconFilename(i)), QFPluginServices::getInstance()->getFitFunctionManager()->getName(i), QFPluginServices::getInstance()->getFitFunctionManager()->getID(i));
    }
    for (int i=0; i<QFPluginServices::getInstance()->getFitAlgorithmManager()->pluginCount(); i++) {
        ui->cmbPlugin->addItem(QIcon(QFPluginServices::getInstance()->getFitAlgorithmManager()->getIconFilename(i)), QFPluginServices::getInstance()->getFitAlgorithmManager()->getName(i), QFPluginServices::getInstance()->getFitAlgorithmManager()->getID(i));
    }

}

PluginLinkDialog::~PluginLinkDialog()
{
    delete ui;
}

QString PluginLinkDialog::insertText() const
{
    QString id=ui->cmbType->itemData(ui->cmbType->currentIndex()).toString();
    switch (ui->cmbType->currentIndex()) {
        case 0:
            return QString("<a href=\"$$plugin_info:help:%1$$\">%1 Help</a>").arg(id);
        case 1:
            return QString("<a href=\"$$plugin_info:tutorial:%1$$\">%1 Tutorial</a>").arg(id);
        case 2:
            return QString("$$plugin_info:helpdir:%1$$").arg(id);
        case 3:
            return QString("$$plugin_info:assetsdir:%1$$").arg(id);
        case 4:
            return QString("$$plugin_info:examplesdir:%1$$").arg(id);
        case 5:
            return QString("$$plugin_info:configdir:%1$$").arg(id);
        default:
            return id;
    }
    return id;
}
