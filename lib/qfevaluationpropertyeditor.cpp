#include "qfevaluationpropertyeditor.h"
#include "qfprojectrawdatamodel.h"
#include "qfevaluationitemfactory.h"
#include "../version.h"
#include "qfproject.h"


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
        return eval->isApplicable(record);
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
    if (eval!=NULL) {
        disconnect(eval, SIGNAL(selectionChanged(QList<QPointer<QFRawDataRecord> >)), this, SLOT(selectionChanged(QList<QPointer<QFRawDataRecord> >)));
    }
    this->eval=eval;
    connect(eval, SIGNAL(selectionChanged(QList<QPointer<QFRawDataRecord> >)), this, SLOT(selectionChanged(QList<QPointer<QFRawDataRecord> >)));
    invalidateFilter();
}

void QFEvaluationRawDataModelProxy::setEditor(QFEvaluationPropertyEditor* editor) {
    this->editor=editor;
}

void QFEvaluationRawDataModelProxy::selectionChanged(QList<QPointer<QFRawDataRecord> > selectedRecords) {
    //std::cout<<"QFEvaluationRawDataModelProxy::selectionChanged()\n";
    invalidateFilter();
    //qDebug()<<rowCount()<<editor;
    if ((rowCount()<=0)&& editor) editor->close();
}










QFEvaluationPropertyEditor::QFEvaluationPropertyEditor(QFPluginServices* services, ProgramOptions* set, QFEvaluationItem* current, int id, QWidget* parent, Qt::WindowFlags f):
    QWidget(parent, f)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    //std::cout<<"creating QFEvaluationPropertyEditor ... \n";
    this->id=id;
    this->current=NULL;
    this->services=services;
    layWidgets=NULL;
    resultsModel=new QFEvaluationResultsModel(this);
    resultsModel->init(NULL, "*");

    rdrModel=new QFProjectRawDataModel(NULL);
    rdrProxy=new QFEvaluationRawDataModelProxy(rdrModel);
    rdrProxy->setSourceModel(rdrModel);
    lstRawData=NULL;
    splitMain=NULL;

    resize(400,300);
    move(5,5);

    setSettings(set);
    //std::cout<<"creating QFEvaluationPropertyEditor ... creating widgets ...\n";
    createWidgets();

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
        lstRawData->setModel(NULL);
        rdrProxy->setEvaluation(NULL);
        rdrProxy->setEditor(NULL);
        rdrModel->setProject(NULL);
        resultsModel->init(NULL, "*");
        oldType=current->getType();
        //oldEditorCount=current->getEditorCount();
        disconnect(current->getProject(), SIGNAL(recordAboutToBeDeleted(QFRawDataRecord*)), this, SLOT(recordAboutToBeDeleted(QFRawDataRecord*)));
        disconnect(current->getProject(), SIGNAL(evaluationAboutToBeDeleted(QFEvaluationItem*)), this, SLOT(evaluationAboutToBeDeleted(QFEvaluationItem*)));
        disconnect(edtName, SIGNAL(textChanged(const QString&)), this, SLOT(nameChanged(const QString&)));
        disconnect(pteDescription, SIGNAL(textChanged()), this, SLOT(descriptionChanged()));
        disconnect(current, SIGNAL(propertiesChanged(const QString&,bool)), this, SLOT(propsChanged(QString,bool)));
        disconnect(lstRawData->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(selectionChanged(const QModelIndex&, const QModelIndex&)));
        disconnect(rdrProxy, SIGNAL(modelReset()), this, SLOT(rdrModelReset()));
        disconnect(current, SIGNAL(resultsChanged(QFRawDataRecord*,QString,QString)), this, SLOT(resultsChanged(QFRawDataRecord*,QString,QString)));
        disconnect(tvResults->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(tvResultsSelectionChanged(const QItemSelection&, const QItemSelection&)));        connect(edtName, SIGNAL(textChanged(const QString&)), this, SLOT(nameChanged(const QString&)));
        if (c) {
            if (c->getType()!=oldType) {
                /*for (int i=oldEditorCount; i>=0; i--) {
                    QWidget* w=tabEditors->widget(i);
                    tabEditors->removeTab(i);
                    if (qobject_cast<QFEvaluationEditor *>(w)) qobject_cast<QFEvaluationEditor *>(w)->setSettings(NULL, id);
                    w->deleteLater();
                    //delete w;
                }*/
                if (editor) {
                    editor->close();
                    editor->setSettings(NULL, id);
                    layWidgets->removeWidget(editor);
                    editor->deleteLater();
                }
            }
        }
        //std::cout<<"old disconnected ...\n";
    }
    current=c;
    if (current) {
        //std::cout<<"connecting new ...\n";
        rdrModel->setProject(current->getProject());
        rdrProxy->setEvaluation(current);
        rdrProxy->setEditor(this);
        resultsModel->init(current, current->getResultsDisplayFilter());
        if (current->getType()!=oldType) {
            //editorList.clear();
            //for (int i=0; i<current->getEditorCount(); i++) {
                QString n=current->getEditorName();
                //std::cout<<"creating tab '"<<n.toStdString()<<"' ... \n";
                QFEvaluationEditor* e=current->createEditor(services, this, this);
                //std::cout<<"creating tab '"<<n.toStdString()<<"' ... reading settings\n";
                e->setSettings(settings, id);
                //std::cout<<"creating tab '"<<n.toStdString()<<"' ... setting current\n";
                e->setCurrent(current, id);
                //std::cout<<"creating tab '"<<n.toStdString()<<"' ... adding tab\n";
                //tabEditors->addTab(e, n);
                layWidgets->addWidget(e);
                //std::cQFEvaluationPropertyEditor::setSettings(out<<"creating tab '"<<n.toStdString()<<"' ... done\n";
                editor=e;
            //}
        } else {
            editor->setCurrent(current, id);
        }
        edtName->setText(current->getName());
        edtName->setEnabled(true);
        pteDescription->setPlainText(current->getDescription());
        pteDescription->setEnabled(true);
        labID->setText(QString::number(current->getID()));
        /*labTopIcon->setPixmap(current->getSmallIcon().pixmap(16,16));
        labTop->setText(tr("<b>%1</b>").arg(current->getName()));*/
        setWindowTitle(current->getName());
        setWindowIcon(current->getSmallIcon());

        labType->setText(current->getTypeDescription());
        labTypeIcon->setPixmap(current->getSmallIcon().pixmap(16,16));
        lstRawData->setModel(rdrProxy);
        connect(edtName, SIGNAL(textChanged(const QString&)), this, SLOT(nameChanged(const QString&)));
        connect(pteDescription, SIGNAL(textChanged()), this, SLOT(descriptionChanged()));
        connect(current->getProject(), SIGNAL(evaluationAboutToBeDeleted(QFEvaluationItem*)), this, SLOT(evaluationAboutToBeDeleted(QFEvaluationItem*)));
        connect(current->getProject(), SIGNAL(recordAboutToBeDeleted(QFRawDataRecord*)), this, SLOT(recordAboutToBeDeleted(QFRawDataRecord*)));
        connect(current, SIGNAL(propertiesChanged(QString,bool)), this, SLOT(propsChanged(QString,bool)));
        connect(lstRawData->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(selectionChanged(const QModelIndex&, const QModelIndex&)));
        connect(rdrProxy, SIGNAL(modelReset()), this, SLOT(rdrModelReset()));
        connect(current, SIGNAL(resultsChanged(QFRawDataRecord*,QString,QString)), this, SLOT(resultsChanged(QFRawDataRecord*,QString,QString)));
        connect(tvResults->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(tvResultsSelectionChanged(const QItemSelection&, const QItemSelection&)));        connect(edtName, SIGNAL(textChanged(const QString&)), this, SLOT(nameChanged(const QString&)));
        lstRawData->selectionModel()->select(rdrProxy->index(0,0), QItemSelectionModel::SelectCurrent);
        selectionChanged(rdrProxy->index(0,0), rdrProxy->index(0,0));//std::cout<<"new connected ...\n";

        QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/");
        compFilterFiles->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+current->getType()+"_evfilterfiles.txt");
        compFilterFilesNot->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+current->getType()+"_evfilterfilesnot.txt");
        compFilterResults->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+current->getType()+"_evfilterresults.txt");
        compFilterResultsNot->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/"+current->getType()+"_evfilterresults_not.txt");

        /*helpWidget->clear();
        QString dll=current->getProject()->getEvaluationItemFactory()->getPluginHelp(current->getType());
        helpWidget->updateHelp(dll);*/

    } else {
        edtName->setText("");
        edtName->setEnabled(false);
        pteDescription->setPlainText("");
        pteDescription->setEnabled(false);
        /*labTopIcon->setText("");
        labTop->setText("");*/
        labID->setText("");
        labType->setText("");
        labTypeIcon->setText("");
        lstRawData->setModel(NULL);
        rdrProxy->setEvaluation(NULL);
        rdrProxy->setEditor(NULL);
        rdrModel->setProject(NULL);
        compFilterFiles->setFilename("");
        compFilterFilesNot->setFilename("");
        compFilterResults->setFilename("");
        compFilterResultsNot->setFilename("");

    }
    checkHelpAvailable();
}

void QFEvaluationPropertyEditor::resultsChanged(QFRawDataRecord* record, const QString& evalName, const QString& resultName) {
    if (!resultsModel) return;
    //resultsModel->resultsChanged();
    if (tvResults->model()->columnCount()*tvResults->model()->rowCount()<10000) tvResults->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

void QFEvaluationPropertyEditor::refreshResults()
{
    resultsModel->resultsChanged();
    resultsChanged();
}

void QFEvaluationPropertyEditor::displayHelp() {
    if (tabMain->currentIndex()==0 || tabMain->currentIndex()==tabMain->count()-1) {
        displayHelpEval();
    } else {
        displayHelpPlugin();
    }

}

void QFEvaluationPropertyEditor::displayHelpPlugin()
{
    QString dll=current->getProject()->getEvaluationItemFactory()->getPluginHelp(current->getType());
    services->displayHelpWindow(dll);
}

void QFEvaluationPropertyEditor::displayHelpPluginTutorial()
{
    QString dll=current->getProject()->getEvaluationItemFactory()->getPluginTutorial(current->getType());
    services->displayHelpWindow(dll);
}

void QFEvaluationPropertyEditor::displayHelpPluginCopyright()
{
    QString dll=current->getProject()->getEvaluationItemFactory()->getPluginCopyrightFile(current->getType());
    services->displayHelpWindow(dll);
}

void QFEvaluationPropertyEditor::displayHelpEval()
{
    QString dll=services->getOptions()->getAssetsDirectory()+QString("/help/qf3_evalscreen.html");
    services->displayHelpWindow(dll);
}

void QFEvaluationPropertyEditor::copyValErrResults() {
    tvResults->copySelectionAsValueErrorToExcel(QFEvaluationResultsModel::AvgRole, QFEvaluationResultsModel::SDRole);
}

void QFEvaluationPropertyEditor::copyValErrResultsNoHead() {
    tvResults->copySelectionAsValueErrorToExcel(QFEvaluationResultsModel::AvgRole, QFEvaluationResultsModel::SDRole, false);
}

void QFEvaluationPropertyEditor::currentTabChanged(int tab)
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

void QFEvaluationPropertyEditor::checkHelpAvailable()
{
    if (!current) {
        actHelpPlugin->setVisible(false);
        actHelpPluginTutorial->setVisible(false);
        actHelpPluginCopyright->setVisible(false);
    } else {
        QString dll=current->getProject()->getEvaluationItemFactory()->getPluginHelp(current->getType());
        actHelpPlugin->setVisible(QFile::exists(dll));
        dll=current->getProject()->getEvaluationItemFactory()->getPluginTutorial(current->getType());
        actHelpPluginTutorial->setVisible(QFile::exists(dll));
        dll=current->getProject()->getEvaluationItemFactory()->getPluginCopyrightFile(current->getType());
        actHelpPluginCopyright->setVisible(QFile::exists(dll));
    }

}

void QFEvaluationPropertyEditor::deleteSelectedRecords() {
    if (!current) return;
    QModelIndexList sel=tvResults->selectionModel()->selectedIndexes();
    if (sel.size()>0) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::question(this, tr("QuickFit %1").arg(QF_VERSION),
                     tr("Do you really want to delete the selected results?"),
                     QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (ret == QMessageBox::Yes) {

            QSet<QFRawDataRecord*> recs;
            for (int i=0; i<sel.size(); i++) {
                QString en=sel[i].data(QFEvaluationResultsModel::EvalNameRole).toString();
                QString rn=sel[i].data(QFEvaluationResultsModel::ResultNameRole).toString();
                int rid=sel[i].data(QFEvaluationResultsModel::ResultIDRole).toInt();
                if (rid>=0) {
                    QFRawDataRecord* r=current->getProject()->getRawDataByID(rid);
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

void QFEvaluationPropertyEditor::nameChanged(const QString& text) {
    if (current) {
        current->setName(text);
    }
}

void QFEvaluationPropertyEditor::descriptionChanged() {
    if (current) {
        current->setDescription(pteDescription->toPlainText());
    }
}

void QFEvaluationPropertyEditor::evaluationAboutToBeDeleted(QFEvaluationItem* r) {
    if ((current==r) && current) {
        close();
    }
}


void QFEvaluationPropertyEditor::propsChanged(const QString& property, bool visible) {
    if (current) {
        if (current->getName()!=edtName->text()) {
            edtName->setText(current->getName());
        }
        if (current->getDescription()!=pteDescription->toPlainText()) {
            pteDescription->setPlainText(current->getDescription());
        }
        /*labTopIcon->setPixmap(current->getSmallIcon().pixmap(16,16));
        labTop->setText(tr("<b>%1</b>").arg(current->getName()));*/
        setWindowTitle(current->getName());
        setWindowIcon(current->getSmallIcon());
        labID->setText(QString::number(current->getID()));
        labType->setText(current->getTypeDescription());
        labTypeIcon->setPixmap(current->getSmallIcon().pixmap(16,16));
    }
}


void QFEvaluationPropertyEditor::createWidgets() {
    QVBoxLayout* ml=new QVBoxLayout(this);
    setLayout(ml);
    ml->setContentsMargins(2,2,2,2);
    /*QHBoxLayout* vl=new QHBoxLayout(this);
    ml->addLayout(vl);
    labTopIcon=new QLabel(this);
    vl->addWidget(labTopIcon);
    labTop=new QLabel(this);
    vl->addWidget(labTop);
    vl->addStretch();*/

    menuBar=new QMenuBar(this);
    menuBar->setVisible(true);
    ml->addWidget(menuBar);
    menuResults=menuBar->addMenu("&Results");
    menuHelp=menuBar->addMenu("&Help");

    tabMain=new QTabWidget(this);
    ml->addWidget(tabMain);

    QWidget* w=new QWidget(tabMain);
    QFormLayout* fl=new QFormLayout(w);
    w->setLayout(fl);
    tabMain->addTab(w, tr("&Properties"));
    labID=new QLabel(w);
    fl->addRow(tr("ID:"), labID);
    labType=new QLabel(w);
    labTypeIcon=new QLabel(w);
    QHBoxLayout* ptl=new QHBoxLayout(this);
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
    layWidgets=new QHBoxLayout(this);
    QWidget* wl=new QWidget(this);
    wl->setLayout(layWidgets);
    //splitMain->addWidget(tabEditors);
    splitMain->addWidget(wl);
    //splitMain->addWidget(lstRawData);
    QVBoxLayout* lstvbl=new QVBoxLayout(splitMain);
    QWidget* lstvblw=new QWidget(splitMain);
    lstvblw->setLayout(lstvbl);
    lstvbl->addWidget(lstRawData, 10);
    btnRemoveRawData=new QPushButton(QIcon(":/lib/item_delete.png"), tr("remove record"), lstvblw);
    btnRemoveRawData->setToolTip(tr("remove the current raw data record from the project"));
    connect(btnRemoveRawData, SIGNAL(clicked()), this, SLOT(removeRawData()));
    lstvbl->addWidget(btnRemoveRawData);

    splitMain->addWidget(lstvblw);

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

    widResults=new QWidget(this);
    QVBoxLayout* rwvlayout=new QVBoxLayout(this);
    widResults->setLayout(rwvlayout);
    tbResults=new QToolBar("toolbar_eval_results", this);
    rwvlayout->addWidget(tbResults);
    actRefreshResults=new QAction(QIcon(":/lib/refresh16.png"), tr("Refresh results ..."), this);
    actRefreshResults->setShortcut(tr("F5"));
    tbResults->addAction(actRefreshResults);
    tbResults->addSeparator();
    actDeleteResults=new QAction(QIcon(":/lib/delete16.png"), tr("Delete selected results"), this);
    actDeleteResults->setShortcut(QKeySequence::Delete);
    tbResults->addAction(actDeleteResults);
    tbResults->addSeparator();

    actCopyResults=new QAction(QIcon(":/lib/copy16.png"), tr("Copy Selection to clipboard (for Excel ...)"), this);
    tbResults->addAction(actCopyResults);
    actCopyResultsNoHead=new QAction(QIcon(":/lib/copy16_nohead.png"), tr("Copy Selection to clipboard (for Excel ...) without herader rows/columns"), this);
    tbResults->addAction(actCopyResultsNoHead);
    actCopyValErrResults=new QAction(QIcon(":/lib/copy16valerr.png"), tr("Copy Selection to clipboard (for Excel ...) as value+error pairs"), this);
    tbResults->addAction(actCopyValErrResults);
    actCopyValErrResultsNoHead=new QAction(QIcon(":/lib/copy16valerr_nohead.png"), tr("Copy Selection to clipboard (for Excel ...) as value+error pairs, but without a header (so data only)"), this);
    tbResults->addAction(actCopyValErrResultsNoHead);
    actSaveResults=new QAction(QIcon(":/lib/save16.png"), tr("Save all results to file"), this);
    tbResults->addAction(actSaveResults);
    actSaveResultsAveraged=new QAction(tr("Save all results to file, averaged vector/matrix results"), this);

    tbResults->addSeparator();
    tbResults->addWidget(new QLabel(" filter: file contains "));
    edtFilterFiles=new QFEnhancedLineEdit(this);
    edtFilterFiles->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFilterFiles, edtFilterFiles));
    edtFilterFiles->setCompleter(new QFCompleterFromFile());
    compFilterFiles=new QFCompleterFromFile(this);
    edtFilterFiles->setCompleter(compFilterFiles);
    connect(edtFilterFiles, SIGNAL(textChanged(QString)), resultsModel, SLOT(setFilesFilter(QString)));
    tbResults->addWidget(edtFilterFiles);
    tbResults->addWidget(new QLabel(tr("  and not contains "), this));
    edtFilterFilesNot=new QFEnhancedLineEdit(this);
    edtFilterFilesNot->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFilterFilesNot, edtFilterFilesNot));
    compFilterFilesNot=new QFCompleterFromFile(this);
    edtFilterFilesNot->setCompleter(compFilterFilesNot);
    connect(edtFilterFilesNot, SIGNAL(textChanged(QString)), resultsModel, SLOT(setFilesFilterNot(QString)));
    tbResults->addWidget(edtFilterFilesNot);
    chkFilterFilesRegExp=new QCheckBox(tr("RegExp"), this);
    chkFilterFilesRegExp->setChecked(false);
    tbResults->addWidget(chkFilterFilesRegExp);
    connect(chkFilterFilesRegExp, SIGNAL(clicked(bool)), resultsModel, SLOT(setFilesFilterUsesRegExp(bool)));
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

    tbResults->addSeparator();
    tbResults->addWidget(new QLabel(" filter: result contains: "));
    edtFilterResults=new QFEnhancedLineEdit(this);
    edtFilterResults->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFilterResults, edtFilterResults));
    edtFilterResults->setToolTip(tr("use this to filter the contents of the results table<br><br>"
                                       "Simply enter a filter string and the table will only display those"
                                       "columns where the result name contains a match to the filter string."
                                       "Depending on whether <i>RegExp</i> is checked or not, you can use"
                                       "either simple text with wildcards '<tt>*</tt>' (match any characters) and '<tt>?</tt>'"
                                       "(match a single character), or full regular expressions in the filter string."
                                       "<br><br><font color=\"darkred\">red text</font> means you entered an invalid regular expression"));
    compFilterResults=new QFCompleterFromFile(this);
    edtFilterResults->setCompleter(compFilterResults);
    connect(edtFilterResults, SIGNAL(textChanged(QString)), resultsModel, SLOT(setResultFilter(QString)));
    tbResults->addWidget(edtFilterResults);
    tbResults->addWidget(new QLabel(tr("  and not contains "), this));
    edtFilterResultsNot=new QFEnhancedLineEdit(this);
    edtFilterResultsNot->addButton(new QFStyledButton(QFStyledButton::ClearLineEdit, edtFilterResultsNot, edtFilterResultsNot));
    compFilterResultsNot=new QFCompleterFromFile(this);
    edtFilterResultsNot->setCompleter(compFilterResultsNot);
    connect(edtFilterResultsNot, SIGNAL(textChanged(QString)), resultsModel, SLOT(setResultFilterNot(QString)));
    tbResults->addWidget(edtFilterResultsNot);
    chkFilterResultsRegExp=new QCheckBox(tr("RegExp"), this);
    chkFilterResultsRegExp->setChecked(false);
    tbResults->addWidget(chkFilterResultsRegExp);
    connect(chkFilterResultsRegExp, SIGNAL(clicked(bool)), resultsModel, SLOT(setResultFilterUsesRegExp(bool)));
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

    tvResults=new QEnhancedTableView(widResults);
    tvResults->setAlternatingRowColors(true);
    tvResults->setItemDelegate(new QFHTMLDelegate(tvResults));
    QFontMetrics fm(font());
    tvResults->verticalHeader()->setDefaultSectionSize((int)round((double)fm.height()*1.5));
    tvResults->setModel(resultsModel);
    tvResults->setContextMenuPolicy(Qt::ActionsContextMenu);
    tvResults->addAction(actCopyResults);
    tvResults->addAction(actCopyResultsNoHead);
    tvResults->addAction(actCopyValErrResults);
    tvResults->addAction(actCopyValErrResultsNoHead);
    tvResults->addAction(actSaveResults);
    tvResults->addAction(actSaveResultsAveraged);
    tvResults->addAction(actRefreshResults);
    tvResults->addAction(actDeleteResults);
    rwvlayout->addWidget(tvResults);
    labAveragedresults=new QLabel(widResults);
    labAveragedresults->setTextInteractionFlags(Qt::TextSelectableByMouse);
    labAveragedresults->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    labAveragedresults->setMaximumHeight(200);
    labAveragedresults->setMaximumHeight(200);
    labAveragedresults->setSizePolicy(QSizePolicy::Ignored, labAveragedresults->sizePolicy().verticalPolicy());
    rwvlayout->addWidget(labAveragedresults);

    connect(actCopyResults, SIGNAL(triggered()), tvResults, SLOT(copySelectionToExcel()));
    connect(actCopyResultsNoHead, SIGNAL(triggered()), tvResults, SLOT(copySelectionToExcelNoHead()));
    connect(actCopyValErrResults, SIGNAL(triggered()), this, SLOT(copyValErrResults()));
    connect(actCopyValErrResultsNoHead, SIGNAL(triggered()), this, SLOT(copyValErrResultsNoHead()));
    connect(actSaveResults, SIGNAL(triggered()), this, SLOT(saveResults()));
    connect(actSaveResultsAveraged, SIGNAL(triggered()), this, SLOT(saveResultsAveraged()));
    connect(actRefreshResults, SIGNAL(triggered()), this, SLOT(refreshResults()));
    connect(actDeleteResults, SIGNAL(triggered()), this, SLOT(deleteSelectedRecords()));

    tabMain->addTab(widResults, tr("Evaluation &Results"));


    actHelp=new QAction(QIcon(":/lib/help.png"), tr("&Help"), this);
    actHelp->setToolTip(tr("display online-help"));
    connect(actHelp, SIGNAL(triggered()), this, SLOT(displayHelp()));

    actHelpPlugin=new QAction(QIcon(":/lib/help.png"), tr("&Plugin Help"), this);
    actHelpPlugin->setToolTip(tr("display online-help for the specific plugin"));
    connect(actHelpPlugin, SIGNAL(triggered()), this, SLOT(displayHelpPlugin()));

    actHelpPluginTutorial=new QAction(QIcon(":/lib/help/help_tutorial.png"), tr("&Plugin Tutorial"), this);
    actHelpPluginTutorial->setToolTip(tr("display the tutorial for the specific plugin"));
    connect(actHelpPluginTutorial, SIGNAL(triggered()), this, SLOT(displayHelpPluginTutorial()));

    actHelpPluginCopyright=new QAction(QIcon(":/lib/help/help_copyright.png"), tr("&Plugin Copyright"), this);
    actHelpPluginCopyright->setToolTip(tr("display copyright note for the specific plugin"));
    connect(actHelpPluginCopyright, SIGNAL(triggered()), this, SLOT(displayHelpPluginCopyright()));

    actHelpEval=new QAction(QIcon(":/lib/help_rdr.png"), tr("&Evaluation item help"), this);
    actHelpEval->setToolTip(tr("display online-help common to all plugins, i.e. for the basic evaluation editor dialog"));
    connect(actHelpEval, SIGNAL(triggered()), this, SLOT(displayHelpEval()));

    menuHelp->addAction(actHelpEval);
    menuHelp->addAction(actHelpPlugin);
    menuHelp->addAction(actHelpPluginTutorial);
    menuHelp->addAction(actHelpPluginCopyright);

    btnHelp=new QToolButton(this);
    btnHelp->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btnHelp->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btnHelp->setDefaultAction(actHelp);
    tabMain->setCornerWidget(btnHelp);

    connect(tabMain, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));

    menuResults->addAction(actRefreshResults);
    menuResults->addSeparator();
    menuResults->addAction(actDeleteResults);
    menuResults->addSeparator();
    menuResults->addAction(actSaveResults);
    menuResults->addAction(actSaveResultsAveraged);
    menuResults->addSeparator();
    menuResults->addAction(actCopyResults);
    menuResults->addAction(actCopyResultsNoHead);
    menuResults->addAction(actCopyValErrResults);
    menuResults->addAction(actCopyValErrResultsNoHead);

    currentTabChanged(0);
}

void QFEvaluationPropertyEditor::setSettings(ProgramOptions* settings) {
    this->settings=settings;
    //std::cout<<"QFEvaluationPropertyEditor::setSettings("<<settings<<")\n";
    if (current && layWidgets) {
        if (editor) {
            editor->setSettings(settings, id);
        }
    }
    readSettings();
}


void QFEvaluationPropertyEditor::readSettings() {
    if (!settings) return;
    settings->getQSettings()->sync();
    if (tabMain) {
        //int idx=settings->getQSettings()->value("evalpropeditor/currentTab", 0).toInt();
    }
    loadWidgetGeometry(*(settings->getQSettings()), this, QPoint(10, 10), QSize(800, 600), "evalpropeditor/");
    if (splitMain) loadSplitter(*(settings->getQSettings()), splitMain, "evalpropeditor/");
    if (editor) editor->readSettings();
    currentSaveDir=settings->getQSettings()->value("evalpropeditor/lastSaveDir", currentSaveDir).toString();
}

void QFEvaluationPropertyEditor::writeSettings() {
    if (!settings) return;
    saveWidgetGeometry(*(settings->getQSettings()), this, "evalpropeditor/");
    saveSplitter(*(settings->getQSettings()), splitMain, "evalpropeditor/");
    if (tabMain) {
        settings->getQSettings()->setValue("evalpropeditor/currentTab", tabMain->currentIndex());
    }

    if (editor) editor->writeSettings();
    settings->getQSettings()->setValue("evalpropeditor/lastSaveDir", currentSaveDir);
}

void QFEvaluationPropertyEditor::deselectCurrent() {
    if (lstRawData && lstRawData->model()) {
        QModelIndex current=lstRawData->selectionModel()->currentIndex();
        int rows=lstRawData->model()->rowCount();

        QModelIndex next;

        //qDebug()<<current<<current.row()<<rows;

        if (current.row()>0) {
            next=lstRawData->model()->index(current.row()-1, current.column());

        } else {
            if (rows>1) next=lstRawData->model()->index(rows-1, current.column());
        }
        //qDebug()<<next;
        //lstRawData->selectionModel()->select(next, QItemSelectionModel::SelectCurrent);
        if (next.isValid()) {
            lstRawData->selectionModel()->setCurrentIndex(next, QItemSelectionModel::SelectCurrent);
        } else {
            close();
        }
    }
}

QMenuBar *QFEvaluationPropertyEditor::getMenuBar() const
{
    return menuBar;
}

void QFEvaluationPropertyEditor::setMenuBarVisible(bool visible)
{
    menuBar->setVisible(visible);
}

QMenu *QFEvaluationPropertyEditor::addMenu(const QString &title, int editor)
{
    menuBar->removeAction(menuHelp->menuAction());
    QMenu* m=menuBar->addMenu(title);
    menuBar->addAction(menuHelp->menuAction());
    menus.append(qMakePair(editor, m));
    currentTabChanged(tabMain->currentIndex());
    return m;
}

void QFEvaluationPropertyEditor::registerMenu(QMenu *menu, int editor)
{
    menus.append(qMakePair(editor, menu));
    currentTabChanged(tabMain->currentIndex());
}

QMenu *QFEvaluationPropertyEditor::getHelpMenu() const
{
    return menuHelp;
}

void QFEvaluationPropertyEditor::selectionChanged(const QModelIndex& index, const QModelIndex& oldindex) {
    if (rdrProxy!=NULL) {
        QFRawDataRecord* rec=current->getProject()->getRawDataByID(rdrProxy->data(index, Qt::UserRole).toInt());
        if (rec!=NULL) {
            current->setHighlightedRecord(rec);
        } else {
            //std::cout<<"!!!   QFEvaluationPropertyEditor::selectionChanged() with NULL new record\n";
        }

        if (lstRawData->model()->rowCount()<=0) close();
    }
}

void QFEvaluationPropertyEditor::rdrModelReset() {
    //std::cout<<"!!!   QFEvaluationPropertyEditor::rdrModelReset() called\n";
    if (!current) return;
    if (!current->getHighlightedRecord()) return;
    if ((!rdrModel) || (!rdrProxy)) return;
    QModelIndex idx=rdrModel->index(current->getProject()->getRawDataIndex(current->getHighlightedRecord()));
    QModelIndex pidx=rdrProxy->mapFromSource(idx);
    lstRawData->selectionModel()->select(pidx, QItemSelectionModel::SelectCurrent);
}



void QFEvaluationPropertyEditor::tvResultsSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {
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



void QFEvaluationPropertyEditor::saveResults() {
    if (current) {
        QString evalfilter=current->getResultsDisplayFilter();
        QString selectedFilter="";
        QString filter= tr("Comma Separated Values (*.csv *.dat);;Semicolon Separated Values [for german Excel] (*.dat *.txt *.csv);;SYLK dataformat (*.slk *.sylk);;SYLK dataformat, flipped (*.slk *.sylk);;Tab separated (*.dat *.txt *.tsv)");
        QString fileName = qfGetSaveFileName(this, tr("Save Results ..."), currentSaveDir, filter, &selectedFilter);
        if ((!fileName.isEmpty())&&(!fileName.isNull())) {
            int f=filter.split(";;").indexOf(selectedFilter);
            bool ok=false;
            if (f==1) {
                ok=current->getProject()->rdrResultsSaveToCSV(evalfilter, fileName, false, ';', ',', '"');
            } else if (f==2) {
                ok=current->getProject()->rdrResultsSaveToSYLK(evalfilter, fileName, false);
            } else if (f==3) {
                ok=current->getProject()->rdrResultsSaveToSYLK(evalfilter, fileName, false, true);
            } else if (f==4) {
                ok=current->getProject()->rdrResultsSaveToCSV(evalfilter, fileName, false, '\t', '.', '"');
            } else {
                ok=current->getProject()->rdrResultsSaveToCSV(evalfilter, fileName, false, ',', '.', '"');
            }
            currentSaveDir=QFileInfo(fileName).absolutePath();
            if (!ok) QMessageBox::critical(NULL, tr("QuickFit"), tr("Could not save file '%1'.").arg(fileName));
        }
    }
}

void QFEvaluationPropertyEditor::saveResultsAveraged()
{
    if (current) {
        QString evalfilter=current->getResultsDisplayFilter();
        QString selectedFilter="";
        QString filter= tr("Comma Separated Values (*.csv *.dat);;Semicolon Separated Values [for german Excel] (*.dat *.txt *.csv);;SYLK dataformat (*.slk *.sylk);;SYLK dataformat, flipped (*.slk *.sylk);;Tab separated (*.dat *.txt *.tsv)");
        QString fileName = qfGetSaveFileName(this, tr("Save Results ..."), currentSaveDir, filter, &selectedFilter);
        if ((!fileName.isEmpty())&&(!fileName.isNull())) {
            int f=filter.split(";;").indexOf(selectedFilter);
            bool ok=false;
            if (f==1) {
                ok=current->getProject()->rdrResultsSaveToCSV(evalfilter, fileName, true, ';', ',', '"');
            } else if (f==2) {
                ok=current->getProject()->rdrResultsSaveToSYLK(evalfilter, fileName, true);
            } else if (f==3) {
                ok=current->getProject()->rdrResultsSaveToSYLK(evalfilter, fileName, true, true);
            } else if (f==4) {
                ok=current->getProject()->rdrResultsSaveToCSV(evalfilter, fileName, true, '\t', '.', '"');
            } else {
                ok=current->getProject()->rdrResultsSaveToCSV(evalfilter, fileName, true, ',', '.', '"');
            }
            currentSaveDir=QFileInfo(fileName).absolutePath();
            if (!ok) QMessageBox::critical(NULL, tr("QuickFit"), tr("Could not save file '%1'.").arg(fileName));
        }
    }
}

void QFEvaluationPropertyEditor::resizeEvent(QResizeEvent* event) {
    labAveragedresults->setMaximumWidth(event->size().width());
}

void QFEvaluationPropertyEditor::removeRawData() {
    if (current) {
        QFRawDataRecord* r=current->getHighlightedRecord();
        if (r) {
            int ret = QMessageBox::question(this, tr("QuickFit %1").arg(VERSION_FULL),
                                    tr("Do you really want to delete the current record?\n   '%1'").arg(r->getName()),
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::No);
            if (ret==QMessageBox::Yes) {
                deselectCurrent();
                current->getProject()->deleteRawData(r->getID());
            }
        }
    }
}

void QFEvaluationPropertyEditor::recordAboutToBeDeleted(QFRawDataRecord* record) {
    if (current) {
        if (current->isApplicable(record)) {
            int cnt=0;
            QList<QFRawDataRecord*> list=current->getProject()->getRawDataList();
            for (int i=0; i<list.size(); i++) {
                if ((record!=list[i])&&(current->isApplicable(list[i]))) {
                    cnt++;
                }
            }
            if (cnt<=0) close();
        }
    }
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

void QFEvaluationPropertyEditor::filterFilesTextChanged(const QString &text)
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
}

void QFEvaluationPropertyEditor::filterResultsTextChanged(const QString &text)
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
}
