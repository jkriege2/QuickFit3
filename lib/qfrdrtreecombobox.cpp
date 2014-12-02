#include "qfrdrtreecombobox.h"
#include <QTreeView>
#include "qfpluginservices.h"

QFRDRTreeComboBox::QFRDRTreeComboBox(QWidget *parent) :
    QComboBox(parent)
{
    QTreeView* tv;
    setView(tv=new QTreeView(this));
    tv->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tv->setHeaderHidden(true);

    projectTree=new QFProjectTreeModel(this);
    projectTree->init(project=QFPluginServices::getInstance()->getCurrentProject(), true, false);
    setModel(projectTree);

    view()->setTextElideMode(Qt::ElideMiddle);
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(myCurrentIndexChanged(int)));

}

QFRDRTreeComboBox::~QFRDRTreeComboBox()
{

}

void QFRDRTreeComboBox::init(QFProject *project)
{
    projectTree->init(project, true, false);
    this->project=project;
}

QFRawDataRecord *QFRDRTreeComboBox::currentRDR() const
{
    QFRawDataRecord* rdr=projectTree->getRawDataByIndex(projectTree->index(currentIndex(), 0));

    return rdr;
}

int QFRDRTreeComboBox::currentRDRID() const
{
    QFRawDataRecord* rdr=projectTree->getRawDataByIndex(projectTree->index(currentIndex(), 0));
    if (rdr) return rdr->getID();
    return -1;
}

void QFRDRTreeComboBox::setCurrentRDR(const QFRawDataRecord *record)
{
    if (!record) {
        setCurrentIndex(-1);
    } else {
        setCurrentIndex(projectTree->index(record).row());
    }
}

void QFRDRTreeComboBox::setCurrentRDRID(int record)
{
    QFRawDataRecord* rdr=project->getRawDataByID(record);
    setCurrentRDR(rdr);
}

void QFRDRTreeComboBox::myCurrentIndexChanged(int i)
{
    emit currentRDRChanged(currentRDR());
}

void QFRDRTreeComboBox::setDisplayRole(bool enabled)
{
    projectTree->setDisplayRole(enable);
}

void QFRDRTreeComboBox::setDisplayGroup(bool enabled)
{
    projectTree->setDisplayGroup(enabled);
}

void QFRDRTreeComboBox::setDisplayGroupAsColor(bool enabled)
{
    projectTree->setDisplayGroupAsColor(enabled);
}

void QFRDRTreeComboBox::setRDRTypeFilter(const QString &type)
{
    projectTree->setRDRTypeFilter(type);
}

void QFRDRTreeComboBox::setRDRApplicableFilter(QFEvaluationItem *eval)
{
    projectTree->setRDRApplicableFilter(eval);
}
