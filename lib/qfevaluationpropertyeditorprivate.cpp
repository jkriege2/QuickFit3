/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#include "qfevaluationpropertyeditorprivate.h"
#include "qfevaluationpropertyeditor.h"
#include "qfprojectrawdatamodel.h"
#include "qfevaluationitemfactory.h"
#include "qfversion.h"
#include "qfproject.h"
#include "qfenhancedtabwidget.h"
#include "datatools.h"
QFEvaluationPropertyEditorPrivate::QFEvaluationPropertyEditorPrivate(QFEvaluationPropertyEditor *parent) :
    QObject(parent)
{
    this->d=parent;
}
























void QFEvaluationPropertyEditorPrivate::resultsChanged(QFRawDataRecord* record, const QString& evalName, const QString& resultName) {
    if (!d->resultsModel) return;
    //resultsModel->resultsChanged();
    if (tvResults->model()->columnCount()*tvResults->model()->rowCount()<10000) tvResults->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

void QFEvaluationPropertyEditorPrivate::refreshResults()
{
    d->resultsModel->resultsChanged();
    resultsChanged();
}

void QFEvaluationPropertyEditorPrivate::displayHelp() {
    if (tabMain->currentIndex()==0 || tabMain->currentIndex()==tabMain->count()-1) {
        displayHelpEval();
    } else {
        displayHelpPlugin();
    }

}

void QFEvaluationPropertyEditorPrivate::displayHelpPlugin()
{
    QString dll=d->current->getProject()->getEvaluationItemFactory()->getPluginHelp(d->current->getType());
    services->displayHelpWindow(dll);
}

void QFEvaluationPropertyEditorPrivate::displayHelpPluginTutorial()
{
    QString dll=d->current->getProject()->getEvaluationItemFactory()->getPluginTutorialMain(d->current->getType());
    services->displayHelpWindow(dll);
}

void QFEvaluationPropertyEditorPrivate::displayHelpPluginCopyright()
{
    QString dll=d->current->getProject()->getEvaluationItemFactory()->getPluginCopyrightFile(d->current->getType());
    services->displayHelpWindow(dll);
}

void QFEvaluationPropertyEditorPrivate::displayHelpEval()
{
    QString dll=services->getOptions()->getAssetsDirectory()+QString("/help/qf3_evalscreen.html");
    services->displayHelpWindow(dll);
}

void QFEvaluationPropertyEditorPrivate::copyValErrResults() {
    tvResults->copySelectionAsValueErrorToExcel(QFEvaluationResultsModel::AvgRole, QFEvaluationResultsModel::SDRole);
}

void QFEvaluationPropertyEditorPrivate::copyValErrResultsNoHead() {
    tvResults->copySelectionAsValueErrorToExcel(QFEvaluationResultsModel::AvgRole, QFEvaluationResultsModel::SDRole, false);
}

void QFEvaluationPropertyEditorPrivate::copyMedianQuantilesResults(){
    tvResults->copySelectionAsMedianQuantilesToExcel(QFEvaluationResultsModel::MedianRole, QFEvaluationResultsModel::Quantile25Role, QFEvaluationResultsModel::Quantile75Role);
}

void QFEvaluationPropertyEditorPrivate::copyMedianQuantilesResultsNoHead() {
    tvResults->copySelectionAsMedianQuantilesToExcel(QFEvaluationResultsModel::MedianRole, QFEvaluationResultsModel::Quantile25Role, QFEvaluationResultsModel::Quantile75Role, false);
}

void QFEvaluationPropertyEditorPrivate::copyExpandedResults()
{
    tvResults->copySelectionToExcelExpanded(Qt::EditRole, true);
}

void QFEvaluationPropertyEditorPrivate::copyExpandedResultsNoHead()
{
    tvResults->copySelectionToExcelExpanded(Qt::EditRole, false);
}

void QFEvaluationPropertyEditorPrivate::copyExpandedResultsNoHeadMatlab()
{
    tvResults->copySelectionToMatlabExpandedNoHead(Qt::EditRole);
}

void QFEvaluationPropertyEditorPrivate::copyExpandedResultsFlipped()
{
    tvResults->copySelectionToExcelExpanded(Qt::EditRole, true, true);
}

void QFEvaluationPropertyEditorPrivate::copyExpandedResultsNoHeadFlipped()
{
    tvResults->copySelectionToExcelExpanded(Qt::EditRole, false, true);
}

void QFEvaluationPropertyEditorPrivate::copyExpandedResultsNoHeadMatlabFlipped()
{
    tvResults->copySelectionToMatlabExpandedNoHead(Qt::EditRole, true);
}
void QFEvaluationPropertyEditorPrivate::currentTabChanged(int tab)
{
    int idx=tab-1;
    for (int i=0; i<menus.size(); i++) {
        if (menus[i].first==-1 || (idx>=0 && menus[i].first==idx)) {
            menus[i].second->menuAction()->setVisible(true);
        } else {
            menus[i].second->menuAction()->setVisible(false);
        }
    }
    menuResults->menuAction()->setVisible(tab==tabMain->count()-1);
}

void QFEvaluationPropertyEditorPrivate::checkHelpAvailable()
{
    if (!d->current) {
        actHelpPlugin->setVisible(false);
        actHelpPluginTutorial->setVisible(false);
        actHelpPluginCopyright->setVisible(false);
    } else {
        QString dll=d->current->getProject()->getEvaluationItemFactory()->getPluginHelp(d->current->getType());
        actHelpPlugin->setVisible(QFile::exists(dll));
        dll=d->current->getProject()->getEvaluationItemFactory()->getPluginTutorialMain(d->current->getType());
        actHelpPluginTutorial->setVisible(QFile::exists(dll));
        dll=d->current->getProject()->getEvaluationItemFactory()->getPluginCopyrightFile(d->current->getType());
        actHelpPluginCopyright->setVisible(QFile::exists(dll));
    }

}

void QFEvaluationPropertyEditorPrivate::propertiesTextChanged(const QString &text) {
    if (!d->current) return;
    if (!d->resultsModel) return;
    if (text.isEmpty()) {
        d->resultsModel->setDisplayProperties(QStringList());
    } else {
        QStringList p=text.split(",");
        for (int i=0; i<p.size(); i++) {
            p[i]=p[i].trimmed();
        }
        d->resultsModel->setDisplayProperties(p);
    }
}

void QFEvaluationPropertyEditorPrivate::filterRecordsChanged()
{
    d->setFilesListFilteres(filesListFiltered);
}

void QFEvaluationPropertyEditorPrivate::deleteSelectedRecords() {
    if (!d->current) return;
    QModelIndexList sel=tvResults->selectionModel()->selectedIndexes();
    if (sel.size()>0) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::question(d, tr("QuickFit %1").arg(qfInfoVersion()),
                     tr("Do you really want to delete the selected results?"),
                     QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (ret == QMessageBox::Yes) {

            QSet<QFRawDataRecord*> recs;
            for (int i=0; i<sel.size(); i++) {
                QString en=sel[i].data(QFEvaluationResultsModel::EvalNameRole).toString();
                QString rn=sel[i].data(QFEvaluationResultsModel::ResultNameRole).toString();
                int rid=sel[i].data(QFEvaluationResultsModel::ResultIDRole).toInt();
                if (rid>=0) {
                    QFRawDataRecord* r=d->current->getProject()->getRawDataByID(rid);
                    if (r) {
                        recs.insert(r);
                        r->disableEmitResultsChanged();
                        r->resultsRemove(en, rn, false);
                    }
                }
            }
            QSetIterator<QFRawDataRecord *> i(recs);
            while (i.hasNext()) {
                i.next()->enableEmitResultsChanged(true);
            }
        }
    }
}

void QFEvaluationPropertyEditorPrivate::showAvgClicked(bool checked)
{
    if (d->current && d->resultsModel) {
        d->resultsModel->setShowVectorMatrixAvg(checked);
    }
}

void QFEvaluationPropertyEditorPrivate::showStatistics()
{

    QFHistogramService* hs=QFHistogramService::getInstance();
    if (hs&&d->current) {
        QModelIndexList idxs=tvResults->selectionModel()->selectedIndexes();
        QMap<int, QFHistogramService::Histogram> hists;
        for (int i=0; i<idxs.size(); i++) {
            QFHistogramService::Histogram h;
            int col=idxs[i].column();
            if (!hists.contains(col)) {
                h.name=d->resultsModel->headerData(col, Qt::Horizontal).toString();
                hists[col]=h;
            }
            QString ename=d->resultsModel->data(idxs[i], QFEvaluationResultsModel::EvalNameRole).toString();
            QString rname=d->resultsModel->data(idxs[i], QFEvaluationResultsModel::ResultNameRole).toString();
            int rid=d->resultsModel->data(idxs[i], QFEvaluationResultsModel::ResultIDRole).toInt();

            QFRawDataRecord* record=d->current->getProject()->getRawDataByID(rid);

            if (record) hists[col].data<<record->resultsGetAsDoubleList(ename, rname);
        }

        bool onePerCol=true;
        if (hists.size()>1) {
            onePerCol=QMessageBox::question(d, tr("Data histogram"), tr("You selected %1 columns.\nShould QuickFit open\n   [Yes] one histogram window per column, or\n   [No]  combine all data into one window?").arg(hists.size()), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes;
        }

        if (onePerCol) {
            QMapIterator<int, QFHistogramService::Histogram> ii(hists);
            while (ii.hasNext()) {
                ii.next();
                QString histID=QString("hist")+d->current->getType()+QString::number(d->current->getID())+"_"+QString::number(ii.key());
                hs->getCreateView(histID, tr("Histogram from %1").arg(d->current->getName()));
                hs->clearView(histID);
                hs->addHistogramToView(histID, ii.value());
            }
        } else {
            QString histID=QString("hist")+d->current->getType()+QString::number(d->current->getID());
            hs->getCreateView(histID, tr("Histogram from %1").arg(d->current->getName()));
            hs->clearView(histID);
            QMapIterator<int, QFHistogramService::Histogram> ii(hists);
            while (ii.hasNext()) {
                ii.next();
                hs->addHistogramToView(histID, ii.value());
            }
        }
    }

}

void QFEvaluationPropertyEditorPrivate::showStatisticsComparing()
{

    QFHistogramService* hs=QFHistogramService::getInstance();
    if (hs&&d->current) {
        QModelIndexList idxs=tvResults->selectionModel()->selectedIndexes();
        QMap<int, QList<QFHistogramService::Histogram> > hists;
        for (int i=0; i<idxs.size(); i++) {
            QFHistogramService::Histogram h;
            int col=idxs[i].column();
            if (!hists.contains(col)) {
                QList<QFHistogramService::Histogram> l;
                hists[col]=l;
            }
            QString ename=d->resultsModel->data(idxs[i], QFEvaluationResultsModel::EvalNameRole).toString();
            QString rname=d->resultsModel->data(idxs[i], QFEvaluationResultsModel::ResultNameRole).toString();
            int rid=d->resultsModel->data(idxs[i], QFEvaluationResultsModel::ResultIDRole).toInt();
            QFRawDataRecord* record=d->current->getProject()->getRawDataByID(rid);

            h.name=d->resultsModel->headerData(col, Qt::Vertical).toString();
            if (record) h.data<<record->resultsGetAsDoubleList(ename, rname);
            hists[col].append(h);
        }

        bool onePerCol=true;
        if (hists.size()>1) {
            onePerCol=QMessageBox::question(d, tr("Data histogram"), tr("You selected %1 columns.\nShould QuickFit open\n   [Yes] one histogram window per column, or\n   [No]  combine all data into one window?").arg(hists.size()), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes;
        }

        if (onePerCol) {
            QMapIterator<int, QList<QFHistogramService::Histogram> > ii(hists);
            while (ii.hasNext()) {
                ii.next();
                QString histID=QString("hist")+d->current->getType()+QString::number(d->current->getID())+"_"+QString::number(ii.key());
                hs->getCreateView(histID, tr("Histogram from %1").arg(d->current->getName()));
                hs->clearView(histID);
                for (int i=0; i<ii.value().size(); i++) {
                    hs->addHistogramToView(histID, ii.value().at(i));
                }
            }
        } else {
            QString histID=QString("hist")+d->current->getType()+QString::number(d->current->getID());
            hs->getCreateView(histID, tr("Histogram from %1").arg(d->current->getName()));
            hs->clearView(histID);
            QMapIterator<int,  QList<QFHistogramService::Histogram> > ii(hists);
            while (ii.hasNext()) {
                ii.next();
                for (int i=0; i<ii.value().size(); i++) {
                    hs->addHistogramToView(histID, ii.value().at(i));
                }
            }
        }
    }

}

void QFEvaluationPropertyEditorPrivate::storeSettings()
{
    if (!d->current) return;
    d->current->setQFProperty("RESULTS_PROPERTIES", edtDisplayProperties->text(), false, false);
    d->current->setQFProperty("FILES_FILTER", edtFilterFiles->text(), false, false);
    d->current->setQFProperty("FILES_FILTERNOT", edtFilterFilesNot->text(), false, false);
    d->current->setQFProperty("FILES_FILTER_REGEXP", chkFilterFilesRegExp->isChecked(), false, false);
    d->current->setQFProperty("RESULTS_FILTER", edtFilterResults->text(), false, false);
    d->current->setQFProperty("RESULTS_FILTERNOT", edtFilterResultsNot->text(), false, false);
    d->current->setQFProperty("RESULTS_FILTER_REGEXP", chkFilterResultsRegExp->isChecked(), false, false);


    d->current->setQFProperty("RESULTS_MIND_UNUSED_VECSTAT", chkMindUnusedForVectorStat->isChecked(), false, false);
    d->current->setQFProperty("RESULTS_EXTRACT_RUNS", chkExtractRuns->isChecked(), false, false);
    d->current->setQFProperty("RESULTS_REMOVE_UNUSED_RUNS", chkRemoveUnusedRuns->isChecked(), false, false);
    d->current->setQFProperty("RESULTS_SHOW_AVG", chkShowAvg->isChecked(), false, false);
}

void QFEvaluationPropertyEditorPrivate::loadSettings()
{
    if (!d->current) return;
    edtDisplayProperties->setText(d->current->getProperty("RESULTS_PROPERTIES", "").toString());
    edtFilterFiles->setText(d->current->getProperty("FILES_FILTER", "").toString());
    edtFilterFilesNot->setText(d->current->getProperty("FILES_FILTERNOT", "").toString());
    chkFilterFilesRegExp->setChecked(d->current->getProperty("FILES_FILTER_REGEXP", false).toBool());
    edtFilterResults->setText(d->current->getProperty("RESULTS_FILTER", "").toString());
    edtFilterResultsNot->setText(d->current->getProperty("RESULTS_FILTERNOT", "").toString());
    chkFilterResultsRegExp->setChecked(d->current->getProperty("RESULTS_FILTER_REGEXP", false).toBool());


    chkMindUnusedForVectorStat->setChecked(d->current->getProperty("RESULTS_MIND_UNUSED_VECSTAT", true).toBool());
    chkExtractRuns->setChecked(d->current->getProperty("RESULTS_EXTRACT_RUNS", true).toBool());
    chkRemoveUnusedRuns->setChecked(d->current->getProperty("RESULTS_REMOVE_UNUSED_RUNS", true).toBool());
    chkShowAvg->setChecked(d->current->getProperty("RESULTS_SHOW_AVG", true).toBool());
}


void QFEvaluationPropertyEditorPrivate::nameChanged(const QString& text) {
    if (d->current) {
        d->current->setName(text);
    }
}

void QFEvaluationPropertyEditorPrivate::descriptionChanged() {
    if (d->current) {
        d->current->setDescription(pteDescription->toPlainText());
    }
}

void QFEvaluationPropertyEditorPrivate::evaluationAboutToBeDeleted(QFEvaluationItem* r) {
    if ((d->current==r) && d->current) {
        d->close();
    }
}


void QFEvaluationPropertyEditorPrivate::propsChanged(const QString& property, bool visible) {
    if (d->current) {
        if (d->current->getName()!=edtName->text()) {
            edtName->setText(d->current->getName());
        }
        if (d->current->getDescription()!=pteDescription->toPlainText()) {
            pteDescription->setPlainText(d->current->getDescription());
        }
        /*labTopIcon->setPixmap(d->current->getSmallIcon().pixmap(16,16));
        labTop->setText(tr("<b>%1</b>").arg(d->current->getName()));*/
        QString prefix="";
        int whm=ProgramOptions::getConfigValue("quickfit/windowheadermode", 1).toInt();
        if (whm==1) {
            prefix=QFileInfo(d->current->getProject()->getFile()).fileName();
        } else if (whm==2) {
            prefix=d->current->getProject()->getName();
            if (prefix.isEmpty()) prefix=QFileInfo(d->current->getProject()->getFile()).fileName();
        }
        if (!prefix.isEmpty()) prefix += QString(": ");

        d->setWindowTitle(QString("%2Evaluation: %1").arg(d->current->getName()).arg(prefix));
        d->setWindowIcon(d->current->getSmallIcon());
        labID->setText(QString::number(d->current->getID()));
        labType->setText(d->current->getTypeDescription());
        labTypeIcon->setPixmap(d->current->getSmallIcon().pixmap(16,16));
    }
}


void QFEvaluationPropertyEditorPrivate::createWidgets() {
    QVBoxLayout* ml=new QVBoxLayout();
    d->setLayout(ml);
    ml->setContentsMargins(2,2,2,2);


    menuBar=new QMenuBar(d);
    menuBar->setVisible(true);
    ml->addWidget(menuBar);
    menuResults=menuBar->addMenu("&Results");
    menuHelp=menuBar->addMenu("&Help");

    tabMain=new QFEnhancedTabWidget(d);
    ml->addWidget(tabMain);

    QWidget* w=new QWidget(tabMain);
    QFormLayout* fl=new QFormLayout();
    w->setLayout(fl);
    tabMain->addTab(w, tr("&Properties"));
    labID=new QLabel(w);
    fl->addRow(tr("ID:"), labID);
    labType=new QLabel(w);
    labTypeIcon=new QLabel(w);
    QHBoxLayout* ptl=new QHBoxLayout();
    ptl->setContentsMargins(0,0,0,0);
    ptl->addWidget(labTypeIcon);
    ptl->addWidget(labType);
    ptl->addStretch();
    fl->addRow(tr("Type:"), ptl);
    edtName=new QLineEdit(w);
    fl->addRow(tr("&Name:"), edtName);
    pteDescription=new QPlainTextEdit(w);
    fl->addRow(tr("&Description:"), pteDescription);

    splitMain=new QVisibleHandleSplitter(tabMain);
    lstRawData=new QListView(splitMain);
    //tabEditors=new QTabWidget(splitMain);
    layWidgets=new QHBoxLayout();
    QWidget* wl=new QWidget(d);
    wl->setLayout(layWidgets);
    //splitMain->addWidget(tabEditors);
    splitMain->addWidget(wl);
    //splitMain->addWidget(lstRawData);
    QVBoxLayout* lstvbl=new QVBoxLayout();
    widRDRList=new QWidget(splitMain);
    widRDRList->setLayout(lstvbl);

    widFilterRecords=new QWidget(d);
    QHBoxLayout* lFilterRecords=new QHBoxLayout();
    widFilterRecords->setLayout(lFilterRecords);
    lFilterRecords->addWidget(new QLabel("filter: "));
    edtFilterRecords=new QFEnhancedLineEdit(d);
    edtFilterRecords->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFilterRecords, edtFilterRecords));
    edtFilterRecords->setCompleter(new QFCompleterFromFile());
    compFilterRecords=new QFCompleterFromFile(d);
    edtFilterRecords->setCompleter(compFilterRecords);
    lFilterRecords->addWidget(edtFilterRecords);
    lFilterRecords->addWidget(new QLabel(tr(" <span style=\"text-decoration: overline\">filter</span>: "), d));
    edtFilterRecordsNot=new QFEnhancedLineEdit(d);
    edtFilterRecordsNot->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFilterRecordsNot, edtFilterRecordsNot));
    compFilterRecordsNot=new QFCompleterFromFile(d);
    edtFilterRecordsNot->setCompleter(compFilterRecordsNot);
    lFilterRecords->addWidget(edtFilterRecordsNot);
    chkFilterRecordsRegExp=new QCheckBox(tr("RegExp"), d);
    chkFilterRecordsRegExp->setChecked(false);
    lFilterRecords->addWidget(chkFilterRecordsRegExp);
    edtFilterRecords->setToolTip(tr("use this to filter the contents of the rdr list<br><br>"
                                       "Simply enter a filter string and the list will only display those"
                                       "rows where the raw data record name contains a match to the filter string."
                                       "Depending on whether <i>RegExp</i> is checked or not, you can use"
                                       "either simple text with wildcards '<tt>*</tt>' (match any characters) and '<tt>?</tt>'"
                                       "(match a single character), or full regular expressions in the filter string."
                                  "<br><br><font color=\"darkred\">red text</font> means you entered an invalid regular expression"));
    edtFilterRecordsNot->setToolTip(tr("use this to filter the contents of the rdr list<br><br>"
                                       "Simply enter a filter string and the list will only display those"
                                       "rows where the raw data record name contains NO match to the filter string."
                                       "Depending on whether <i>RegExp</i> is checked or not, you can use"
                                       "either simple text with wildcards '<tt>*</tt>' (match any characters) and '<tt>?</tt>'"
                                       "(match a single character), or full regular expressions in the filter string."
                                     "<br><br><font color=\"darkred\">red text</font> means you entered an invalid regular expression"));
    connect(edtFilterRecordsNot, SIGNAL(textChanged(QString)), this, SLOT(filterRecordsTextChanged(QString)));
    connect(edtFilterRecords, SIGNAL(textChanged(QString)), this, SLOT(filterRecordsTextChanged(QString)));

    lstvbl->addWidget(widFilterRecords);

    lstvbl->addWidget(lstRawData, 10);
    btnRemoveRawData=new QPushButton(QIcon(":/lib/item_delete.png"), tr("remove record"), widRDRList);
    btnRemoveRawData->setToolTip(tr("remove the current raw data record from the project"));
    connect(btnRemoveRawData, SIGNAL(clicked()), this, SLOT(removeRawData()));
    lstvbl->addWidget(btnRemoveRawData);

    splitMain->addWidget(widRDRList);

    splitMain->setCollapsible(0, false);
    splitMain->setCollapsible(1, false);
    splitMain->setStretchFactor(0,5);
    splitMain->setStretchFactor(1,1);

    //connect(lstRawData, SIGNAL(activated(const QModelIndex&)), this, SLOT(selectionChanged(const QModelIndex&)));
    //connect(lstRawData, SIGNAL(clicked(const QModelIndex&)), this, SLOT(selectionChanged(const QModelIndex&)));
    //connect(lstRawData, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(selectionChanged(const QModelIndex&)));
    //connect(lstRawData->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(selectionChanged(const QModelIndex&, const QModelIndex&)));

    //QItemSelectionModel::SelectCurrent

    tabMain->addTab(splitMain, tr("&Evaluation"));




    widResults=new QWidget(d);
    QVBoxLayout* rwvlayout=new QVBoxLayout();
    widResults->setLayout(rwvlayout);

    tvResults=new QEnhancedTableView(widResults);
    actCopyResults=tvResults->getActCopyExcel();
    actCopyResultsNoHead=tvResults->getActCopyExcelNoHead();
    tvResults->setAlternatingRowColors(true);
    tvResults->setItemDelegate(new QFHTMLDelegate(tvResults));
    QFontMetrics fm(d->font());
    tvResults->verticalHeader()->setDefaultSectionSize((int)round((double)fm.height()*1.5));
    tvResults->verticalHeader()->setResizeMode(QHeaderView::Interactive);
    tvResults->verticalHeader()->setTextElideMode(Qt::ElideMiddle);
    tvResults->verticalHeader()->setMaximumWidth(750);
    tvResults->setModel(d->resultsModel);
    tvResults->setContextMenuPolicy(Qt::ActionsContextMenu);


    tbResults=new QToolBar("toolbar_eval_results", d);
    rwvlayout->addWidget(tbResults);
    tbResultsSettings=new QToolBar("toolbar_eval_results_settings", d);
    rwvlayout->addWidget(tbResultsSettings);


    actRefreshResults=new QAction(QIcon(":/lib/refresh16.png"), tr("Refresh results ..."), d);
    actRefreshResults->setShortcut(tr("F5"));
    tbResults->addAction(actRefreshResults);
    tbResults->addSeparator();
    actDeleteResults=new QAction(QIcon(":/lib/delete16.png"), tr("Delete selected results"), d);
    actDeleteResults->setShortcut(QKeySequence::Delete);
    tbResults->addAction(actDeleteResults);
    tbResults->addSeparator();


    tvResults->addActionsToToolbar(tbResults);
    actCopyValErrResults=new QAction(QIcon(":/lib/copy16valerr.png"), tr("Copy Selection as value+error pairs"), this);
    tbResults->addAction(actCopyValErrResults);
    actCopyValErrResultsNoHead=new QAction(QIcon(":/lib/copy16valerr_nohead.png"), tr("Copy Selection as value+error pairs, w/o header"), d);
    tbResults->addAction(actCopyValErrResultsNoHead);

    actCopyMedianQuantilesResults=new QAction(QIcon(":/lib/copy16valerr.png"), tr("Copy Selection as median+q25+q75"), d);
    actCopyMedianQuantilesNoHead=new QAction(QIcon(":/lib/copy16valerr_nohead.png"), tr("Copy Selection as median+q25+q75, w/o header"), d);

    QMenu* menuExpanded=new QMenu(tr("copy in expanded form"), tvResults);
    menuExpanded->setIcon(QIcon(":/lib/copy16.png"));
    tvResults->addAction(menuExpanded->menuAction());
    actCopyExpanded=new QAction( tr("Copy Selection in expanded form"), d);
    menuExpanded->addAction(actCopyExpanded);
    actCopyExpandedFlipped=new QAction( tr("Copy Selection in expanded form, flipped"), d);
    menuExpanded->addAction(actCopyExpandedFlipped);
    actCopyExpandedNoHead=new QAction( tr("Copy Selection in expanded form, w/o header"), d);
    menuExpanded->addAction(actCopyExpandedNoHead);
    actCopyExpandedNoHeadFlipped=new QAction( tr("Copy Selection in expanded form, w/o header, flipped"), d);
    menuExpanded->addAction(actCopyExpandedNoHeadFlipped);
    actCopyExpandedNoHeadMatlab=new QAction( tr("Copy Selection in expanded form, to Matlab"), d);
    menuExpanded->addAction(actCopyExpandedNoHeadMatlab);
    actCopyExpandedNoHeadMatlabFlipped=new QAction( tr("Copy Selection in expanded form, to Matlab, flipped"), d);
    menuExpanded->addAction(actCopyExpandedNoHeadMatlabFlipped);

    actSaveResults=new QAction(QIcon(":/lib/save16.png"), tr("Save all results to file"), d);
    tbResults->addAction(actSaveResults);
    actSaveResultsAveraged=new QAction(tr("Save all results to file, averaged vector/matrix results"), d);

    tbResults->addSeparator();
    actStatistics=new QAction(QIcon(":/lib/result_statistics.png"), tr("histogram: column-wise"), d);
    tbResults->addAction(actStatistics);
    actStatisticsComparing=new QAction(QIcon(":/lib/result_statistics_compare.png"), tr("histogram: cell-wise"), d);
    tbResults->addAction(actStatisticsComparing);

    //tbResults->addSeparator();
    chkShowAvg=new QCheckBox(tr("show Avg+/-SD for vector/matrix resuts"), d);
    tbResultsSettings->addWidget(chkShowAvg);
    chkShowAvg->setChecked(true);
    connect(chkShowAvg, SIGNAL(toggled(bool)), this, SLOT(showAvgClicked(bool)));
    connect(chkShowAvg, SIGNAL(toggled(bool)), this, SLOT(storeSettings()));
    tbResultsSettings->addSeparator();

    chkExtractRuns=new QCheckBox(tr("show runs/indices"), d);
    chkExtractRuns->setChecked(true);
    tbResultsSettings->addWidget(chkExtractRuns);
    connect(chkExtractRuns, SIGNAL(toggled(bool)), d->resultsModel, SLOT(setExtractIndexes(bool)));
    connect(chkExtractRuns, SIGNAL(toggled(bool)), this, SLOT(storeSettings()));
    chkRemoveUnusedRuns=new QCheckBox(tr("don't show removed runs"), d);
    chkRemoveUnusedRuns->setChecked(false);
    tbResultsSettings->addWidget(chkRemoveUnusedRuns);
    connect(chkExtractRuns, SIGNAL(toggled(bool)), chkRemoveUnusedRuns, SLOT(setEnabled(bool)));
    connect(chkRemoveUnusedRuns, SIGNAL(toggled(bool)), d->resultsModel, SLOT(setRemoveUnusedIndexes(bool)));
    connect(chkRemoveUnusedRuns, SIGNAL(toggled(bool)), this, SLOT(storeSettings()));

    chkMindUnusedForVectorStat=new QCheckBox(tr("mind excluded runs for in-cell statistics"), d);
    chkMindUnusedForVectorStat->setChecked(true);
    tbResultsSettings->addWidget(chkMindUnusedForVectorStat);
    connect(chkMindUnusedForVectorStat, SIGNAL(toggled(bool)), d->resultsModel, SLOT(setMinUnusedIndexesForStatistics(bool)));
    connect(chkMindUnusedForVectorStat, SIGNAL(toggled(bool)), this, SLOT(storeSettings()));


    tbResultsSettings->addSeparator();
    tbResultsSettings->addWidget(new QLabel(" display properties: "));
    edtDisplayProperties=new QFEnhancedLineEdit(d);
    edtDisplayProperties->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtDisplayProperties, edtDisplayProperties));
    edtDisplayProperties->setToolTip(tr("put a comma ',' separated list of properties here that should be part of the results table. <br><b>Note:</b> These properties can NOT be filtered with the filters below."));
    connect(edtDisplayProperties, SIGNAL(textChanged(QString)), this, SLOT(propertiesTextChanged(QString)));
    tbResultsSettings->addWidget(edtDisplayProperties);



    tbResultsFilter=new QToolBar("toolbar_eval_results2", d);
    rwvlayout->addWidget(tbResultsFilter);
    tbResultsFilter->addWidget(new QLabel(" filter: file contains "));
    edtFilterFiles=new QFEnhancedLineEdit(d);
    edtFilterFiles->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFilterFiles, edtFilterFiles));
    edtFilterFiles->setCompleter(new QFCompleterFromFile());
    compFilterFiles=new QFCompleterFromFile(d);
    edtFilterFiles->setCompleter(compFilterFiles);
    connect(edtFilterFiles, SIGNAL(textChanged(QString)), d->resultsModel, SLOT(setFilesFilter(QString)));
    tbResultsFilter->addWidget(edtFilterFiles);
    tbResultsFilter->addWidget(new QLabel(tr("  and not contains "), d));
    edtFilterFilesNot=new QFEnhancedLineEdit(d);
    edtFilterFilesNot->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFilterFilesNot, edtFilterFilesNot));
    compFilterFilesNot=new QFCompleterFromFile(d);
    edtFilterFilesNot->setCompleter(compFilterFilesNot);
    connect(edtFilterFilesNot, SIGNAL(textChanged(QString)), d->resultsModel, SLOT(setFilesFilterNot(QString)));
    tbResultsFilter->addWidget(edtFilterFilesNot);
    chkFilterFilesRegExp=new QCheckBox(tr("RegExp"), d);
    connect(chkFilterFilesRegExp, SIGNAL(toggled(bool)), this, SLOT(storeSettings()));
    chkFilterFilesRegExp->setChecked(false);
    tbResultsFilter->addWidget(chkFilterFilesRegExp);
    connect(chkFilterFilesRegExp, SIGNAL(clicked(bool)), d->resultsModel, SLOT(setFilesFilterUsesRegExp(bool)));
    edtFilterFiles->setToolTip(tr("use this to filter the contents of the results table<br><br>"
                                       "Simply enter a filter string and the table will only display those"
                                       "rows where the raw data record name contains a match to the filter string."
                                       "Depending on whether <i>RegExp</i> is checked or not, you can use"
                                       "either simple text with wildcards '<tt>*</tt>' (match any characters) and '<tt>?</tt>'"
                                       "(match a single character), or full regular expressions in the filter string."
                                  "<br><br><font color=\"darkred\">red text</font> means you entered an invalid regular expression"));
    edtFilterFilesNot->setToolTip(tr("use this to filter the contents of the results table<br><br>"
                                       "Simply enter a filter string and the table will only display those"
                                       "rows where the raw data record name contains NO match to the filter string."
                                       "Depending on whether <i>RegExp</i> is checked or not, you can use"
                                       "either simple text with wildcards '<tt>*</tt>' (match any characters) and '<tt>?</tt>'"
                                       "(match a single character), or full regular expressions in the filter string."
                                     "<br><br><font color=\"darkred\">red text</font> means you entered an invalid regular expression"));

    tbResultsFilter->addSeparator();
    tbResultsFilter->addWidget(new QLabel(" filter: result contains: "));
    edtFilterResults=new QFEnhancedLineEdit(d);
    edtFilterResults->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFilterResults, edtFilterResults));
    edtFilterResults->setToolTip(tr("use this to filter the contents of the results table<br><br>"
                                       "Simply enter a filter string and the table will only display those"
                                       "columns where the result name contains a match to the filter string."
                                       "Depending on whether <i>RegExp</i> is checked or not, you can use"
                                       "either simple text with wildcards '<tt>*</tt>' (match any characters) and '<tt>?</tt>'"
                                       "(match a single character), or full regular expressions in the filter string."
                                       "<br><br><font color=\"darkred\">red text</font> means you entered an invalid regular expression"));
    compFilterResults=new QFCompleterFromFile(d);
    edtFilterResults->setCompleter(compFilterResults);
    connect(edtFilterResults, SIGNAL(textChanged(QString)), d->resultsModel, SLOT(setResultFilter(QString)));
    tbResultsFilter->addWidget(edtFilterResults);
    tbResultsFilter->addWidget(new QLabel(tr("  and not contains "), d));
    edtFilterResultsNot=new QFEnhancedLineEdit(d);
    edtFilterResultsNot->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFilterResultsNot, edtFilterResultsNot));
    compFilterResultsNot=new QFCompleterFromFile(d);
    edtFilterResultsNot->setCompleter(compFilterResultsNot);
    connect(edtFilterResultsNot, SIGNAL(textChanged(QString)), d->resultsModel, SLOT(setResultFilterNot(QString)));
    tbResultsFilter->addWidget(edtFilterResultsNot);
    chkFilterResultsRegExp=new QCheckBox(tr("RegExp"), d);
    chkFilterResultsRegExp->setChecked(false);
    tbResultsFilter->addWidget(chkFilterResultsRegExp);
    connect(chkFilterResultsRegExp, SIGNAL(clicked(bool)), d->resultsModel, SLOT(setResultFilterUsesRegExp(bool)));
    edtFilterResultsNot->setToolTip(tr("use this to filter the contents of the results table<br><br>"
                                       "Simply enter a filter string and the table will only display those"
                                       "columns where the result name contains NO match to the filter string."
                                       "Depending on whether <i>RegExp</i> is checked or not, you can use"
                                       "either simple text with wildcards '<tt>*</tt>' (match any characters) and '<tt>?</tt>'"
                                       "(match a single character), or full regular expressions in the filter string."
                                       "<br><br><font color=\"darkred\">red text</font> means you entered an invalid regular expression"));



    connect(edtFilterResultsNot, SIGNAL(textChanged(QString)), this, SLOT(filterResultsTextChanged(QString)));
    connect(edtFilterResults, SIGNAL(textChanged(QString)), this, SLOT(filterResultsTextChanged(QString)));
    connect(edtFilterFiles, SIGNAL(textChanged(QString)), this, SLOT(filterFilesTextChanged(QString)));
    connect(edtFilterFilesNot, SIGNAL(textChanged(QString)), this, SLOT(filterFilesTextChanged(QString)));


    rwvlayout->addWidget(tvResults);
    labAveragedresults=new QLabel(widResults);
    labAveragedresults->setTextInteractionFlags(Qt::TextSelectableByMouse);
    labAveragedresults->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    labAveragedresults->setMaximumHeight(200);
    labAveragedresults->setMaximumHeight(200);
    labAveragedresults->setSizePolicy(QSizePolicy::Ignored, labAveragedresults->sizePolicy().verticalPolicy());
    rwvlayout->addWidget(labAveragedresults);

    //connect(actCopyResults, SIGNAL(triggered()), tvResults, SLOT(copySelectionToExcel()));
    //connect(actCopyResultsNoHead, SIGNAL(triggered()), tvResults, SLOT(copySelectionToExcelNoHead()));
    connect(actCopyValErrResults, SIGNAL(triggered()), this, SLOT(copyValErrResults()));
    connect(actCopyValErrResultsNoHead, SIGNAL(triggered()), this, SLOT(copyValErrResultsNoHead()));
    connect(actCopyMedianQuantilesResults, SIGNAL(triggered()), this, SLOT(copyMedianQuantilesResults()));
    connect(actCopyMedianQuantilesNoHead, SIGNAL(triggered()), this, SLOT(copyMedianQuantilesResultsNoHead()));
    connect(actSaveResults, SIGNAL(triggered()), this, SLOT(saveResults()));
    connect(actSaveResultsAveraged, SIGNAL(triggered()), this, SLOT(saveResultsAveraged()));
    connect(actRefreshResults, SIGNAL(triggered()), this, SLOT(refreshResults()));
    connect(actDeleteResults, SIGNAL(triggered()), this, SLOT(deleteSelectedRecords()));
    connect(actStatistics, SIGNAL(triggered()), this, SLOT(showStatistics()));
    connect(actStatisticsComparing, SIGNAL(triggered()), this, SLOT(showStatisticsComparing()));


    connect(actCopyExpanded, SIGNAL(triggered()), this, SLOT(copyExpandedResults()));
    connect(actCopyExpandedNoHead, SIGNAL(triggered()), this, SLOT(copyExpandedResultsNoHead()));
    connect(actCopyExpandedNoHeadMatlab, SIGNAL(triggered()), this, SLOT(copyExpandedResultsNoHeadMatlab()));
    connect(actCopyExpandedFlipped, SIGNAL(triggered()), this, SLOT(copyExpandedResultsFlipped()));
    connect(actCopyExpandedNoHeadFlipped, SIGNAL(triggered()), this, SLOT(copyExpandedResultsNoHeadFlipped()));
    connect(actCopyExpandedNoHeadMatlabFlipped, SIGNAL(triggered()), this, SLOT(copyExpandedResultsNoHeadMatlabFlipped()));


    tabMain->addTab(widResults, tr("Evaluation &Results"));


    actHelp=new QAction(QIcon(":/lib/help.png"), tr("&Help"), d);
    actHelp->setToolTip(tr("display online-help"));
    connect(actHelp, SIGNAL(triggered()), this, SLOT(displayHelp()));

    actHelpPlugin=new QAction(QIcon(":/lib/help.png"), tr("&Plugin Help"), d);
    actHelpPlugin->setToolTip(tr("display online-help for the specific plugin"));
    connect(actHelpPlugin, SIGNAL(triggered()), this, SLOT(displayHelpPlugin()));

    actHelpPluginTutorial=new QAction(QIcon(":/lib/help/help_tutorial.png"), tr("&Plugin Tutorial"), d);
    actHelpPluginTutorial->setToolTip(tr("display the tutorial for the specific plugin"));
    connect(actHelpPluginTutorial, SIGNAL(triggered()), this, SLOT(displayHelpPluginTutorial()));

    actHelpPluginCopyright=new QAction(QIcon(":/lib/help/help_copyright.png"), tr("&Plugin Copyright"), d);
    actHelpPluginCopyright->setToolTip(tr("display copyright note for the specific plugin"));
    connect(actHelpPluginCopyright, SIGNAL(triggered()), this, SLOT(displayHelpPluginCopyright()));

    actHelpEval=new QAction(QIcon(":/lib/help_rdr.png"), tr("&Evaluation item help"), d);
    actHelpEval->setToolTip(tr("display online-help common to all plugins, i.e. for the basic evaluation editor dialog"));
    connect(actHelpEval, SIGNAL(triggered()), this, SLOT(displayHelpEval()));

    menuHelp->addAction(actHelpEval);
    menuHelp->addAction(actHelpPlugin);
    menuHelp->addAction(actHelpPluginTutorial);
    menuHelp->addAction(actHelpPluginCopyright);

    btnHelp=new QToolButton(d);
    btnHelp->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnHelp->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btnHelp->setDefaultAction(actHelp);
    tabMain->setCornerWidget(btnHelp);

    connect(tabMain, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));

    tvResults->addAction(actCopyValErrResults);
    tvResults->addAction(actCopyValErrResultsNoHead);
    tvResults->addAction(actSaveResults);
    tvResults->addAction(actSaveResultsAveraged);
    tvResults->addAction(actRefreshResults);
    tvResults->addAction(actDeleteResults);
    tvResults->addAction(actStatistics);
    tvResults->addAction(actStatisticsComparing);

    menuResults->addAction(actRefreshResults);
    menuResults->addSeparator();
    menuResults->addAction(actDeleteResults);
    menuResults->addSeparator();
    menuResults->addAction(actSaveResults);
    menuResults->addAction(actSaveResultsAveraged);
    menuResults->addSeparator();
    tvResults->addActionsToMenu(menuResults);
    menuResults->addSeparator();
    //menuResults->addAction(actCopyResults);
    //menuResults->addAction(actCopyResultsNoHead);
    menuResults->addAction(actCopyValErrResults);
    menuResults->addAction(actCopyValErrResultsNoHead);
    menuResults->addSeparator();
    menuResults->addAction(actCopyMedianQuantilesResults);
    menuResults->addAction(actCopyMedianQuantilesNoHead);
    menuResults->addSeparator();
    menuResults->addAction(actCopyExpanded);
    menuResults->addAction(actCopyExpandedFlipped);
    menuResults->addAction(actCopyExpandedNoHead);
    menuResults->addAction(actCopyExpandedNoHeadFlipped);
    menuResults->addAction(actCopyExpandedNoHeadMatlab);
    menuResults->addAction(actCopyExpandedNoHeadMatlabFlipped);
    menuResults->addSeparator();
    menuResults->addAction(actStatistics);
    menuResults->addAction(actStatisticsComparing);

    currentTabChanged(0);
}


void QFEvaluationPropertyEditorPrivate::selectionChanged(const QModelIndex& index, const QModelIndex& oldindex) {
    if (d->rdrProxy!=NULL) {
        QFRawDataRecord* rec=d->current->getProject()->getRawDataByID(d->rdrProxy->data(index, Qt::UserRole).toInt());
        if (rec!=NULL) {
            d->current->setHighlightedRecord(rec);
        } else {
            //std::cout<<"!!!   QFEvaluationPropertyEditorPrivate::selectionChanged() with NULL new record\n";
        }

        if (lstRawData->model()->rowCount()<=0) d->close();
    }
}

void QFEvaluationPropertyEditorPrivate::rdrModelReset() {
    //std::cout<<"!!!   QFEvaluationPropertyEditorPrivate::rdrModelReset() called\n";
    if (!d->current) return;
    if (!d->current->getHighlightedRecord()) return;
    if ((!d->rdrModel) || (!d->rdrProxy)) return;
    QModelIndex idx=d->rdrModel->index(d->current->getProject()->getRawDataIndex(d->current->getHighlightedRecord()));
    QModelIndex pidx=d->rdrProxy->mapFromSource(idx);
    lstRawData->selectionModel()->select(pidx, QItemSelectionModel::SelectCurrent);
}



void QFEvaluationPropertyEditorPrivate::tvResultsSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {
    QModelIndexList sel=tvResults->selectionModel()->selectedIndexes();
    QMap<int, QString> names;
    QMap<int, double> sum, sum2, count;
    for (int i=0; i<sel.size(); i++) {
        //int c=sel[i].row();
        int r=sel[i].column();
        QVariant dataS=sel[i].data(QFEvaluationResultsModel::SumRole);
        QVariant dataS2=sel[i].data(QFEvaluationResultsModel::Sum2Role);
        QVariant dataC=sel[i].data(QFEvaluationResultsModel::CountRole);
        QString name=sel[i].data(QFEvaluationResultsModel::NameRole).toString();
        double d=0;
        bool ok=false;
        if (dataS.canConvert(QVariant::Double) && dataS2.canConvert(QVariant::Double) && dataC.canConvert(QVariant::LongLong)) {
            if (names.contains(r)) {
                sum[r] = sum[r]+dataS.toDouble();
                sum2[r] = sum2[r]+dataS2.toDouble();
                count[r] = count[r]+dataC.toLongLong();
            } else {
                sum[r] = dataS.toDouble();
                sum2[r] = dataS2.toDouble();
                count[r] = dataC.toLongLong();
                names[r] = name;
            }

        }
        /*if (data.canConvert(QVariant::Double)) {
            d=data.toDouble();
            ok=true;
        }
        if (data.canConvert(QVariant::PointF)) {
            d=data.toPointF().x();
            ok=true;
        }
        if (data.type()==QVariant::String) { ok=false; }
        if (ok) {
            if (names.contains(r)) {
                sum[r] = sum[r]+d;
                sum2[r] = sum2[r]+d*d;
                count[r] = count[r]+1;
            } else {
                sum[r] = d;
                sum2[r] = d*d;
                count[r] = 1;
                names[r] = name;
            }
        }*/
    }

    int lineHeight=labAveragedresults->fontMetrics().lineSpacing();
    int maxheight=labAveragedresults->maximumHeight();
    int linespercol=maxheight/(2*lineHeight)-1;
    QStringList datalist;
    QMapIterator<int, QString> it(names);
    while (it.hasNext()) {
        it.next();
        QString name=it.value();
        int i=it.key();
        if (count[i]>0) {
            double avg=sum[i]/count[i];
            double error=sqrt(sum2[i]/count[i]-sum[i]*sum[i]/count[i]/count[i]);
            datalist.append(QString("<td align=\"right\"><b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;%1:&nbsp;</b></td><td>&nbsp;&nbsp;%2 &plusmn; %3&nbsp;</td>").arg(name).arg(roundWithError(avg, error)).arg(roundError(error)));
        }
    }
    QString results=tr("<table border=\"1\" cellspacing=\"0\"><tr>");
    int cols=(int)ceil((double)datalist.size()/(double)linespercol);
    for (int i=0; i<cols; i++) {
        results+=tr("<th align=\"right\"><b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Field:&nbsp;</b></th><th><b>&nbsp;&nbsp;Average &plusmn; StdDev&nbsp;</b></th>");
    }
    results+=tr("</th>");
    for (int r=0; r<linespercol; r++) {
        results+=tr("<tr>");
        for (int c=0; c<cols; c++) {
            int idx=c*linespercol+r;
            if ((idx>=0)&&(idx<datalist.size())) {
                results+=datalist[idx];
            } else {
                results+=tr("<td></td><td></td>");
            }
        }
        results+=tr("</tr>");
    }
    results+=tr("</table>");
    labAveragedresults->setText(results);
}



void QFEvaluationPropertyEditorPrivate::saveResults() {
    if (d->current) {

        bool exportAll=true;
        QAbstractItemModel* model=tvResults->model();
        QModelIndexList indxs=tvResults->selectionModel()->selectedIndexes();
        bool all=true;
        for (int c=0; c<model->columnCount(); c++) {
            if (!tvResults->selectionModel()->isColumnSelected(c, QModelIndex())) {
                all=false;
                break;
            }
        }
        if (all) {
            for (int r=0; r<model->rowCount(); r++) {
                if (!tvResults->selectionModel()->isRowSelected(r, QModelIndex())) {
                    all=false;
                    break;
                }
            }
        }

        if (!all && indxs.size()>0) {
            exportAll=QMessageBox::question(d, tr("Save Results ..."), tr("You selected a range of cells.\nDo you want to export this range only?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::No;

        }

        //qDebug()<<all<<exportAll<<indxs.size();

        QStringList evalIDs;
        QList<QPair<QPointer<QFRawDataRecord>, QString> > records;
        if (!exportAll) {
            for (int i=0; i<indxs.size(); i++) {
                evalIDs.append(indxs[i].data(QFEvaluationResultsModel::ResultNameRole).toString());
                QPointer<QFRawDataRecord>  pp=d->current->getProject()->getRawDataByID(indxs[i].data(QFEvaluationResultsModel::ResultIDRole).toInt());
                records.append(qMakePair(pp, indxs[i].data(QFEvaluationResultsModel::EvalNameRole).toString()));
                //qDebug()<<evalIDs.last()<<records.last().first<<records.last().second;
            }
        }


        QString evalfilter=d->current->getResultsDisplayFilter();
        QStringList f=QFDataExportHandler::getFormats();
        QString lastDir=ProgramOptions::getConfigValue("QFEvaluationPropertyEditorPrivate/lastDataDir", "").toString();
        QString selFilter=ProgramOptions::getConfigValue("QFEvaluationPropertyEditorPrivate/lastDataFilter", "").toString();
        QString fn=qfGetSaveFileName(d, tr("Save results to file ..."), lastDir, f.join(";;"), &selFilter);
        if (fn.size()>0) {
            int flip=QMessageBox::question(d, tr("Save results to file ..."), tr("Flip table?"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::No);
            if (flip==QMessageBox::Yes || flip==QMessageBox::No) {
                d->current->getProject()->rdrResultsSave(evalfilter, fn, f.indexOf(selFilter), false, flip==QMessageBox::Yes, evalIDs, records);
                ProgramOptions::setConfigValue("QFEvaluationPropertyEditorPrivate/lastDataDir", lastDir);
                ProgramOptions::setConfigValue("QFEvaluationPropertyEditorPrivate/lastDataFilter", selFilter);
            }
        }

        /*
        QString evalfilter=d->current->getResultsDisplayFilter();
        QString selectedFilter="";
        QString filter= tr("Comma Separated Values (*.csv *.dat);;Semicolon Separated Values [for german Excel] (*.dat *.txt *.csv);;SYLK dataformat (*.slk *.sylk);;SYLK dataformat, flipped (*.slk *.sylk);;Tab separated (*.dat *.txt *.tsv);;Semicolon Separated Values (*.dat *.txt *.csv)");
        QString fileName = qfGetSaveFileName(d, tr("Save Results ..."), currentSaveDir, filter, &selectedFilter);
        if ((!fileName.isEmpty())&&(!fileName.isNull())) {
            int f=filter.split(";;").indexOf(selectedFilter);
            bool ok=false;
            if (f==1) {
                ok=d->current->getProject()->rdrResultsSaveToCSV(evalfilter, fileName, false, ';', ',', '"', evalIDs, records);
            } else if (f==2) {
                ok=d->current->getProject()->rdrResultsSaveToSYLK(evalfilter, fileName, false, false, evalIDs, records);
            } else if (f==3) {
                ok=d->current->getProject()->rdrResultsSaveToSYLK(evalfilter, fileName, false, true, evalIDs, records);
            } else if (f==4) {
                ok=d->current->getProject()->rdrResultsSaveToCSV(evalfilter, fileName, false, '\t', '.', '"', evalIDs, records);
            } else if (f==4) {
                ok=d->current->getProject()->rdrResultsSaveToCSV(evalfilter, fileName, false, ';', '.', '"', evalIDs, records);
            } else {
                ok=d->current->getProject()->rdrResultsSaveToCSV(evalfilter, fileName, false, ',', '.', '"', evalIDs, records);
            }
            currentSaveDir=QFileInfo(fileName).absolutePath();
            if (!ok) QMessageBox::critical(NULL, tr("QuickFit"), tr("Could not save file '%1'.").arg(fileName));
        }*/
    }
}

void QFEvaluationPropertyEditorPrivate::saveResultsAveraged()
{
    if (d->current) {


        bool exportAll=true;
        QAbstractItemModel* model=tvResults->model();
        QModelIndexList indxs=tvResults->selectionModel()->selectedIndexes();
        bool all=true;
        for (int c=0; c<model->columnCount(); c++) {
            if (!tvResults->selectionModel()->isColumnSelected(c, QModelIndex())) {
                all=false;
                break;
            }
        }
        if (all) {
            for (int r=0; r<model->rowCount(); r++) {
                if (!tvResults->selectionModel()->isRowSelected(r, QModelIndex())) {
                    all=false;
                    break;
                }
            }
        }

        if (!all && indxs.size()>0) {
            exportAll=QMessageBox::question(d, tr("Save Results ..."), tr("You selected a range of cells.\nDo you want to export this range only?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::No;

        }

        QStringList evalIDs;
        QList<QPair<QPointer<QFRawDataRecord>, QString> > records;
        if (!exportAll) {
            for (int i=0; i<indxs.size(); i++) {
                evalIDs.append(indxs[i].data(QFEvaluationResultsModel::EvalNameRole).toString());
                QPointer<QFRawDataRecord> pp=d->current->getProject()->getRawDataByID(indxs[i].data(QFEvaluationResultsModel::ResultIDRole).toInt());
                records.append(qMakePair(pp, indxs[i].data(QFEvaluationResultsModel::ResultNameRole).toString()));
            }
        }


        QString evalfilter=d->current->getResultsDisplayFilter();
        QStringList f=QFDataExportHandler::getFormats();
        QString lastDir=ProgramOptions::getConfigValue("QFEvaluationPropertyEditorPrivate/lastDataDir", "").toString();
        QString selFilter=ProgramOptions::getConfigValue("QFEvaluationPropertyEditorPrivate/lastDataFilter", "").toString();
        QString fn=qfGetSaveFileName(d, tr("Save results to file ..."), lastDir, f.join(";;"), &selFilter);
        if (fn.size()>0) {
            int flip=QMessageBox::question(d, tr("Save results to file ..."), tr("Flip table?"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::No);
            if (flip==QMessageBox::Yes || flip==QMessageBox::No) {
                d->current->getProject()->rdrResultsSave(evalfilter, fn, f.indexOf(selFilter), true, flip==QMessageBox::Yes, evalIDs, records);
                ProgramOptions::setConfigValue("QFEvaluationPropertyEditorPrivate/lastDataDir", lastDir);
                ProgramOptions::setConfigValue("QFEvaluationPropertyEditorPrivate/lastDataFilter", selFilter);
            }
        }


        /*
        QString selectedFilter="";
        QString filter= tr("Comma Separated Values (*.csv *.dat);;Semicolon Separated Values [for german Excel] (*.dat *.txt *.csv);;SYLK dataformat (*.slk *.sylk);;SYLK dataformat, flipped (*.slk *.sylk);;Tab separated (*.dat *.txt *.tsv);;Semicolon Separated Values [for english Excel] (*.dat *.txt *.csv)");
        QString fileName = qfGetSaveFileName(d, tr("Save Results ..."), currentSaveDir, filter, &selectedFilter);
        if ((!fileName.isEmpty())&&(!fileName.isNull())) {
            int f=filter.split(";;").indexOf(selectedFilter);
            bool ok=false;
            if (f==1) {
                ok=d->current->getProject()->rdrResultsSaveToCSV(evalfilter, fileName, true, ';', ',', '"', evalIDs, records);
            } else if (f==2) {
                ok=d->current->getProject()->rdrResultsSaveToSYLK(evalfilter, fileName, true, false, evalIDs, records);
            } else if (f==3) {
                ok=d->current->getProject()->rdrResultsSaveToSYLK(evalfilter, fileName, true, true, evalIDs, records);
            } else if (f==4) {
                ok=d->current->getProject()->rdrResultsSaveToCSV(evalfilter, fileName, true, '\t', '.', '"', evalIDs, records);
            } else if (f==5) {
                ok=d->current->getProject()->rdrResultsSaveToCSV(evalfilter, fileName, true, '\t', '.', '"', evalIDs, records);
            } else {
                ok=d->current->getProject()->rdrResultsSaveToCSV(evalfilter, fileName, true, ';', '.', '"', evalIDs, records);
            }
            currentSaveDir=QFileInfo(fileName).absolutePath();
            if (!ok) QMessageBox::critical(NULL, tr("QuickFit"), tr("Could not save file '%1'.").arg(fileName));
        }*/
    }
}


void QFEvaluationPropertyEditorPrivate::removeRawData() {
    if (d->current) {
        QFRawDataRecord* r=d->current->getHighlightedRecord();
        if (r) {
            int ret = QMessageBox::question(d, tr("QuickFit %1").arg(qfInfoVersionFull()),
                                    tr("Do you really want to delete the current record?\n   '%1'").arg(r->getName()),
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::No);
            if (ret==QMessageBox::Yes) {
                d->deselectCurrent();
                d->current->getProject()->deleteRawData(r->getID());
            }
        }
    }
}

void QFEvaluationPropertyEditorPrivate::recordAboutToBeDeleted(QFRawDataRecord* record) {
    if (d->current) {
        if (d->current->isFilteredAndApplicable(record)) {
            int cnt=0;
            QList<QFRawDataRecord*> list=d->current->getProject()->getRawDataList();
            for (int i=0; i<list.size(); i++) {
                if ((record!=list[i])&&(d->current->isFilteredAndApplicable(list[i]))) {
                    cnt++;
                }
            }
            if (cnt<=0) d->close();
        }
    }
}


void QFEvaluationPropertyEditorPrivate::filterFilesTextChanged(const QString &text)
{
    bool error=false;
    if (chkFilterFilesRegExp->isChecked()) {
        QRegExp rx(text);
        error=!rx.isValid();
    }
    QWidget* s=qobject_cast<QWidget*>(sender());
    if (s) {
       QPalette p=s->palette();
       if (error) {
           p.setColor(QPalette::Text, QColor("darkred"));
       } else {
           p.setColor(QPalette::Text, edtName->palette().color(QPalette::Text));
       }
       s->setPalette(p);
    }
    storeSettings();
}

void QFEvaluationPropertyEditorPrivate::filterResultsTextChanged(const QString &text)
{
    bool error=false;
    if (chkFilterResultsRegExp->isChecked()) {
        QRegExp rx(text);
        error=!rx.isValid();
    }
    QWidget* s=qobject_cast<QWidget*>(sender());
    if (s) {
       QPalette p=s->palette();
       if (error) {
           p.setColor(QPalette::Text, QColor("darkred"));
       } else {
           p.setColor(QPalette::Text, edtName->palette().color(QPalette::Text));
       }
       s->setPalette(p);
    }
    storeSettings();
}

void QFEvaluationPropertyEditorPrivate::filterRecordsTextChanged(const QString &text)
{
    bool error=false;
    if (chkFilterRecordsRegExp->isChecked()) {
        QRegExp rx(text);
        error=!rx.isValid();
    }
    QWidget* s=qobject_cast<QWidget*>(sender());
    if (s) {
       QPalette p=s->palette();
       if (error) {
           p.setColor(QPalette::Text, QColor("darkred"));
       } else {
           p.setColor(QPalette::Text, edtName->palette().color(QPalette::Text));
       }
       s->setPalette(p);
    }
    storeSettings();
}
