#include "qffitalgorithmcombobox.h"
#include "qffitalgorithmmanager.h"

QFFitAlgorithmComboBox::QFFitAlgorithmComboBox(QWidget *parent) :
    QComboBox(parent)
{
    updateFitAlgorithms();
    actHelp=new QAction(QIcon(":/lib/help/help.png"), tr("Fit algorithm help ..."), this);
    connect(actHelp, SIGNAL(triggered()), this, SLOT(showHelpCurrent()));
    addAction(actHelp);
}

QString QFFitAlgorithmComboBox::currentFitAlgorithmID() const
{
    return itemData(currentIndex()).toString();
}

QFFitAlgorithm *QFFitAlgorithmComboBox::createCurrentInstance(QObject *parent) const
{
    QFFitAlgorithmManager* manager=QFFitAlgorithmManager::getInstance();
    return manager->createAlgorithm(currentFitAlgorithmID(), parent);

}

void QFFitAlgorithmComboBox::setCurrentAlgorithm(const QString &id)
{
    int idx=findData(id);
    if (idx>=0) setCurrentIndex(idx);
    else setCurrentIndex(0);
}

void QFFitAlgorithmComboBox::showHelpCurrent()
{
    QString pid=currentFitAlgorithmID();
    int ppid=QFPluginServices::getInstance()->getFitAlgorithmManager()->getPluginForID(pid);
    QString help=QFPluginServices::getInstance()->getFitAlgorithmManager()->getPluginHelp(ppid, pid);
    if (QFile::exists(help)) {
        QFPluginServices::getInstance()->displayHelpWindow(help);
     } else {
        QMessageBox::information(this, tr("Fit algorithm help"), tr("No Online-Help for this fit algorithm (%1) available.").arg(pid));
    }
}

void QFFitAlgorithmComboBox::updateFitAlgorithms()
{
    QFFitAlgorithmManager* manager=QFFitAlgorithmManager::getInstance();
    bool upd=updatesEnabled();
    setUpdatesEnabled(false);
    clear();
    QStringList m_ids=manager->getIDList();
    for (int i=0; i<m_ids.size(); i++) {
        QFFitAlgorithm* a=manager->createAlgorithm(m_ids[i], this);
        if (a->isThreadSafe()) addItem(QIcon(":/lib/fitalg_icon_mt.png"), a->name() , m_ids[i]);
        else addItem(QIcon(":/lib/fitalg_icon.png"), a->name() , m_ids[i]);
        delete a;
    }
    model()->sort(0);
    setUpdatesEnabled(upd);

}
