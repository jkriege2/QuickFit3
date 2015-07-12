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

#include "qfevaluationpropertyeditor.h"
#include "qfprojectrawdatamodel.h"
#include "qfevaluationitemfactory.h"
#include "qfversion.h"
#include "qfproject.h"
#include "qfevaluationpropertyeditorprivate.h"

QFEvaluationRawDataModelProxy::QFEvaluationRawDataModelProxy(QObject *parent):
    QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    eval=NULL;
    editor=NULL;
};


bool QFEvaluationRawDataModelProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    int ID=-1;
    if (index.isValid()) {
        ID=sourceModel()->data(index, Qt::UserRole).toInt();
    }
    if ((eval!=NULL) && (ID>=0)) {
        QFRawDataRecord* record=eval->getProject()->getRawDataByID(ID);
        if (record==NULL) return false;
        else return eval->isFilteredAndApplicable(record);
    }
    return false;
}

QVariant QFEvaluationRawDataModelProxy::data(const QModelIndex &index, int role) const {
    if (eval!=NULL) if (eval->getUseSelection()) {
        if (role==Qt::CheckStateRole) {
            int ID=QSortFilterProxyModel::data(index, Qt::UserRole).toInt();
            QFRawDataRecord* record=eval->getProject()->getRawDataByID(ID);
            return eval->isSelected(record)?QVariant(Qt::Checked):QVariant(Qt::Unchecked);
        }
    }
    return QSortFilterProxyModel::data(index, role);
}

bool QFEvaluationRawDataModelProxy::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (eval!=NULL) if (eval->getUseSelection()) {
        if (role==Qt::CheckStateRole) {
            int ID=QSortFilterProxyModel::data(index, Qt::UserRole).toInt();
            QFRawDataRecord* record=eval->getProject()->getRawDataByID(ID);
            if (value.toInt()==0) {
                eval->deselectRecord(record);
            } else {
                eval->selectRecord(record);
            }
            return true;
        }
    }
    return QSortFilterProxyModel::setData(index, value, role);
}

Qt::ItemFlags QFEvaluationRawDataModelProxy::flags(const QModelIndex &index) const {
    Qt::ItemFlags def=QSortFilterProxyModel::flags(index);
    if (eval!=NULL) if (eval->getUseSelection()) {
        def|=Qt::ItemIsUserCheckable;
    }
    return def;
}


void QFEvaluationRawDataModelProxy::setEvaluation(QFEvaluationItem* eval) {
    if (this->eval!=NULL) {
        disconnect(this->eval, SIGNAL(selectionChanged(QList<QPointer<QFRawDataRecord> >)), this, SLOT(selectionChanged(QList<QPointer<QFRawDataRecord> >)));
    }
    this->eval=eval;
    if (eval!=NULL) {
        connect(eval, SIGNAL(selectionChanged(QList<QPointer<QFRawDataRecord> >)), this, SLOT(selectionChanged(QList<QPointer<QFRawDataRecord> >)));
        invalidateFilter();
    }

}

void QFEvaluationRawDataModelProxy::setEditor(QFEvaluationPropertyEditor* editor) {
    this->editor=editor;
}


void QFEvaluationRawDataModelProxy::selectionChanged(QList<QPointer<QFRawDataRecord> > /*selectedRecords*/) {
    //std::cout<<"QFEvaluationRawDataModelProxy::selectionChanged()\n";
    invalidateFilter();
    //qDebug()<<rowCount()<<editor;
    if (editor && (rowCount()<=0)) {
        editor->closeBecauseNoRDRs();
    }
}










QFEvaluationPropertyEditor::QFEvaluationPropertyEditor(QFPluginServices* services, ProgramOptions* set, QFEvaluationItem* current, int id, QWidget* parent, Qt::WindowFlags f):
    QWidget(parent, f)
{
    p=new QFEvaluationPropertyEditorPrivate(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    //std::cout<<"creating QFEvaluationPropertyEditor ... \n";
    p->id=id;
    this->current=NULL;
    p->services=services;
    p->layWidgets=NULL;
    resultsModel=new QFEvaluationResultsModel(this);
    resultsModel->init(NULL, "*");

    rdrModel=new QFProjectRawDataModel(this, NULL);
    rdrProxy=new QFEvaluationRawDataModelProxy(rdrModel);
    rdrProxy->setSourceModel(rdrModel);
    p->lstRawData=NULL;
    p->splitMain=NULL;
    p->filesListFiltered=true;

    resize(400,300);
    move(5,5);

    setSettings(set);
    //std::cout<<"creating QFEvaluationPropertyEditor ... creating widgets ...\n";
    p->createWidgets();

    resize(400,300);
    move(5,5);
    //std::cout<<"creating QFEvaluationPropertyEditor ... setting current ...\n";
    setCurrent(current);
    //std::cout<<"creating QFEvaluationPropertyEditor ... DONE!\n";
    readSettings();

}

QFEvaluationPropertyEditor::~QFEvaluationPropertyEditor() {
    //std::cout<<"deleting QFEvaluationPropertyEditor\n";
    //std::cout<<"deleting QFEvaluationPropertyEditor ... OK\n";
    if (p) {
        p->disconnect();
        //delete p;
    }
}

void QFEvaluationPropertyEditor::closeEvent ( QCloseEvent * event ) {
    //std::cout<<"QFEvaluationPropertyEditor::closeEvent\n";
    writeSettings();
    event->accept();
    //std::cout<<"QFEvaluationPropertyEditor::closeEvent ... OK\n";
}

void QFEvaluationPropertyEditor::setCurrent(QFEvaluationItem* c) {
    if (current==c) return;
    QString oldType="";
    //int oldEditorCount=0;
    if (current) {
        //std::cout<<"disconnecting old ...\n";
        p->lstRawData->setModel(NULL);
        p->compDisplayProperties->setProject(NULL);
        rdrProxy->setEvaluation(NULL);
        rdrProxy->setEditor(NULL);
        rdrModel->setProject(NULL);
        resultsModel->init(NULL, "*");
        oldType=current->getType();
        //oldEditorCount=current->getEditorCount();
        disconnect(current->getProject(), SIGNAL(recordAboutToBeDeleted(QFRawDataRecord*)), p, SLOT(recordAboutToBeDeleted(QFRawDataRecord*)));
        disconnect(current->getProject(), SIGNAL(evaluationAboutToBeDeleted(QFEvaluationItem*)), p, SLOT(evaluationAboutToBeDeleted(QFEvaluationItem*)));
        disconnect(p->edtName, SIGNAL(textChanged(const QString&)), p, SLOT(nameChanged(const QString&)));
        disconnect(p->pteDescription, SIGNAL(textChanged()), p, SLOT(descriptionChanged()));
        disconnect(current, SIGNAL(propertiesChanged(const QString&,bool)), p, SLOT(propsChanged(QString,bool)));
        disconnect(p->lstRawData->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), p, SLOT(selectionChanged(const QModelIndex&, const QModelIndex&)));
        disconnect(rdrProxy, SIGNAL(modelReset()), p, SLOT(rdrModelReset()));
        disconnect(current, SIGNAL(resultsChanged(QFRawDataRecord*,QString,QString)), p, SLOT(resultsChanged(QFRawDataRecord*,QString,QString)));
        disconnect(p->tvResults->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), p, SLOT(tvResultsSelectionChanged(const QItemSelection&, const QItemSelection&)));
        disconnect(p->edtFilterRecords, SIGNAL(textChanged(QString)), p, SLOT(filterRecordsChanged()));
        disconnect(p->edtFilterRecordsNot, SIGNAL(textChanged(QString)), p, SLOT(filterRecordsChanged()));
        disconnect(p->chkFilterRecordsRegExp, SIGNAL(toggled(bool)), p, SLOT(filterRecordsChanged()));
        if (c) {
            if (c->getType()!=oldType) {
                /*for (int i=oldEditorCount; i>=0; i--) {
                    QWidget* w=tabEditors->widget(i);
                    tabEditors->removeTab(i);
                    if (qobject_cast<QFEvaluationEditor *>(w)) qobject_cast<QFEvaluationEditor *>(w)->setSettings(NULL, id);
                    w->deleteLater();
                    //delete w;
                }*/
                if (p->editor) {
                    p->editor->close();
                    p->editor->setSettings(NULL, p->id);
                    p->layWidgets->removeWidget(p->editor);
                    p->editor->deleteLater();
                }
            }
        }
        //std::cout<<"old disconnected ...\n";
    }
    current=c;
    if (current) {
        //std::cout<<"connecting new ...\n";
        rdrModel->setProject(current->getProject());
        p->compDisplayProperties->setProject(current->getProject());
        rdrProxy->setEvaluation(current);
        rdrProxy->setEditor(this);
        resultsModel->init(current, current->getResultsDisplayFilter());
        p->widRDRList->setVisible(current->getShowRDRList());
        if (current->getType()!=oldType) {
            //editorList.clear();
            //for (int i=0; i<current->getEditorCount(); i++) {
                QString n=current->getEditorName();
                //std::cout<<"creating tab '"<<n.toStdString()<<"' ... \n";
                QFEvaluationEditor* e=current->createEditor(p->services, this, this);
                //std::cout<<"creating tab '"<<n.toStdString()<<"' ... reading settings\n";
                e->setSettings(p->settings, p->id);
                //std::cout<<"creating tab '"<<n.toStdString()<<"' ... setting current\n";
                e->setCurrent(current, p->id);
                //std::cout<<"creating tab '"<<n.toStdString()<<"' ... adding tab\n";
                //tabEditors->addTab(e, n);
                p->layWidgets->addWidget(e);
                //std::cQFEvaluationPropertyEditor::setSettings(out<<"creating tab '"<<n.toStdString()<<"' ... done\n";
                p->editor=e;
            //}
        } else {
            p->editor->setCurrent(current, p->id);
        }
        p->edtName->setText(current->getName());
        p->edtName->setEnabled(true);
        p->pteDescription->setPlainText(current->getDescription());
        p->pteDescription->setEnabled(true);
        p->labID->setText(QString::number(current->getID()));
        /*labTopIcon->setPixmap(current->getSmallIcon().pixmap(16,16));
        labTop->setText(tr("<b>%1</b>").arg(current->getName()));*/
        setWindowTitle(current->getName());
        setWindowIcon(current->getSmallIcon());

        if (p->filesListFiltered && current->getShowRDRList()) {
            p->edtFilterRecords->setText(current->getNameFilter());
            p->edtFilterRecordsNot->setText(current->getNameNotFilter());
            p->chkFilterRecordsRegExp->setChecked(current->getNameFilterRegExp());
            setFilesListFilteres(true);
        } else {
            setFilesListFilteres(false);
        }


        p->labType->setText(current->getTypeDescription());
        p->labTypeIcon->setPixmap(current->getSmallIcon().pixmap(16,16));
        p->lstRawData->setModel(rdrProxy);
        connect(p->edtName, SIGNAL(textChanged(const QString&)), p, SLOT(nameChanged(const QString&)));
        connect(p->pteDescription, SIGNAL(textChanged()), p, SLOT(descriptionChanged()));
        connect(current->getProject(), SIGNAL(evaluationAboutToBeDeleted(QFEvaluationItem*)), p, SLOT(evaluationAboutToBeDeleted(QFEvaluationItem*)));
        connect(current->getProject(), SIGNAL(recordAboutToBeDeleted(QFRawDataRecord*)), p, SLOT(recordAboutToBeDeleted(QFRawDataRecord*)));
        connect(current, SIGNAL(propertiesChanged(QString,bool)), p, SLOT(propsChanged(QString,bool)));
        if (current->getShowRDRList()) {
            connect(p->edtFilterRecords, SIGNAL(textChanged(QString)), p, SLOT(filterRecordsChanged()));
            connect(p->edtFilterRecordsNot, SIGNAL(textChanged(QString)), p, SLOT(filterRecordsChanged()));
            connect(p->chkFilterRecordsRegExp, SIGNAL(toggled(bool)), p, SLOT(filterRecordsChanged()));
            p->lstRawData->selectionModel()->select(rdrProxy->index(0,0), QItemSelectionModel::SelectCurrent);
            p->selectionChanged(rdrProxy->index(0,0), rdrProxy->index(0,0));//std::cout<<"new connected ...\n";
        }
        connect(p->lstRawData->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), p, SLOT(selectionChanged(const QModelIndex&, const QModelIndex&)));
        connect(rdrProxy, SIGNAL(modelReset()), p, SLOT(rdrModelReset()));
        connect(current, SIGNAL(resultsChanged(QFRawDataRecord*,QString,QString)), p, SLOT(resultsChanged(QFRawDataRecord*,QString,QString)));
        connect(p->tvResults->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), p, SLOT(tvResultsSelectionChanged(const QItemSelection&, const QItemSelection&)));


        QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/");
        p->compFilterFiles->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+current->getType()+"_evfilterfiles.txt");
        p->compFilterFilesNot->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+current->getType()+"_evfilterfilesnot.txt");
        p->compFilterResults->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+current->getType()+"_evfilterresults.txt");
        p->compFilterResultsNot->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+current->getType()+"_evfilterresults_not.txt");
        p->compFilterRecords->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+current->getType()+"_evfilterrecords.txt");
        p->compFilterRecordsNot->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+current->getType()+"_evfilterrecordsnot.txt");

        p->loadSettings();


        p->filterRecordsChanged();

        /*helpWidget->clear();
        QString dll=current->getProject()->getEvaluationItemFactory()->getPluginHelp(current->getType());
        helpWidget->updateHelp(dll);*/

    } else {
        p->edtName->setText("");
        p->edtName->setEnabled(false);
        p->pteDescription->setPlainText("");
        p->pteDescription->setEnabled(false);
        /*labTopIcon->setText("");
        labTop->setText("");*/
        p->labID->setText("");
        p->labType->setText("");
        p->labTypeIcon->setText("");
        p->lstRawData->setModel(NULL);
        rdrProxy->setEvaluation(NULL);
        rdrProxy->setEditor(NULL);
        rdrModel->setProject(NULL);
        p->compFilterFiles->setFilename("");
        p->compFilterFilesNot->setFilename("");
        p->compFilterRecords->setFilename("");
        p->compFilterRecordsNot->setFilename("");
        p->compFilterResults->setFilename("");
        p->compFilterResultsNot->setFilename("");

    }
    if (p->tabMain->count()>2) p->tabMain->setCurrentIndex(1);
    p->showAvgClicked(p->chkShowAvg->isChecked());
    p->checkHelpAvailable();
}


void QFEvaluationPropertyEditor::setSettings(ProgramOptions* settings) {
    p->settings=settings;
    //std::cout<<"QFEvaluationPropertyEditor::setSettings("<<settings<<")\n";
    if (current && p->layWidgets) {
        if (p->editor) {
            p->editor->setSettings(p->settings, p->id);
        }
    }
    readSettings();
}

void QFEvaluationPropertyEditor::setFilesListFilteres(bool filtered)
{
    if (!current) return;
    p->filesListFiltered=filtered;
    p->widFilterRecords->setVisible(filtered);
    if (!filtered) {
        current->setNameNameNotFilter("", "", false, false);
    } else {
        current->setNameNameNotFilter(p->edtFilterRecords->text(), p->edtFilterRecordsNot->text(), p->chkFilterRecordsRegExp->isChecked(), p->chkFilterRecordsRegExp->isChecked());
    }
    rdrProxy->invalidate();
}

bool QFEvaluationPropertyEditor::isFilesListFiltered() const
{
    return p->filesListFiltered;
}

QFEvaluationItem *QFEvaluationPropertyEditor::getCurrent() const
{
    return current;
}


void QFEvaluationPropertyEditor::readSettings() {
    if (!p->settings) return;
    p->settings->getQSettings()->sync();
    if (p->tabMain) {
        //int idx=settings->getQSettings()->value("evalpropeditor/currentTab", 0).toInt();
    }
    loadWidgetGeometry(*(p->settings->getQSettings()), this, QPoint(10, 10), QSize(800, 600), "evalpropeditor/");
    if (p->splitMain) loadSplitter(*(p->settings->getQSettings()), p->splitMain, "evalpropeditor/");
    if (p->editor) p->editor->readSettings();
    p->currentSaveDir=p->settings->getQSettings()->value("evalpropeditor/lastSaveDir", p->currentSaveDir).toString();
}

void QFEvaluationPropertyEditor::writeSettings() {
    if (!p->settings) return;
    saveWidgetGeometry(*(p->settings->getQSettings()), this, "evalpropeditor/");
    saveSplitter(*(p->settings->getQSettings()), p->splitMain, "evalpropeditor/");
    if (p->tabMain) {
        p->settings->getQSettings()->setValue("evalpropeditor/currentTab", p->tabMain->currentIndex());
    }

    if (p->editor) p->editor->writeSettings();
    p->settings->getQSettings()->setValue("evalpropeditor/lastSaveDir", p->currentSaveDir);
}

int QFEvaluationPropertyEditor::getID()
{
    return p->id;
}

void QFEvaluationPropertyEditor::deselectCurrent() {
    if (p->lstRawData && p->lstRawData->model()) {
        QModelIndex current=p->lstRawData->selectionModel()->currentIndex();
        int rows=p->lstRawData->model()->rowCount();

        QModelIndex next;

        //qDebug()<<current<<current.row()<<rows;

        if (current.row()>0) {
            next=p->lstRawData->model()->index(current.row()-1, current.column());

        } else {
            if (rows>1) next=p->lstRawData->model()->index(rows-1, current.column());
        }
        //qDebug()<<next;
        //lstRawData->selectionModel()->select(next, QItemSelectionModel::SelectCurrent);
        if (next.isValid()) {
            p->lstRawData->selectionModel()->setCurrentIndex(next, QItemSelectionModel::SelectCurrent);
        } else {
            closeBecauseNoRDRs();
        }
    }
}

QMenuBar *QFEvaluationPropertyEditor::getMenuBar() const
{
    return p->menuBar;
}

void QFEvaluationPropertyEditor::setMenuBarVisible(bool visible)
{
    p->menuBar->setVisible(visible);
}

QMenu *QFEvaluationPropertyEditor::addMenu(const QString &title, int editor)
{
    p->menuBar->removeAction(p->menuHelp->menuAction());
    QMenu* m=p->menuBar->addMenu(title);
    p->menuBar->addAction(p->menuHelp->menuAction());
    p->menus.append(qMakePair(editor, m));
    p->currentTabChanged(p->tabMain->currentIndex());
    return m;
}

QMenu *QFEvaluationPropertyEditor::addOrFindMenu(const QString &title, int editor)
{
    for (int i=0; i<p->menus.size(); i++)  {
        if ((p->menus[i].first==editor || p->menus[i].first<0) && p->menus[i].second && p->menus[i].second->title()==title) return p->menus[i].second;
    }
    return addMenu(title, editor);
}

void QFEvaluationPropertyEditor::registerMenu(QMenu *menu, int editor)
{
    p->menus.append(qMakePair(editor, menu));
    p->currentTabChanged(p->tabMain->currentIndex());
}

QMenu *QFEvaluationPropertyEditor::getHelpMenu() const
{
    return p->menuHelp;
}

QPointer<QFEvaluationEditor> QFEvaluationPropertyEditor::getEditor() const
{
    return p->editor;
}

void QFEvaluationPropertyEditor::sendEditorCommand(const QString &command, const QVariant &param1, const QVariant &param2, const QVariant &param3, const QVariant &param4, const QVariant &param5)
{
    if (getEditor()) {
        getEditor()->sendEditorCommand(command, param1, param2, param3, param4, param5);
    }
}

void QFEvaluationPropertyEditor::closeBecauseNoRDRs()
{
    hide();
    QMessageBox::information(NULL, tr("QF3 Evaluation Editor Closed ..."), tr("QuickFit 3.0 closed an evaluation editor window, because no applicable raw data records (RDRs) were found in the current project.\n\nAdd RDRs to the project, that this evaluation is applicable to, before opening the evaluation editor!"), QMessageBox::Ok, QMessageBox::Ok);
    close();
}


void QFEvaluationPropertyEditor::resizeEvent(QResizeEvent* event) {
    p->labAveragedresults->setMaximumWidth(event->size().width());
}

/*
void QFEvaluationPropertyEditor::selectionChanged(QList<QPointer<QFRawDataRecord> > selectedRecords) {
    if (current) {

    }
}

void QFEvaluationPropertyEditor::highlightingChanged(QFRawDataRecord* formerRecord, QFRawDataRecord* currentRecord) {

}*/

bool QFEvaluationPropertyEditor::event(QEvent * ev) {
    QTime t;
    t.start();
    //qDebug()<<"~~~ QFEvaluationPropertyEditor::event("<<ev->type()<<")";
    bool ok=QWidget::event(ev);
    //qDebug()<<"~~~ QFEvaluationPropertyEditor::event("<<ev->type()<<") done: "<<t.elapsed()<<" ms";
    return ok;
}
