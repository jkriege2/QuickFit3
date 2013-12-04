#include "qfrdrtablecombobox.h"


QFRDRTableComboBoxMatchFunctor::QFRDRTableComboBoxMatchFunctor(bool requireColumnGraphics, bool requireNotReadonly):
    QFMatchRDRFunctor()
{
    this->requireColumnGraphics=requireColumnGraphics;
    this->requireNotReadonly=requireNotReadonly;
}

void QFRDRTableComboBoxMatchFunctor::setRequireColumnGraphics(bool require)
{
    this->requireColumnGraphics=require;
}

void QFRDRTableComboBoxMatchFunctor::setRequireNotReadonly(bool require)
{
    this->requireNotReadonly=require;
}


bool QFRDRTableComboBoxMatchFunctor::matches(const QFRawDataRecord *record) const
{
    if (!record) return false;
    const QFRDRTableInterface* tab=dynamic_cast<const QFRDRTableInterface*>(record);
    bool ok=false;
    if (tab) {
        ok=true;
        //qDebug()<<"        "<<this->requireColumnGraphics<<record->inherits("QFRDRColumnGraphsInterface");
        //qDebug()<<"        "<<this->requireNotReadonly<<tab->tableIsReadonly();
        if (this->requireColumnGraphics && !record->inherits("QFRDRColumnGraphsInterface")) ok=false;
        if (this->requireNotReadonly && tab->tableIsReadonly()) ok=false;

    }
    //qDebug()<<"match "<<record->getName()<<" : "<<ok;
    return ok;
}










QFRDRTableComboBox::QFRDRTableComboBox(QWidget *parent) :
    QFRDRComboBox(parent)
{
    tabFunctor=new QFRDRTableComboBoxMatchFunctor();
}

QFRDRTableComboBox::~QFRDRTableComboBox()
{
    delete tabFunctor;
}

void QFRDRTableComboBox::init(QFProject *project)
{
    QFRDRComboBox::init(project, tabFunctor, false);
}

QFRDRColumnGraphsInterface *QFRDRTableComboBox::currentColumnGraphics() const
{
    return dynamic_cast<QFRDRColumnGraphsInterface*>(currentRDR());
}

QFRDRTableInterface *QFRDRTableComboBox::currentTable() const
{
    return dynamic_cast<QFRDRTableInterface*>(currentRDR());
}

void QFRDRTableComboBox::setRequireColumnGraphics(bool require)
{
    tabFunctor->setRequireColumnGraphics(require);
}

void QFRDRTableComboBox::setRequireNotReadonly(bool require)
{
    tabFunctor->setRequireNotReadonly(require);
}

void QFRDRTableComboBox::myCurrentIndexChanged(int i)
{
    QFRDRComboBox::myCurrentIndexChanged(i);
    //Debug()<<"emitting currentColumnGraphicsChanged("<<currentColumnGraphics()<<")";
    emit currentColumnGraphicsChanged(currentColumnGraphics());
    //qDebug()<<"emitting currentTableChanged("<<currentTable()<<")";
    emit currentTableChanged(currentTable());

}

QFRDRColumnGraphsComboBox::QFRDRColumnGraphsComboBox(QWidget *parent):
    QComboBox(parent)
{
    cols=NULL;
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(myCurrentIndexChanged(int)));
}

QFRDRColumnGraphsComboBox::~QFRDRColumnGraphsComboBox()
{
}

QFRDRColumnGraphsInterface *QFRDRColumnGraphsComboBox::currentColumnGraphics() const
{
    return cols;
}

void QFRDRColumnGraphsComboBox::setColumnGraphs(QFRDRColumnGraphsInterface *cols)
{
    this->cols=cols;
    refill();
}

void QFRDRColumnGraphsComboBox::refill()
{
    QString current=currentText();

    setUpdatesEnabled(false);
    disconnect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(myCurrentIndexChanged(int)));

    clear();
    if (cols) {
        for (int i=0; i<cols->colgraphGetPlotCount(); i++) {
            addItem(cols->colgraphGetPlotTitle(i));
        }
    }

    setUpdatesEnabled(true);

    int idx=findText(current);
    if (idx>=0) setCurrentIndex(idx);
    else setCurrentIndex(0);
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(myCurrentIndexChanged(int)));
    if (count()<=0) emit refilledEmpty();
    emit refilled(count()>0);
}

void QFRDRColumnGraphsComboBox::myCurrentIndexChanged(int i)
{
    //qDebug()<<"myCurrentIndexChanged(index="<<i<<")";
}

