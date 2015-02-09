/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "qffitfunctionselectdialog.h"
#include "ui_qffitfunctionselectdialog.h"
#include "qffitfunctionmanager.h"
#include "qfpluginservices.h"
#include "qffitfunction.h"
#include "libwid_imexport.h"
#include "qfhtmlhelptools.h"
#include "qftools.h"

QFFitFunctionSelectDialog::QFFitFunctionSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFFitFunctionSelectDialog)
{
    setWindowTitle(tr("Select fit model function ..."));
    model=NULL;
    ui->setupUi(this);
    filterModel.setDynamicSortFilter(true);
    filterModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    filterModel.setFilterRole(Qt::UserRole+10);
    ui->lstModels->setModel(&filterModel);
    ui->lstModels->expandAll();
    connect(ui->lstModels->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(currentRowChanged(QModelIndex,QModelIndex)));
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(0,2);

    QSettings* set=ProgramOptions::getInstance()->getQSettings();
    if (set) {
        loadWidgetGeometry(*set, this, pos(), size(), "QFFitFunctionSelectDialog/windowsize");
        loadSplitter(*set, ui->splitter, "QFFitFunctionSelectDialog/splitter");
    }
}

QFFitFunctionSelectDialog::~QFFitFunctionSelectDialog()
{
    QSettings* set=ProgramOptions::getInstance()->getQSettings();
    if (set) {
        saveWidgetGeometry(*set, this, "QFFitFunctionSelectDialog/windowsize");
        saveSplitter(*set, ui->splitter, "QFFitFunctionSelectDialog/splitter");
    }
    delete ui;
}

QString QFFitFunctionSelectDialog::getSelected() const
{
    QString id="";
    if (ui->lstModels->currentIndex().row()>=0) {
        id=ui->lstModels->currentIndex().data(Qt::UserRole+1).toString();
    }
    return id;
}

void QFFitFunctionSelectDialog::init(const QString &filter, const QString &current)
{
    QFFitFunctionManager* manager=QFFitFunctionManager::getInstance();
    bool upd=updatesEnabled();
    /*int idx=ui->lstModels->currentIndex().row();
    if (idx<0) idx=0;*/
    QModelIndex idx=ui->lstModels->currentIndex();
    bool widVisible=isVisible(); if (widVisible) setUpdatesEnabled(false);

    filterModel.setSourceModel(NULL);
    if (model) delete model;
    model=new QFSimpleTreeModel(this);
    //QMap<QString, QFFitFunction*> m_fitFunctions=manager->getModels(filter, this);

    QMap<QString, QFFitFunction*> m_fitFunctions;
    if (filter.contains(",")) {
        QStringList fl=filter.split(",");
        for (int i=0; i<fl.size(); i++) {
            QMap<QString, QFFitFunction*> ff=manager->getModels(fl[i], this);
            QMapIterator<QString, QFFitFunction*> itf(ff);
            while (itf.hasNext()) {
                itf.next();
                if (!m_fitFunctions.contains(itf.key()))  m_fitFunctions[itf.key()]=itf.value();
                else delete itf.value();
            }
        }
    } else {
        m_fitFunctions=manager->getModels(filter, this);
    }

    QFSimpleTreeModel *m = model;

    QMapIterator<QString, QFFitFunction*> it(m_fitFunctions);
    int i=0;

    while (it.hasNext())  {
        it.next();
        if (it.value()) {

            QFSimpleTreeModelItem* item=model->addFolderedItem(it.value()->category(), it.value()->shortName(), it.key());
            item->setData(it.key(), Qt::UserRole+1);
            item->setIcon(QIcon(":/lib/fitfunc_icon.png"));
            item->setData(it.value()->category()+QString("/")+it.value()->shortName()+QString("/")+it.key(), Qt::UserRole+10);
            if (it.value()->isDeprecated() && m) {
                item->setForeground(QColor("darkgrey"));
                item->setText(tr("[DEPRECATED]: %1").arg(item->text()));

            }

            if (it.key()==current) idx=model->index(item);//idx=i;
            i++;
            delete it.value();
        }
    }
    filterModel.setSourceModel(model);
    ui->lstModels->setModel(&filterModel);
    ui->lstModels->expandAll();
    //model->sort(0);
    model->sort();
    if (widVisible) setUpdatesEnabled(upd);
    ui->lstModels->expandAll();
    ui->lstModels->setCurrentIndex(idx);//model->index(idx, 0));
    ui->edtFilter->clear();
}

void QFFitFunctionSelectDialog::init(const QStringList &availableFF, const QString &current)
{
    if (availableFF.size()<=0) init("", current);
    else {
        QFFitFunctionManager* manager=QFFitFunctionManager::getInstance();
        bool upd=updatesEnabled();
        QModelIndex idx=ui->lstModels->currentIndex();//.row();
        //if (idx<0) idx=0;
        bool widVisible=isVisible(); if (widVisible) setUpdatesEnabled(false);

        filterModel.setSourceModel(NULL);
        if (model) delete model;
        model=new QFSimpleTreeModel(this);
        QMap<QString, QFFitFunction*> m_fitFunctions;
        for (int i=0; i<availableFF.size(); i++) {
            m_fitFunctions[availableFF[i]]=manager->createFunction(availableFF[i], this);
        }

        QFSimpleTreeModel *m = model;

        QMapIterator<QString, QFFitFunction*> it(m_fitFunctions);
        int i=0;

        while (it.hasNext())  {
            it.next();
            if (it.value()) {
                /*QStandardItem* item=new QStandardItem(QIcon(":/lib/fitfunc_icon.png"), it.value()->name());
                item->setData(it.key(), Qt::UserRole+1);
                model->appendRow(item);

                if (it.value()->isDeprecated() && m) {
                    item->setForeground(QColor("darkgrey"));
                    item->setText(tr("[DEPRECATED]: %1").arg(item->text()));

                }*/
                QFSimpleTreeModelItem* item=model->addFolderedItem(it.value()->category(), it.value()->shortName(), it.key());
                item->setData(it.key(), Qt::UserRole+1);
                item->setIcon(QIcon(":/lib/fitfunc_icon.png"));
                item->setData(it.value()->category()+QString("/")+it.value()->shortName()+QString("/")+it.key(), Qt::UserRole+10);
                if (it.value()->isDeprecated() && m) {
                    item->setForeground(QColor("darkgrey"));
                    item->setText(tr("[DEPRECATED]: %1").arg(item->text()));

                }

                if (it.key()==current) idx=model->index(item);
                i++;
                delete it.value();
            }
        }
        filterModel.setSourceModel(model);
        ui->lstModels->setModel(&filterModel);
        ui->lstModels->expandAll();
        //model->sort(0);
        model->sort();
        if (widVisible) setUpdatesEnabled(upd);
        ui->lstModels->expandAll();
        ui->lstModels->setCurrentIndex(idx);//model->index(idx, 0));
        ui->edtFilter->clear();
    }
}

void QFFitFunctionSelectDialog::currentRowChanged(const QModelIndex & current, const QModelIndex & previous)
{


    QString pid=getSelected();
    int ppid=QFPluginServices::getInstance()->getFitFunctionManager()->getPluginForID(pid);
    QString help=QFPluginServices::getInstance()->getFitFunctionManager()->getPluginHelp(ppid, pid);
    ui->txtHelp->setHtml(transformQF3HelpHTMLFile(help));
}

void QFFitFunctionSelectDialog::on_edtFilter_textChanged(const QString &text)
{
    filterModel.setFilterRegExp(QRegExp(QString("*")+text+QString("*"), Qt::CaseInsensitive, QRegExp::Wildcard));
    ui->lstModels->expandAll();
}
