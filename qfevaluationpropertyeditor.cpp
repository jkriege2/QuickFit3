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
    invalidateFilter();
}











QFEvaluationPropertyEditor::QFEvaluationPropertyEditor(ProgramOptions* set, QFEvaluationItem* current, int id, QWidget* parent, Qt::WindowFlags f):
    QWidget(parent, f)
{
    //std::cout<<"creating QFEvaluationPropertyEditor ... \n";
    this->id=id;
    this->current=NULL;

    rdrModel=new QFProjectRawDataModel(NULL);
    rdrProxy=new QFEvaluationRawDataModelProxy(rdrModel);
    rdrProxy->setSourceModel(rdrModel);

    setSettings(set);
    //std::cout<<"creating QFEvaluationPropertyEditor ... creating widgets ...\n";
    createWidgets();
    //std::cout<<"creating QFEvaluationPropertyEditor ... setting current ...\n";
    setCurrent(current);
    //std::cout<<"creating QFEvaluationPropertyEditor ... DONE!\n";
}

QFEvaluationPropertyEditor::~QFEvaluationPropertyEditor()
{
    std::cout<<"deleting QFEvaluationPropertyEditor\n";
    std::cout<<"deleting QFEvaluationPropertyEditor ... OK\n";
}

void QFEvaluationPropertyEditor::closeEvent ( QCloseEvent * event ) {
    writeSettings();
    event->accept();
}

void QFEvaluationPropertyEditor::setCurrent(QFEvaluationItem* c) {
    QString oldType="";
    int oldEditorCount=0;
    if (current) {
        std::cout<<"disconnecting old ...\n";
        lstRawData->setModel(NULL);
        rdrProxy->setEvaluation(NULL);
        rdrModel->setProject(NULL);
        oldType=current->getType();
        oldEditorCount=current->getEditorCount();
        disconnect(current->getProject(), SIGNAL(evaluationAboutToBeDeleted(QFEvaluationItem*)), this, SLOT(evaluationAboutToBeDeleted(QFEvaluationItem*)));
        disconnect(edtName, SIGNAL(textChanged(const QString&)), this, SLOT(nameChanged(const QString&)));
        disconnect(pteDescription, SIGNAL(textChanged()), this, SLOT(descriptionChanged()));
        disconnect(current, SIGNAL(propertiesChanged()), this, SLOT(propsChanged()));
        if (c) {
            if (c->getType()!=oldType) {
                for (int i=oldEditorCount; i>=1; i--) {
                    QWidget* w=tabMain->widget(i);
                    tabMain->removeTab(i);
                    if (qobject_cast<QFEvaluationEditor *>(w)) qobject_cast<QFEvaluationEditor *>(w)->setSettings(NULL, id);
                    w->deleteLater();
                    //delete w;
                }
            }
        }
        std::cout<<"old disconnected ...\n";
    }
    current=c;
    if (current) {
        std::cout<<"connecting new ...\n";
        rdrModel->setProject(current->getProject());
        rdrProxy->setEvaluation(current);
        if (current->getType()!=oldType) {
            editorList.clear();
            for (int i=0; i<current->getEditorCount(); i++) {
                QString n=current->getEditorName(i);
                //std::cout<<"creating tab '"<<n.toStdString()<<"' ... \n";
                QFEvaluationEditor* e=current->createEditor(i, tabMain);
                //std::cout<<"creating tab '"<<n.toStdString()<<"' ... reading settings\n";
                e->setSettings(settings, id);
                //std::cout<<"creating tab '"<<n.toStdString()<<"' ... setting current\n";
                e->setCurrent(current, id);
                //std::cout<<"creating tab '"<<n.toStdString()<<"' ... adding tab\n";
                tabMain->addTab(e, n);
                //std::cQFEvaluationPropertyEditor::setSettings(out<<"creating tab '"<<n.toStdString()<<"' ... done\n";
                editorList.append(e);
            }
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

        std::cout<<"new connected ...\n";


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
        QFEvaluationItem* n=current->getPreviousOfSameType();
        if (!n) n=current->getNextOfSameType();
        setCurrent(n);
    }
    if (r) {
        if (r->getProject()->getRawDataCount()<=1) {
            close();
        }
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

    lstRawData=new QListView(tabMain);
    tabMain->addTab(lstRawData, tr("raw data"));

}

void QFEvaluationPropertyEditor::setSettings(ProgramOptions* settings) {
    this->settings=settings;
    std::cout<<"QFEvaluationPropertyEditor::setSettings("<<settings<<")\n";
    if (current && tabMain) {
        for (int i=0; i<current->getEditorCount(); i++) {
            if (qobject_cast<QFEvaluationEditor *>(tabMain->widget(i))) {
                qobject_cast<QFEvaluationEditor *>(tabMain->widget(i))->setSettings(settings, id);
            }
        }
    }
    readSettings();
}


void QFEvaluationPropertyEditor::readSettings() {
    std::cout<<"QFEvaluationPropertyEditor::readSettings()\n";
    if (!settings) return;
    QPoint pos = settings->getQSettings()->value(QString("evalpropeditor/pos").arg(id), QPoint(200, 200)).toPoint();
    QSize size = settings->getQSettings()->value(QString("evalpropeditor/size").arg(id), QSize(300, 400)).toSize();
    resize(size.boundedTo(QApplication::desktop()->screenGeometry(this).size()));
    if (pos.x()<-width() || pos.x()>QApplication::desktop()->screenGeometry(this).width()-30) pos.setX(0);
    if (pos.y()<0 || pos.y()>QApplication::desktop()->screenGeometry(this).height()) pos.setY(0);
    move(pos);
}

void QFEvaluationPropertyEditor::writeSettings() {
    if (!settings) return;
    settings->getQSettings()->setValue(QString("evalpropeditor/pos").arg(id), pos());
    settings->getQSettings()->setValue(QString("evalpropeditor/size").arg(id), size());
}







