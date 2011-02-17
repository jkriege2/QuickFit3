#include "qfrawdatapropertyeditor.h"
#include "dlgnewproperty.h"

// TODO: add some more options to the fit results display: average over several selected data items
// TODO: add some more options to the fit results display: store column under different name


QFRawDataPropertyEditor::QFRawDataPropertyEditor(QFPluginServices* services, ProgramOptions* set, QFRawDataRecord* current, int id, QWidget* parent, Qt::WindowFlags f):
    QWidget(parent, f)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
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
    std::cout<<"deleting old editors ... \n";
    int oldEditorCount=0;
    if (current) {
        oldType=current->getType();
        oldEditorCount=current->getEditorCount();
        disconnect(current->getProject(), SIGNAL(recordAboutToBeDeleted(QFRawDataRecord*)), this, SLOT(recordAboutToBeDeleted(QFRawDataRecord*)));
        disconnect(edtName, SIGNAL(textChanged(const QString&)), this, SLOT(nameChanged(const QString&)));
        disconnect(pteDescription, SIGNAL(textChanged()), this, SLOT(descriptionChanged()));
        disconnect(current, SIGNAL(propertiesChanged()), this, SLOT(propsChanged()));
        disconnect(current->getPropertyModel(), SIGNAL(modelReset()), this, SLOT(resizePropertiesTable()));
        disconnect(current->resultsGetModel(), SIGNAL(modelReset()), tvResults, SLOT(resizeColumnsToContents()));
        if (c) {
            if (c->getType()!=oldType) {
                for (int i=editorList.size()-1; i>=0; i--) {
                    QFRawDataEditor* w=editorList[i];
                    if (w) {
                        tabMain->removeTab(tabMain->indexOf(editorList[i]));
                        QCoreApplication::processEvents();
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
            }
        }
    }
    std::cout<<"deleting old editors ... DONE!\n";
    current=c;
    std::cout<<"creating new editors ... \n";
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
        } else {
            for (int i=0; i<editorList.size(); i++) {
                editorList[i]->setCurrent(current, id);
            }
        }
        edtName->setText(current->getName());
        edtName->setEnabled(true);
        pteDescription->setPlainText(current->getDescription());
        pteDescription->setEnabled(true);
        labID->setText(QString::number(current->getID()));
        labTopIcon->setPixmap(current->getSmallIcon().pixmap(16,16));
        labTop->setText(tr("current record:<br /><b>%1</b>").arg(current->getName()));
        labType->setText(current->getTypeDescription());
        labTypeIcon->setPixmap(current->getSmallIcon().pixmap(16,16));
        lstFiles->clear();
        lstFiles->addItems(current->getFiles());
        lstFiles->setEnabled(true);
        tvProperties->setModel(current->getPropertyModel());
        tvProperties->setEnabled(true);
        tvResults->setModel(current->resultsGetModel());
        connect(edtName, SIGNAL(textChanged(const QString&)), this, SLOT(nameChanged(const QString&)));
        connect(pteDescription, SIGNAL(textChanged()), this, SLOT(descriptionChanged()));
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


    } else {
        edtName->setText("");
        edtName->setEnabled(false);
        pteDescription->setPlainText("");
        pteDescription->setEnabled(false);
        labTopIcon->setText("");
        labTop->setText("");
        labID->setText("");
        labType->setText("");
        labTypeIcon->setText("");
        lstFiles->clear();
        lstFiles->setEnabled(false);
        tvProperties->setModel(NULL);
        tvProperties->setEnabled(false);
    }
    std::cout<<"creating new editors ... DONE!\n";
}

void QFRawDataPropertyEditor::resizeEvent ( QResizeEvent * event ) {
    if (current) {
        /*current->getProject()->setIntProperty(QString("rawdatapropeditor%1/posx").arg(id), pos().x());
        current->getProject()->setIntProperty(QString("rawdatapropeditor%1/posy").arg(id), pos().y());
        current->getProject()->setIntProperty(QString("rawdatapropeditor%1/width").arg(id), size().width());
        current->getProject()->setIntProperty(QString("rawdatapropeditor%1/height").arg(id), size().height());
        */
    }
    QWidget::resizeEvent(event);
}

void QFRawDataPropertyEditor::nameChanged(const QString& text) {
    if (current) {
        current->setName(text);
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

void QFRawDataPropertyEditor::propsChanged() {
    if (current) {
        if (current->getName()!=edtName->text()) {
            edtName->setText(current->getName());
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
        lstFiles->addItems(current->getFiles());
    }
}


void QFRawDataPropertyEditor::createWidgets() {
    QVBoxLayout* ml=new QVBoxLayout(this);
    setLayout(ml);
    ml->setContentsMargins(2,2,2,2);
    QHBoxLayout* vl=new QHBoxLayout(this);
    ml->addLayout(vl);
    btnPrevious=new QPushButton(QIcon(":/prop_previous.png"), tr("&previous"), this);
    btnPrevious->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    vl->addWidget(btnPrevious);
    connect(btnPrevious, SIGNAL(clicked()), this, SLOT(previousPressed()));
    btnNext=new QPushButton(QIcon(":/prop_next.png"), tr("&next"), this);
    btnNext->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    vl->addWidget(btnNext);
    labTopIcon=new QLabel(this);
    vl->addWidget(labTopIcon);
    labTop=new QLabel(this);
    vl->addWidget(labTop);
    vl->addStretch();
    connect(btnNext, SIGNAL(clicked()), this, SLOT(nextPressed()));
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
    lstFiles=new QListWidget(w);
    fl->addRow(tr("&Files:"), lstFiles);
    tvProperties=new QTableView(w);
    QFontMetrics fm(font());
    tvProperties->verticalHeader()->setDefaultSectionSize((int)round((double)fm.height()*1.1));
    tvProperties->horizontalHeader()->setStretchLastSection(true);

    QHBoxLayout* pl1=new QHBoxLayout(this);
    pl1->setContentsMargins(0,0,0,0);
    pl1->addWidget(tvProperties);
    QVBoxLayout* pl2=new QVBoxLayout(this);
    pl1->addLayout(pl2);
    btnNewProperty=new QPushButton(QIcon(":/prop_add.png"), tr("In&sert Property"), this);
    connect(btnNewProperty, SIGNAL(clicked()), this, SLOT(newPropClicked()));
    pl2->addWidget(btnNewProperty);
    btnDeleteProperty=new QPushButton(QIcon(":/prop_delete.png"), tr("&Delete Property"), this);
    connect(btnDeleteProperty, SIGNAL(clicked()), this, SLOT(deletePropClicked()));
    pl2->addWidget(btnDeleteProperty);
    pl2->addStretch();
    fl->addRow(tr("Properties:"), pl1);

    widResults=new QWidget(this);
    QVBoxLayout* rwvlayout=new QVBoxLayout(this);
    widResults->setLayout(rwvlayout);
    tvResults=new QEnhancedTableView(widResults);
    tvResults->setAlternatingRowColors(true);
    tvResults->verticalHeader()->setDefaultSectionSize((int)round((double)fm.height()*1.5));
    rwvlayout->addWidget(tvResults);
    tabMain->addTab(widResults, tr("Evaluation &Results"));

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
        d->cmbName->setEditText(tr("new_property"));
        d->cmbName->addItems(current->getProject()->getAllPropertyNames());

        if (d->exec()==QDialog::Accepted) {
            QVariant v=d->edtValue->text();
            switch (d->cmbType->currentIndex()) {
                case 1: v.convert(QVariant::Double); break;
                case 2: v.convert(QVariant::LongLong); break;
                case 3: v.convert(QVariant::Bool); break;
                case 4: v.convert(QVariant::DateTime); break;
            }
            current->setQFProperty(d->cmbName->currentText(), v);
        }

        delete d;
    }
}


void QFRawDataPropertyEditor::deletePropClicked() {
    if (current) {
        QModelIndexList l=tvProperties->selectionModel()->selectedIndexes();
        for (int i=current->getPropertyModel()->rowCount()-1; i>=0; i--) {
            if (tvProperties->selectionModel()->rowIntersectsSelection(i, QModelIndex()) ) {
                //std::cout<<"deleting "<<current->getPropertyName(i).toStdString()<<std::endl;
                QString n=current->getPropertyName(i);
                if (n!="" && current->propertyExists(n)) {
                    current->deleteProperty(n);
                    return;
                }
            }
        }
    }
}

void QFRawDataPropertyEditor::setSettings(ProgramOptions* settings) {
    std::cout<<"QFRawDataPropertyEditor::setSettings("<<settings<<")\n";
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
        int idx=settings->getQSettings()->value("rawdatapropeditor/currentTab", 0).toInt();
        //if ((idx>=0) && (idx<tabMain->count())) tabMain->setCurrentIndex(idx);
    }
    for (int i=0; i<editorList.size(); i++) {
        if (editorList[i]) {
            editorList[i]->readSettings();
        }
    }
}

void QFRawDataPropertyEditor::writeSettings() {
    if (!settings) return;
    saveWidgetGeometry(*(settings->getQSettings()), this, "rawdatapropeditor/");
    if (tabMain) settings->getQSettings()->setValue("rawdatapropeditor/currentTab", tabMain->currentIndex());
    for (int i=0; i<editorList.size(); i++) {
        if (editorList[i]) {
            editorList[i]->writeSettings();
        }
    }
}







