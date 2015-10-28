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


#include "qffitfunctioncombobox.h"
#include "qffitfunctionmanager.h"
#include "qfpluginservices.h"
#include "qffitfunctionselectdialog.h"

QFFitFunctionComboBox::QFFitFunctionComboBox(QWidget *parent) :
    QFEnhancedComboBox(parent)
    //QFTreeViewComboBox(parent)
{
    setContextMenuPolicy(Qt::ActionsContextMenu);
    m_model=new QFPseudoTreeModel(this);
    //m_model=new QFSimpleTreeModel(this);
    updateFitFunctions("");
    connect(QFFitFunctionManager::getInstance(), SIGNAL(fitFunctionsChanged()), this, SLOT(reloadFitFunctions()));
    actHelp=new QFActionWithNoMenuRole(QIcon(":/lib/help/help.png"), tr("Fit model help ..."), this);
    connect(actHelp, SIGNAL(triggered()), this, SLOT(showHelpCurrent()));
    addAction(actHelp);
    actSelectModel=new QFActionWithNoMenuRole(QIcon(":/lib/select_fitfunction.png"), tr("select fit model ..."), this);
    connect(actSelectModel, SIGNAL(triggered()), this, SLOT(selectModelDialog()));
    addAction(actSelectModel);
    setModel(m_model);
}

QString QFFitFunctionComboBox::currentFitFunctionID() const
{
    return getCurrentIndexData(Qt::UserRole).toString();// itemData(currentIndex()).toString();
}

QFFitFunction *QFFitFunctionComboBox::createCurrentInstance() const
{
    QFFitFunctionManager* manager=QFFitFunctionManager::getInstance();
    if (manager) return manager->createFunction(currentFitFunctionID());
    return NULL;
}

void QFFitFunctionComboBox::setCurrentFitFunction(const QString &id)
{
    //int idx=findData(id);
    //if (idx>=0) setCurrentIndex(idx);
    //else setCurrentIndex(0);
    //qDebug()<<"setCurrentFitFunction("<<id;
    QModelIndex idx=m_model->index(QVariant(id), Qt::UserRole);
    //qDebug()<<"setCurrentFitFunction("<<id<<"): "<<idx;
    selectIndex(idx);
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
    if (m_availableFuncs.size()>0) dlg->init(m_availableFuncs, currentFitFunctionID());
    else dlg->init(m_filter, currentFitFunctionID());
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

int QFFitFunctionComboBox::currentIndex() const
{
    return QFEnhancedComboBox::currentIndex();
}

void QFFitFunctionComboBox::setCurrentIndex(int idx)
{
    QFEnhancedComboBox::setCurrentIndex(idx);
}

void QFFitFunctionComboBox::updateFitFunctions(const QString &filter)
{
    m_filter=filter;
    m_availableFuncs.clear();
    QFFitFunctionManager* manager=QFFitFunctionManager::getInstance();
    bool upd=updatesEnabled();
    //setUpdatesEnabled(false);
    m_model->clear();
    QMap<QString, QFFitFunction*> m_fitFunctions;
    if (filter.contains(",")) {
        QStringList fl=filter.split(",");
        for (int i=0; i<fl.size(); i++) {
            QMap<QString, QFFitFunction*> ff=manager->getModels(fl[i]);
            QMapIterator<QString, QFFitFunction*> itf(ff);
            while (itf.hasNext()) {
                itf.next();
                if (!m_fitFunctions.contains(itf.key()))  m_fitFunctions[itf.key()]=itf.value();
                else delete itf.value();
            }
        }
    } else {
        m_fitFunctions=manager->getModels(filter);
    }



    QMapIterator<QString, QFFitFunction*> it(m_fitFunctions);
    while (it.hasNext())  {
        it.next();
        if (it.value()) {
            //QFSimpleTreeModelItem* item=m_model->addFolderedItem(it.value()->category(), it.value()->shortName(), it.key());
            QFPseudoTreeModelItem* item=m_model->addFolderedItem(it.value()->category(), it.value()->shortName(), it.key());
            item->setIcon(QIcon(":/lib/fitfunc_icon.png"));
            if (it.value()->isDeprecated()) {
                item->setForeground(QColor("darkgrey"));
                item->setText(tr("[DEPRECATED]: %1").arg(item->text()));

            }

            delete it.value();
        }
    }
    m_model->sortTree();
    //model()->sort(0);
    //setUpdatesEnabled(upd);
}

void QFFitFunctionComboBox::updateFitFunctions(const QStringList &availableFF)
{
    m_filter="";
    m_availableFuncs=availableFF;
    if (m_availableFuncs.size()<=0) updateFitFunctions("");
    else {
        QFFitFunctionManager* manager=QFFitFunctionManager::getInstance();
        bool upd=updatesEnabled();
        //setUpdatesEnabled(false);
        QStandardItemModel *m = qobject_cast<QStandardItemModel *>(model());


        m_model->clear();
        for (int i=0; i<m_availableFuncs.size(); i++)  {
            QFFitFunction* ff=manager->createFunction(m_availableFuncs[i]);
            if (ff) {
                //QFSimpleTreeModelItem* item=m_model->addFolderedItem(ff->category(), ff->shortName(), ff->id());
                QFPseudoTreeModelItem* item=m_model->addFolderedItem(ff->category(), ff->shortName(), ff->id());
                item->setIcon(QIcon(":/lib/fitfunc_icon.png"));
                if (ff->isDeprecated()) {
                    item->setForeground(QColor("darkgrey"));
                    item->setText(tr("[DEPRECATED]: %1").arg(item->text()));

                }

                delete ff;
            }
        }
        //model()->sort(0);
        m_model->sortTree();
        //setUpdatesEnabled(upd);
    }
}
