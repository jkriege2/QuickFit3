#include "qffitfunctioncombobox.h"
#include "qffitfunctionmanager.h"
#include "qfpluginservices.h"
#include "qffitfunctionselectdialog.h"

QFFitFunctionComboBox::QFFitFunctionComboBox(QWidget *parent) :
    QComboBox(parent)
{
    setContextMenuPolicy(Qt::ActionsContextMenu);
    updateFitFunctions("");
    connect(QFFitFunctionManager::getInstance(), SIGNAL(fitFunctionsChanged()), this, SLOT(reloadFitFunctions()));
    actHelp=new QAction(QIcon(":/lib/help/help.png"), tr("Fit model help ..."), this);
    connect(actHelp, SIGNAL(triggered()), this, SLOT(showHelpCurrent()));
    addAction(actHelp);
    actSelectModel=new QAction(QIcon(":/lib/select_fitfunction.png"), tr("select fit model ..."), this);
    connect(actSelectModel, SIGNAL(triggered()), this, SLOT(selectModelDialog()));
    addAction(actSelectModel);
}

QString QFFitFunctionComboBox::currentFitFunctionID() const
{
    return itemData(currentIndex()).toString();
}

QFFitFunction *QFFitFunctionComboBox::createCurrentInstance(QObject* parent) const
{
    QFFitFunctionManager* manager=QFFitFunctionManager::getInstance();
    return manager->createFunction(currentFitFunctionID(), parent);
}

void QFFitFunctionComboBox::setCurrentFitFunction(const QString &id)
{
    int idx=findData(id);
    if (idx>=0) setCurrentIndex(idx);
    else setCurrentIndex(0);
}

void QFFitFunctionComboBox::showHelpCurrent()
{
    QString pid=currentFitFunctionID();
    int ppid=QFPluginServices::getInstance()->getFitFunctionManager()->getPluginForID(pid);
    QString help=QFPluginServices::getInstance()->getFitFunctionManager()->getPluginHelp(ppid, pid);
    if (QFile::exists(help)) {
        QFPluginServices::getInstance()->displayHelpWindow(help);
     } else {
        QMessageBox::information(this, tr("Fit model help ..."), tr("No Online-Help for this fit function (%1) available.").arg(pid));
    }
}

void QFFitFunctionComboBox::selectModelDialog()
{
    QFFitFunctionSelectDialog* dlg=new QFFitFunctionSelectDialog(this);
    dlg->init(m_filter, currentFitFunctionID());
    if (dlg->exec()) {
        setCurrentFitFunction(dlg->getSelected());
    }
    delete dlg;
}

void QFFitFunctionComboBox::reloadFitFunctions()
{
    QString id=currentFitFunctionID();
    updateFitFunctions(m_filter);
    setCurrentFitFunction(id);
}

void QFFitFunctionComboBox::updateFitFunctions(const QString &filter)
{
    m_filter=filter;
    QFFitFunctionManager* manager=QFFitFunctionManager::getInstance();
    bool upd=updatesEnabled();
    setUpdatesEnabled(false);
    clear();
    QMap<QString, QFFitFunction*> m_fitFunctions=manager->getModels(filter, this);
    QMapIterator<QString, QFFitFunction*> it(m_fitFunctions);
    while (it.hasNext())  {
        it.next();
        if (it.value()) {
            addItem(QIcon(":/lib/fitfunc_icon.png"), it.value()->shortName(), it.key());
            delete it.value();
        }
    }
    model()->sort(0);
    setUpdatesEnabled(upd);
}
