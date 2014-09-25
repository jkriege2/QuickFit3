#include "qfcompleterproperties.h"

QFCompleterProperties::QFCompleterProperties(QObject *parent):
    QCompleter(parent)
{
    m_project=NULL;
    m_stringlist=new QStringListModel(this);
    setModel(m_stringlist);
    setCompletionMode(QCompleter::PopupCompletion);
}

QFCompleterProperties::~QFCompleterProperties()
{
    setProject(NULL);
}

void QFCompleterProperties::setProject(QFProject *project)
{
    if (m_project) {
        disconnect(m_project, SIGNAL(propertiesChanged()), this, SLOT(propertiesChanged()));
    }
    this->m_project=project;
    QStringList sl;
    if (m_project) {
        sl=m_project->getAllPropertyNames();
        connect(m_project, SIGNAL(propertiesChanged()), this, SLOT(propertiesChanged()));
    }
    m_stringlist->setStringList(sl);
}

void QFCompleterProperties::propertiesChanged()
{
    QStringList sl;
    if (m_project) {
        sl=m_project->getAllPropertyNames();
    }
    m_stringlist->setStringList(sl);
}
