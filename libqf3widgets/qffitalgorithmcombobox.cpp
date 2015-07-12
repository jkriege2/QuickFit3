/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "qffitalgorithmcombobox.h"
#include "qffitalgorithmmanager.h"

QFFitAlgorithmComboBox::QFFitAlgorithmComboBox(QWidget *parent) :
    QFEnhancedComboBox(parent)
{
    onlyFMin=false;
    updateFitAlgorithms();
    actHelp=new QAction(QIcon(":/lib/help/help.png"), tr("Fit algorithm help ..."), this);
    connect(actHelp, SIGNAL(triggered()), this, SLOT(showHelpCurrent()));
    addAction(actHelp);
}

QString QFFitAlgorithmComboBox::currentFitAlgorithmID() const
{
    return itemData(currentIndex()).toString();
}

QFFitAlgorithm *QFFitAlgorithmComboBox::createCurrentInstance() const
{
    QFFitAlgorithmManager* manager=QFFitAlgorithmManager::getInstance();
    return manager->createAlgorithm(currentFitAlgorithmID());

}

void QFFitAlgorithmComboBox::showOnlyFMin(bool en)
{
    onlyFMin=en;
    updateFitAlgorithms();
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
    bool widVisible=isVisible(); if (widVisible) setUpdatesEnabled(false);
    clear();
    QStringList m_ids=manager->getIDList();
    for (int i=0; i<m_ids.size(); i++) {
        QFFitAlgorithm* a=manager->createAlgorithm(m_ids[i]);
        if (a) {
            if (!onlyFMin || (onlyFMin && a->get_implementsMinimize())) {
                if (a->isThreadSafe()) addItem(QIcon(":/lib/fitalg_icon_mt.png"), a->name() , m_ids[i]);
                else addItem(QIcon(":/lib/fitalg_icon.png"), a->name() , m_ids[i]);
                if (a->isDeprecated()) {
                    setItemData(i, QColor("grey"), Qt::TextColorRole);
                    setItemText(i, tr("[DEPRECATED]: %1").arg(itemText(i)));
                }
            }
            delete a;
        }
    }
    model()->sort(0);
    if (widVisible) setUpdatesEnabled(upd);

}


QFFitAlgorithmFMinComboBox::QFFitAlgorithmFMinComboBox(QWidget *parent):
    QFFitAlgorithmComboBox(parent)
{
    onlyFMin=true;
    updateFitAlgorithms();
}

void QFFitAlgorithmFMinComboBox::showOnlyFMin(bool /*en*/)
{
    onlyFMin=true;
    updateFitAlgorithms();
}
