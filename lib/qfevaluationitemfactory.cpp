#include "qfevaluationitemfactory.h"
#include "qfevaluationitem.h"

QFEvaluationItemFactory::QFEvaluationItemFactory(QObject* parent):
    QObject(parent)
{

}

QFEvaluationItemFactory::~QFEvaluationItemFactory()
{
    //dtor
}


void QFEvaluationItemFactory::searchPlugins(QString directory) {
    QDir pluginsDir = QDir(directory);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            QFPluginEvaluationItem* iRecord = qobject_cast<QFPluginEvaluationItem*>(plugin);
            if (iRecord) {
                items[iRecord->getID()]=iRecord;
                filenames[iRecord->getID()]=fileName;
            }
        }
    }
}

void QFEvaluationItemFactory::distribute(QFProject* project, ProgramOptions* settings, QFPluginReportWrapper* reporter, QWidget* parent) {
    for (int i=0; i<getIDList().size(); i++) {
        items[getIDList().at(i)]->setProject(project);
        items[getIDList().at(i)]->setSettings(settings);
        items[getIDList().at(i)]->setReporter(reporter);
        items[getIDList().at(i)]->setParentWidget(parent);
    }
}




