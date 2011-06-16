#include "qfevaluationpropertyeditor.h"
#include "qfprojectrawdatamodel.h"


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
        disconnect(eval, SIGNAL(selectionChanged(QList<QFRawDataRecord*>)), this, SLOT(selectionChanged(QList<QFRawDataRecord*>)));
    }
    this->eval=eval;
    connect(eval, SIGNAL(selectionChanged(QList<QFRawDataRecord*>)), this, SLOT(selectionChanged(QList<QFRawDataRecord*>)));
    invalidateFilter();
}

void QFEvaluationRawDataModelProxy::selectionChanged(QList<QFRawDataRecord*> selectedRecords) {
    //std::cout<<"QFEvaluationRawDataModelProxy::selectionChanged()\n";
    invalidateFilter();
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
        rdrModel->setProject(NULL);
        resultsModel->init(NULL, "*");
        oldType=current->getType();
        //oldEditorCount=current->getEditorCount();
        disconnect(current->getProject(), SIGNAL(evaluationAboutToBeDeleted(QFEvaluationItem*)), this, SLOT(evaluationAboutToBeDeleted(QFEvaluationItem*)));
        disconnect(edtName, SIGNAL(textChanged(const QString&)), this, SLOT(nameChanged(const QString&)));
        disconnect(pteDescription, SIGNAL(textChanged()), this, SLOT(descriptionChanged()));
        disconnect(current, SIGNAL(propertiesChanged()), this, SLOT(propsChanged()));
        disconnect(lstRawData->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(selectionChanged(const QModelIndex&, const QModelIndex&)));
        disconnect(rdrProxy, SIGNAL(modelReset()), this, SLOT(rdrModelReset()));
        disconnect(current, SIGNAL(resultsChanged()), resultsModel, SLOT(resultsChanged()));
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
        resultsModel->init(current, current->getResultsDisplayFilter());
        if (current->getType()!=oldType) {
            //editorList.clear();
            //for (int i=0; i<current->getEditorCount(); i++) {
                QString n=current->getEditorName();
                //std::cout<<"creating tab '"<<n.toStdString()<<"' ... \n";
                QFEvaluationEditor* e=current->createEditor(services, this);
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
        connect(current, SIGNAL(propertiesChanged()), this, SLOT(propsChanged()));
        connect(lstRawData->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(selectionChanged(const QModelIndex&, const QModelIndex&)));
        connect(rdrProxy, SIGNAL(modelReset()), this, SLOT(rdrModelReset()));
        connect(current, SIGNAL(resultsChanged()), resultsModel, SLOT(resultsChanged()));
        connect(tvResults->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(tvResultsSelectionChanged(const QItemSelection&, const QItemSelection&)));        connect(edtName, SIGNAL(textChanged(const QString&)), this, SLOT(nameChanged(const QString&)));
        lstRawData->selectionModel()->select(rdrProxy->index(0,0), QItemSelectionModel::SelectCurrent);
        selectionChanged(rdrProxy->index(0,0), rdrProxy->index(0,0));//std::cout<<"new connected ...\n";


        helpWidget->clear();
        helpWidget->updateHelp(QString(services->getAssetsDirectory()+QString("/plugins/help/")+current->getType()+QString("/"))+current->getType()+".html");

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
        rdrModel->setProject(NULL);

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


void QFEvaluationPropertyEditor::propsChanged() {
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

    splitMain=new QSplitter(tabMain);
    lstRawData=new QListView(splitMain);
    //tabEditors=new QTabWidget(splitMain);
    layWidgets=new QHBoxLayout(this);
    QWidget* wl=new QWidget(this);
    wl->setLayout(layWidgets);
    //splitMain->addWidget(tabEditors);
    splitMain->addWidget(wl);
    splitMain->addWidget(lstRawData);

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
    tbResults->addAction(actRefreshResults);
    tbResults->addSeparator();

    actCopyResults=new QAction(QIcon(":/lib/copy16.png"), tr("Copy Selection to Clipboard (for Excel ...)"), this);
    tbResults->addAction(actCopyResults);
    actSaveResults=new QAction(QIcon(":/lib/save16.png"), tr("Save all results to file"), this);
    tbResults->addAction(actSaveResults);



    tvResults=new QEnhancedTableView(widResults);
    tvResults->setAlternatingRowColors(true);
    QFontMetrics fm(font());
    tvResults->verticalHeader()->setDefaultSectionSize((int)round((double)fm.height()*1.5));
    tvResults->setModel(resultsModel);
    rwvlayout->addWidget(tvResults);
    labAveragedresults=new QLabel(widResults);
    labAveragedresults->setTextInteractionFlags(Qt::TextSelectableByMouse);
    labAveragedresults->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    labAveragedresults->setMaximumHeight(200);
    labAveragedresults->setMaximumHeight(200);
    labAveragedresults->setSizePolicy(QSizePolicy::Ignored, labAveragedresults->sizePolicy().verticalPolicy());
    rwvlayout->addWidget(labAveragedresults);

    connect(actCopyResults, SIGNAL(triggered()), tvResults, SLOT(copySelectionToExcel()));
    connect(actSaveResults, SIGNAL(triggered()), this, SLOT(saveResults()));
    connect(actRefreshResults, SIGNAL(triggered()), resultsModel, SLOT(resultsChanged()));

    tabMain->addTab(widResults, tr("Evaluation &Results"));

    helpWidget=new QFHTMLHelpWindow(this);
    tabMain->addTab(helpWidget, QIcon(":/lib/help.png"), tr("&Online-Help"));
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
        int idx=settings->getQSettings()->value("evalpropeditor/currentTab", 0).toInt();
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


void QFEvaluationPropertyEditor::selectionChanged(const QModelIndex& index, const QModelIndex& oldindex) {
    if (rdrProxy!=NULL) {
        QFRawDataRecord* rec=current->getProject()->getRawDataByID(rdrProxy->data(index, Qt::UserRole).toInt());
        if (rec!=NULL) {
            current->setHighlightedRecord(rec);
        } else {
            //std::cout<<"!!!   QFEvaluationPropertyEditor::selectionChanged() with NULL new record\n";
        }
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
        int c=sel[i].row();
        int r=sel[i].column();
        QVariant data=sel[i].data(QFRDRResultsModel::ValueRole);
        QString name=sel[i].data(QFRDRResultsModel::NameRole).toString();
        double d=0;
        bool ok=false;
        if (data.canConvert(QVariant::Double)) {
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
        }
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
        QString filter= tr("Comma Separated Values (*.csv *.dat);;Semicolon Separated Values [for german Excel] (*.dat *.txt *.csv);;SYLK dataformat (*.slk *.sylk)");
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Results ..."), currentSaveDir, filter, &selectedFilter);
        if ((!fileName.isEmpty())&&(!fileName.isNull())) {
            int f=filter.split(";;").indexOf(selectedFilter);
            if (f==1) {
                current->getProject()->rdrResultsSaveToCSV(evalfilter, fileName, ";", ',', '"');
            } else if (f==2) {
                current->getProject()->rdrResultsSaveToSYLK(evalfilter, fileName);
            } else {
                current->getProject()->rdrResultsSaveToCSV(evalfilter, fileName, ", ", '.', '"');
            }
            currentSaveDir=QFileInfo(fileName).absolutePath();
        }
    }
}

void QFEvaluationPropertyEditor::resizeEvent(QResizeEvent* event) {
    labAveragedresults->setMaximumWidth(event->size().width());
}
