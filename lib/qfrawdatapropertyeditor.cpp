#include "qfrawdatapropertyeditor.h"
#include "dlgnewproperty.h"
#include "qfrawdatarecordfactory.h"
#include "../version.h"
#include "qfhtmldelegate.h"
#include "qfstyleditemdelegate.h"


// TODO: add some more options to the fit results display: store column under different name


QFRawDataPropertyEditor::QFRawDataPropertyEditor(QFPluginServices* services, ProgramOptions* set, QFRawDataRecord* current, int id, QWidget* parent, Qt::WindowFlags f):
    QWidget(parent, f)
{
    setAttribute(Qt::WA_DeleteOnClose, true);

    currentSaveDir="";
    //std::cout<<"creating QFRawDataPropertyEditor ...\n";
    this->current=NULL;
    this->id=id;
    this->services=services;
    setSettings(set);
    //std::cout<<"creating QFRawDataPropertyEditor ... creating widgets ...\n";
    createWidgets();
    //std::cout<<"creating QFRawDataPropertyEditor ... setting current ...\n";
    setCurrent(current);
    //std::cout<<"creating QFRawDataPropertyEditor ... DONE!\n";
    readSettings();
}

QFRawDataPropertyEditor::~QFRawDataPropertyEditor()
{
    writeSettings();
}

#define ADDTOGRID(layout, counter, label, widget) {\
    QWidget* lparent=NULL; \
    QWidget* bwidget=NULL; \
      lparent=widget; \
      bwidget=widget; \
      layout->addWidget(widget, counter, 1); \
    QLabel* l=new QLabel(label, lparent); \
    l->setBuddy(bwidget); \
    layout->addWidget(l, counter, 0); \
    counter++; \
}

#define ADDTOGRIDLAY(layout, counter, label, widget) {\
    QWidget* lparent=NULL; \
    QWidget* bwidget=NULL; \
    bwidget=new QWidget(widget->parentWidget()); \
    bwidget->setLayout(widget); \
    lparent=bwidget; \
    layout->addWidget(bwidget, counter, 1); \
    QLabel* l=new QLabel(label, lparent); \
    l->setBuddy(bwidget); \
    layout->addWidget(l, counter, 0); \
    counter++; \
}

void QFRawDataPropertyEditor::createWidgets() {
    QLabel* l;
    QVBoxLayout* ml=new QVBoxLayout(this);
    setLayout(ml);
    ml->setContentsMargins(2,2,2,2);
    QHBoxLayout* vl=new QHBoxLayout(this);
    ml->addLayout(vl);
    btnPrevious=new QPushButton(QIcon(":/lib/prop_previous.png"), tr("&previous"), this);
    btnPrevious->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btnPrevious->setToolTip(tr("move to previous record"));
    vl->addWidget(btnPrevious);
    connect(btnPrevious, SIGNAL(clicked()), this, SLOT(previousPressed()));
    btnNext=new QPushButton(QIcon(":/lib/prop_next.png"), tr("&next"), this);
    btnNext->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btnNext->setToolTip(tr("move to next record"));
    connect(btnNext, SIGNAL(clicked()), this, SLOT(nextPressed()));
    vl->addWidget(btnNext);
    labTopIcon=new QLabel(this);
    vl->addWidget(labTopIcon);
    labTop=new QLabel(this);
    vl->addWidget(labTop);
    vl->addStretch();

    btnDeleteReord=new QPushButton(QIcon(":/lib/item_delete.png"), tr("&Remove Record"), this);
    btnDeleteReord->setToolTip(tr("removes the currently displayed record from the project"));
    btnDeleteReord->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    vl->addWidget(btnDeleteReord);
    connect(btnDeleteReord, SIGNAL(clicked()), this, SLOT(deleteRecord()));
    btnHelp=new QPushButton(QIcon(":/lib/help.png"), tr("&Help"), this);
    btnHelp->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btnHelp->setToolTip(tr("display online-help"));
    vl->addWidget(btnHelp);
    connect(btnHelp, SIGNAL(clicked()), this, SLOT(displayHelp()));

    tabMain=new QTabWidget(this);
    ml->addWidget(tabMain);

    QWidget* w=new QWidget(tabMain);
    QGridLayout* fl=new QGridLayout(w);
    //int flcounter=0;
    //fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    w->setLayout(fl);
    tabMain->addTab(w, tr("&Properties"));
    labID=new QLabel(w);
    labID->setSizePolicy(labID->sizePolicy().verticalPolicy(),QSizePolicy::Fixed);
    //fl->addRow(tr("ID:"), labID);
    fl->addWidget((l=new QLabel(tr("ID:"), w)), 0, 0);
    l->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    fl->addWidget(labID, 0, 1);
    fl->setRowStretch(0,1);
    labType=new QLabel(w);
    labTypeIcon=new QLabel(w);
    labType->setSizePolicy(labType->sizePolicy().verticalPolicy(),QSizePolicy::Fixed);
    labTypeIcon->setSizePolicy(labTypeIcon->sizePolicy().verticalPolicy(),QSizePolicy::Fixed);
    QHBoxLayout* ptl=new QHBoxLayout(this);
    ptl->setContentsMargins(0,0,0,0);
    ptl->addWidget(labTypeIcon);
    ptl->addWidget(labType);
    ptl->addStretch();
    //fl->addRow(tr("Type:"), ptl);
    fl->addWidget((l=new QLabel(tr("Type:"), w)), 1, 0);
    fl->setRowStretch(1,1);
    l->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    fl->addLayout(ptl, 1, 1);
    edtName=new QLineEdit(w);
    //fl->addRow(tr("&Name:"), edtName);
    fl->addWidget((l=new QLabel(tr("&Name:"), w)), 2, 0);
    l->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    l->setBuddy(edtName);
    fl->addWidget(edtName, 2, 1);
    fl->setRowStretch(2,1);

    edtFolder=new QLineEdit(w);
    fl->addWidget((l=new QLabel(tr("&Folder:"), w)), 3, 0);
    l->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    l->setBuddy(edtFolder);
    fl->addWidget(edtFolder, 3, 1);
    fl->setRowStretch(3,1);

    pteDescription=new QPlainTextEdit(w);
    pteDescription->setSizePolicy( pteDescription->sizePolicy().horizontalPolicy(),QSizePolicy::Preferred);

    //fl->addRow(tr("&Description:"), pteDescription);
    fl->addWidget((l=new QLabel(tr("&Description:"), w)), 4, 0);
    l->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    l->setBuddy(pteDescription);
    fl->addWidget(pteDescription, 4, 1);
    fl->setRowStretch(4,1);
    //fl->setRowStretch(flcounter-1, 2);
    lstFiles=new QListWidget(w);
    lstFiles->setSizePolicy(lstFiles->sizePolicy().verticalPolicy(), QSizePolicy::Preferred);
    //fl->addRow(tr("&Files:"), lstFiles);
    fl->addWidget((l=new QLabel(tr("&Files:"), w)), 5, 0);
    l->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    l->setBuddy(lstFiles);
    fl->addWidget(lstFiles, 5, 1);
    fl->setRowStretch(5,1);
    //fl->setRowStretch(flcounter-1, 1);
    tvProperties=new QEnhancedTableView(w);
    QFontMetrics fm(font());
    tvProperties->verticalHeader()->setDefaultSectionSize((int)round((double)fm.height()*1.1));
    tvProperties->horizontalHeader()->setStretchLastSection(true);
    //tvProperties->setSizePolicy(tvProperties->sizePolicy().horizontalPolicy(), QSizePolicy::Expanding);
    tvProperties->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tvProperties->setItemDelegate(new QFItemDelegate(tvProperties));
    paramFilterProxy=new QSortFilterProxyModel(this);
    paramFilterProxy->setDynamicSortFilter(false);
    paramFilterProxy->setFilterKeyColumn(0);
    paramFilterProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    paramFilterProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    tvProperties->setModel(paramFilterProxy);

    QWidget* widProperties=new QWidget(this);
    QHBoxLayout* pl1=new QHBoxLayout(this);
    widProperties->setLayout(pl1);
    widProperties->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pl1->setContentsMargins(0,0,0,0);
    pl1->addWidget(tvProperties);
    QVBoxLayout* pl2=new QVBoxLayout(this);
    pl1->addLayout(pl2);
    btnNewProperty=new QPushButton(QIcon(":/lib/prop_add.png"), tr("In&sert Property"), this);
    connect(btnNewProperty, SIGNAL(clicked()), this, SLOT(newPropClicked()));
    pl2->addWidget(btnNewProperty);
    btnDeleteProperty=new QPushButton(QIcon(":/lib/prop_delete.png"), tr("&Delete Property"), this);
    connect(btnDeleteProperty, SIGNAL(clicked()), this, SLOT(deletePropClicked()));
    pl2->addWidget(btnDeleteProperty);
    pl2->addStretch();
    //fl->addRow(tr("Properties:"), widProperties);
    fl->addWidget((l=new QLabel(tr("Properties:"), w)), 6, 0);
    l->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    fl->addWidget(widProperties, 6, 1);
    fl->setRowStretch(6,3);
    //fl->setRowStretch(flcounter-1, 5);

    widResults=new QWidget(this);
    QVBoxLayout* rwvlayout=new QVBoxLayout(this);
    widResults->setLayout(rwvlayout);
    tbResults=new QToolBar("toolbar_rdr_results", this);
    rwvlayout->addWidget(tbResults);
    actCopyResults=new QAction(QIcon(":/lib/copy16.png"), tr("Copy Selection to Clipboard (for Excel ...)"), this);
    tbResults->addAction(actCopyResults);
    actSaveResults=new QAction(QIcon(":/lib/save16.png"), tr("Save all results to file"), this);
    tbResults->addAction(actSaveResults);

    tbResults->addSeparator();
    actDeleteResults=new QAction(QIcon(":/lib/delete16.png"), tr("Delete Selection"), this);
    tbResults->addAction(actDeleteResults);


    tvResults=new QEnhancedTableView(widResults);
    tvResults->setAlternatingRowColors(true);
    tvResults->verticalHeader()->setDefaultSectionSize((int)round((double)fm.height()*1.5));
    tvResults->setItemDelegate(new QFHTMLDelegate(tvResults));
    rwvlayout->addWidget(tvResults);
    labAveragedresults=new QLabel(widResults);
    labAveragedresults->setTextInteractionFlags(Qt::TextSelectableByMouse);
    labAveragedresults->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    labAveragedresults->setMaximumHeight(200);
    labAveragedresults->setSizePolicy(QSizePolicy::Ignored, labAveragedresults->sizePolicy().verticalPolicy());
    rwvlayout->addWidget(labAveragedresults);

    connect(actCopyResults, SIGNAL(triggered()), tvResults, SLOT(copySelectionToExcel()));
    connect(actSaveResults, SIGNAL(triggered()), this, SLOT(saveResults()));
    connect(actDeleteResults, SIGNAL(triggered()), this, SLOT(deleteSelectedResults()));

    tabMain->addTab(widResults, tr("Evaluation &Results"));



}

void QFRawDataPropertyEditor::closeEvent ( QCloseEvent * event ) {
    writeSettings();
    event->accept();
}

void QFRawDataPropertyEditor::resizePropertiesTable() {
    if (!current || !tvProperties) return;
    //std::cout<<"starting resizePropertiesTable() ...";
    tvProperties->resizeColumnsToContents();
    /*for (int i=0; i<current->getPropertyModel()->rowCount(); i++) {
        tvProperties->setRowHeight(i, tvProperties->fontMetrics().height());
    }*/
    //std::cout<<" DONE!\n";
}

void QFRawDataPropertyEditor::setCurrent(QFRawDataRecord* c) {
    QString oldType="";
    //std::cout<<"deleting old editors ... \n";
    int oldEditorCount=0;
    if (current) {
        oldType=current->getType();
        oldEditorCount=current->getEditorCount();
        disconnect(current->getProject(), SIGNAL(recordAboutToBeDeleted(QFRawDataRecord*)), this, SLOT(recordAboutToBeDeleted(QFRawDataRecord*)));
        disconnect(edtName, SIGNAL(textChanged(const QString&)), this, SLOT(nameChanged(const QString&)));
        disconnect(edtFolder, SIGNAL(textChanged(const QString&)), this, SLOT(folderChanged(const QString&)));
        disconnect(pteDescription, SIGNAL(textChanged()), this, SLOT(descriptionChanged()));
        disconnect(current, SIGNAL(propertiesChanged()), this, SLOT(propsChanged()));
        disconnect(current->getPropertyModel(), SIGNAL(modelReset()), this, SLOT(resizePropertiesTable()));
        disconnect(current->resultsGetModel(), SIGNAL(modelReset()), tvResults, SLOT(resizeColumnsToContents()));
        disconnect(tvResults->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(tvResultsSelectionChanged(const QItemSelection&, const QItemSelection&)));
        if (c) {
            if (c->getType()!=oldType) {
                for (int i=editorList.size()-1; i>=0; i--) {
                    QFRawDataEditor* w=editorList[i];
                    if (w) {
                        tabMain->removeTab(tabMain->indexOf(editorList[i]));
                        QApplication::processEvents(QEventLoop::AllEvents, 50);
                        w->saveSettings();
                        w->setSettings(NULL, id);
                        w->setCurrent(NULL, id);
                        w->setParent(NULL);
                        w->deleteLater();
                        //delete w;
                    }
                }
                editorList.clear();
                tabMain->removeTab(tabMain->indexOf(widResults));
                //tabMain->removeTab(tabMain->indexOf(helpWidget));
            }
        }
    }
    //std::cout<<"deleting old editors ... DONE!\n";
    current=c;
    //std::cout<<"creating new editors ... \n";
    if (current) {
        setWindowTitle(tr("Raw Data Editor/Viewer (%1)").arg(current->getTypeDescription()));
        if (current->getType()!=oldType) {
            editorList.clear();
            for (int i=0; i<current->getEditorCount(); i++) {
                QString n=current->getEditorName(i);
                QFRawDataEditor* e=current->createEditor(services, i, NULL);
                if (e) {
                    e->setSettings(settings, id);
                    e->setCurrent(current, id);
                    //e->setSettings(settings, id);
                    tabMain->addTab(e, n);
                    editorList.append(e);
                }
            }
            tabMain->addTab(widResults, tr("Evaluation &Results"));
            //tabMain->addTab(helpWidget, QIcon(":/lib/help.png"), tr("Online-&Help"));
        } else {
            for (int i=0; i<editorList.size(); i++) {
                editorList[i]->setCurrent(current, id);
            }
        }
        edtName->setText(current->getName());
        edtName->setEnabled(true);
        edtFolder->setText(current->getFolder());
        edtFolder->setEnabled(true);

        pteDescription->setPlainText(current->getDescription());
        pteDescription->setEnabled(true);
        labID->setText(QString::number(current->getID()));
        labTopIcon->setPixmap(current->getSmallIcon().pixmap(16,16));
        labTop->setText(tr("current record:<br /><b>%1</b>").arg(current->getName()));
        labType->setText(current->getTypeDescription());
        labTypeIcon->setPixmap(current->getSmallIcon().pixmap(16,16));
        lstFiles->clear();
        QStringList f=current->getFiles();
        QStringList t=current->getFilesTypes();
        for (int i=0; i<f.size(); i++) {
            if (i<t.size()) {
                if (!t[i].isEmpty()) f[i]=f[i]+QString(" [%1]").arg(t[i]);
            }
        }
        lstFiles->addItems(f);
        lstFiles->setEnabled(true);
        paramFilterProxy->setSourceModel(current->getPropertyModel());
        paramFilterProxy->sort(0);
        tvProperties->setEnabled(true);
        tvResults->setModel(current->resultsGetModel());
        connect(tvResults->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(tvResultsSelectionChanged(const QItemSelection&, const QItemSelection&)));
        connect(pteDescription, SIGNAL(textChanged()), this, SLOT(descriptionChanged()));
        connect(edtName, SIGNAL(textChanged(const QString&)), this, SLOT(nameChanged(const QString&)));
        connect(edtFolder, SIGNAL(textChanged(const QString&)), this, SLOT(folderChanged(const QString&)));
        connect(current->getProject(), SIGNAL(recordAboutToBeDeleted(QFRawDataRecord*)), this, SLOT(recordAboutToBeDeleted(QFRawDataRecord*)));
        connect(current, SIGNAL(propertiesChanged()), this, SLOT(propsChanged()));
        connect(current->getPropertyModel(), SIGNAL(modelReset()), tvProperties, SLOT(resizeColumnsToContents()));
        connect(current->getPropertyModel(), SIGNAL(modelReset()), tvProperties, SLOT(resizeRowsToContents()));
        connect(current->resultsGetModel(), SIGNAL(modelReset()), tvResults, SLOT(resizeColumnsToContents()));
        connect(current->resultsGetModel(), SIGNAL(modelReset()), tvResults, SLOT(resizeRowsToContents()));

        /*QPoint pos;
        pos.setX(current->getProject()->getProperty(QString("rawdatapropeditor%1/posx").arg(id), 20).toInt());
        pos.setY(current->getProject()->getProperty(QString("rawdatapropeditor%1/posy").arg(id), 20).toInt());
        QSize size;
        size.setWidth(current->getProject()->getProperty(QString("rawdatapropeditor%1/width").arg(id), 800).toInt());
        size.setHeight(current->getProject()->getProperty(QString("rawdatapropeditor%1/height").arg(id), 650).toInt());
        resize(size.boundedTo(QApplication::desktop()->screenGeometry(this).size()));
        if (pos.x()<-width() || pos.x()>QApplication::desktop()->screenGeometry(this).width()-30) pos.setX(0);
        if (pos.y()<0 || pos.y()>QApplication::desktop()->screenGeometry(this).height()) pos.setY(0);
        move(pos);
        */

        /*helpWidget->clear();
        QString dll=current->getProject()->getRawDataRecordFactory()->getPluginHelp(current->getType());
        helpWidget->updateHelp(dll);*/

    } else {
        edtName->setText("");
        edtName->setEnabled(false);
        edtFolder->setText("");
        edtFolder->setEnabled(false);
        pteDescription->setPlainText("");
        pteDescription->setEnabled(false);
        labTopIcon->setText("");
        labTop->setText("");
        labID->setText("");
        labType->setText("");
        labTypeIcon->setText("");
        lstFiles->clear();
        lstFiles->setEnabled(false);
        paramFilterProxy->setSourceModel(NULL);
        tvProperties->setEnabled(false);
    }
    //std::cout<<"creating new editors ... DONE!\n";
}

void QFRawDataPropertyEditor::displayHelp() {
    QString dll=current->getProject()->getRawDataRecordFactory()->getPluginHelp(current->getType());
    services->displayHelpWindow(dll);
}

void QFRawDataPropertyEditor::resizeEvent ( QResizeEvent * event ) {
    labAveragedresults->setMaximumWidth(event->size().width());
    QWidget::resizeEvent(event);
}

void QFRawDataPropertyEditor::nameChanged(const QString& text) {
    if (current) {
        current->setName(text);
    }
}

void QFRawDataPropertyEditor::folderChanged(const QString& text) {
    if (current) {
        current->setFolder(text);
    }
}

void QFRawDataPropertyEditor::descriptionChanged() {
    if (current) {
        current->setDescription(pteDescription->toPlainText());
    }
}

void QFRawDataPropertyEditor::recordAboutToBeDeleted(QFRawDataRecord* r) {
    if ((current==r) && current) {
        QFRawDataRecord* n=current->getPreviousOfSameType();
        if (!n) n=current->getNextOfSameType();
        setCurrent(n);
    }
    if (r) {
        if (r->getProject()->getRawDataCount()<=1) {
            close();
        }
    }
}

void QFRawDataPropertyEditor::nextPressed() {
    if (current) {
        QFRawDataRecord* n=current->getNextOfSameType();
        setCurrent(n);
    }
}

void QFRawDataPropertyEditor::previousPressed() {
    if (current) {
        QFRawDataRecord* n=current->getPreviousOfSameType();
        setCurrent(n);
    }
}

void QFRawDataPropertyEditor::deleteRecord() {
    if (current) {
        int ret = QMessageBox::question(this, tr("QuickFit %1").arg(VERSION_FULL),
                                tr("Do you really want to delete the current record?\n   '%1'").arg(current->getName()),
                                QMessageBox::Yes | QMessageBox::No,
                                QMessageBox::No);
        if (ret==QMessageBox::Yes) {
            QPointer<QFRawDataRecord> m=current;
            previousPressed();
            current->getProject()->deleteRawData(m->getID());
            if (current==m) {
                close();
            }
        }
    }
}

void QFRawDataPropertyEditor::propsChanged() {
    if (current) {
        if (current->getName()!=edtName->text()) {
            edtName->setText(current->getName());
        }
        if (current->getFolder()!=edtFolder->text()) {
            edtFolder->setText(current->getFolder());
        }
        if (current->getDescription()!=pteDescription->toPlainText()) {
            pteDescription->setPlainText(current->getDescription());
        }
        labTopIcon->setPixmap(current->getSmallIcon().pixmap(16,16));
        labTop->setText(tr("current record:<br /><b>%1</b>").arg(current->getName()));
        labID->setText(QString::number(current->getID()));
        labType->setText(current->getTypeDescription());
        labTypeIcon->setPixmap(current->getSmallIcon().pixmap(16,16));
        lstFiles->clear();
        QStringList f=current->getFiles();
        QStringList t=current->getFilesTypes();
        for (int i=0; i<f.size(); i++) {
            if (i<t.size()) {
                if (!t[i].isEmpty()) f[i]=f[i]+QString(" [%1]").arg(t[i]);
            }
        }
        lstFiles->addItems(f);
        paramFilterProxy->sort(0);
    }
}



void QFRawDataPropertyEditor::newPropClicked() {
    if (current) {
/*        QStringList items;
        items << tr("integer") << tr("double") << tr("string") << tr("boolean");
        bool ok;
        QString item = QInputDialog::getItem(this, tr("Insert New Property ..."), tr("Property Type:"), items, 1, false, &ok);
        int i=items.indexOf(item);
        if (ok) {
            QString name = QInputDialog::getText(this, tr("Insert New Property ..."),  tr("Property Name:"), QLineEdit::Normal, tr("PROPERTY_NAME"), &ok);
            if (ok) {
                QVariant v;
                if (i==0) v.setValue((int)0);
                if (i==1) v.setValue((double)0.0);
                if (i==2) v.setValue(QString(""));
                if (i==3) v.setValue((bool)false);
                current->setProperty(name, v);
            }
        }*/
        dlgNewProperty* d=new dlgNewProperty(this);
        d->setPropertyName(tr("new_property"));
        d->addPropertyNames(current->getProject()->getAllPropertyNames());

        if (d->exec()==QDialog::Accepted) {
            QVariant v=d->propertyValue();
            switch (d->propertyTypeInt()) {
                case 1: v.convert(QVariant::Double); break;
                case 2: v.convert(QVariant::LongLong); break;
                case 3: v.convert(QVariant::Bool); break;
                case 4: v.convert(QVariant::DateTime); break;
                default: break;
            }
            current->setQFProperty(d->propertyName(), v);
        }

        delete d;
    }
}


void QFRawDataPropertyEditor::deletePropClicked() {
    if (current) {
        QModelIndexList l=tvProperties->selectionModel()->selectedIndexes();
        QStringList props;
        for (int i=0; i<l.size(); i++) {
            int row=l[i].row();
            QString prop=current->getVisibleProperty(row);
            if ((!prop.isEmpty()) && current->propertyExists(prop)) {
                props.append(prop);
            }
        }
        if (props.size()>0) {
            for (int i=0; i<props.size(); i++) {
                //std::cout<<"deleting "<<props[i].toStdString()<<std::endl;
                current->deleteProperty(props[i]);
            }
        }

    }
}

void QFRawDataPropertyEditor::setSettings(ProgramOptions* settings) {
    //std::cout<<"QFRawDataPropertyEditor::setSettings("<<settings<<")\n";
    this->settings=settings;
    if (current && tabMain) {
        for (int i=0; i<current->getEditorCount(); i++) {
            if (qobject_cast<QFRawDataEditor *>(tabMain->widget(i))) {
                qobject_cast<QFRawDataEditor *>(tabMain->widget(i))->setSettings(settings, id);
            }
        }
    }
    readSettings();
}

void QFRawDataPropertyEditor::readSettings() {
    if (!settings) return;
    loadWidgetGeometry(*(settings->getQSettings()), this, QPoint(20, 20), QSize(800, 600), "rawdatapropeditor/");
    if (tabMain) {
        //helpWidget->readSettings(*settings->getQSettings(), "rawdatapropeditor/help_");
        //int idx=settings->getQSettings()->value("rawdatapropeditor/currentTab", 0).toInt();
        //if ((idx>=0) && (idx<tabMain->count())) tabMain->setCurrentIndex(idx);
    }
    for (int i=0; i<editorList.size(); i++) {
        if (editorList[i]) {
            editorList[i]->readSettings();
        }
    }
    currentSaveDir=settings->getQSettings()->value("rawdatapropeditor/lastSaveDir", currentSaveDir).toString();
}

void QFRawDataPropertyEditor::writeSettings() {
    if (!settings) return;
    saveWidgetGeometry(*(settings->getQSettings()), this, "rawdatapropeditor/");
    if (tabMain) {
        settings->getQSettings()->setValue("rawdatapropeditor/currentTab", tabMain->currentIndex());
        //helpWidget->writeSettings(*settings->getQSettings(), "rawdatapropeditor/help_");
    }
    for (int i=0; i<editorList.size(); i++) {
        if (editorList[i]) {
            editorList[i]->writeSettings();
        }
    }
    settings->getQSettings()->setValue("rawdatapropeditor/lastSaveDir", currentSaveDir);
}

void QFRawDataPropertyEditor::tvResultsSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {
    QModelIndexList sel=tvResults->selectionModel()->selectedIndexes();
    QMap<int, QString> names;
    QMap<int, double> sum, sum2, count;
    for (int i=0; i<sel.size(); i++) {
        int r=sel[i].row();
        //int c=sel[i].column();
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

void QFRawDataPropertyEditor::deleteSelectedResults() {
    QModelIndexList sel=tvResults->selectionModel()->selectedIndexes();
    if (sel.size()>0) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::question(this, tr("QuickFit 3.0"),
                     tr("Do you really want to delete the selected results?"),
                     QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (ret == QMessageBox::Yes) {
            // first we store a list of all items to be deleted (one list for the evaluation name (col)
            // and one list for the result name (row).
            //std::cout<<"\n\n-- deleteing "<<sel.size()<<" items:\n";
            QStringList row, col;
            for (int i=0; i<sel.size(); i++) {
                QString hh=tvResults->model()->headerData(sel[i].column(), Qt::Horizontal).toString();
                QString vh=tvResults->model()->headerData(sel[i].row(), Qt::Vertical).toString();
                //std::cout<<"hh="<<hh.toStdString()<<"   vh="<<vh.toStdString()<<"\n";
                row.append(vh);
                col.append(hh);
            }
            //QAbstractItemModel* m=tvResults->model();
            //tvResults->setModel(NULL);
            current->disableEmitResultsChanged();
            for (int i=0; i<qMin(row.size(), col.size()); i++) {
                QString hh=col[i];
                QString vh=row[i];
                current->resultsRemove(hh, vh, false);
            }
            current->enableEmitResultsChanged();
            current->emitResultsChanged();
            //tvResults->setModel(m);
        }
    }
}

void QFRawDataPropertyEditor::saveResults() {
    if (current) {
        QString selectedFilter="";
        QString filter= tr("Comma Separated Values (*.csv *.dat);;Semicolon Separated Values [for german Excel] (*.dat *.txt *.csv);;SYLK dataformat (*.slk *.sylk)");
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Results ..."), currentSaveDir, filter, &selectedFilter);
        if ((!fileName.isEmpty())&&(!fileName.isNull())) {
            int f=filter.split(";;").indexOf(selectedFilter);
            if (f==1) {
                current->resultsSaveToCSV(fileName, ";", ',', '"');
            } else if (f==2) {
                current->resultsSaveToSYLK(fileName);
            } else {
                current->resultsSaveToCSV(fileName, ", ", '.', '"');
            }
            currentSaveDir=QFileInfo(fileName).absolutePath();
        }
    }
}

