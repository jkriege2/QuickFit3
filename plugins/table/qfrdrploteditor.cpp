#include "qfrdrploteditor.h"
#include "qfrdrtable.h"
#include "dlgcsvparameters.h"


QFRDRPlotEditor::QFRDRPlotEditor(QFPluginServices* services,  QFRawDataPropertyEditor* propEditor, QWidget* parent):
    QFRawDataEditor(services, propEditor, parent)
{
    //std::cout<<"QFRDRPlotEditor() ...\n";
    createWidgets();
    //std::cout<<"QFRDRPlotEditor() ... done\n";
}

QFRDRPlotEditor::~QFRDRPlotEditor()
{
    writeSettings();
}

void QFRDRPlotEditor::createWidgets() {
    QVBoxLayout* l=new QVBoxLayout(this);
    setLayout(l);

    tbMain=new QToolBar("tbPlotMain", this);
    l->addWidget(tbMain);

    tabPlots=new QTabWidget(this);
    tabPlots->setTabsClosable(true);
    connect(tabPlots, SIGNAL(tabCloseRequested(int)), this, SLOT(deletePlot(int)));
    l->addWidget(tabPlots);

    actAddPlot=new QAction(QIcon(":/table/plot_add.png"), tr("add plot"), this);
    actDeletePlot=new QAction(QIcon(":/table/plot_delete.png"), tr("delete current plot"), this);

    tbMain->addAction(actAddPlot);
    tbMain->addAction(actDeletePlot);


    menuEdit=propertyEditor->addMenu("&Edit", 1);


    menuPlot=propertyEditor->addMenu("&Plot", 1);
    menuPlot->addAction(actAddPlot);
    menuPlot->addAction(actDeletePlot);

}

void QFRDRPlotEditor::connectWidgets(QFRawDataRecord* current, QFRawDataRecord* old) {
    if (old) {
        QFRDRTable* m=qobject_cast<QFRDRTable*>(old);
        if (m && m->model()) {

            //disconnect(m->model(), SIGNAL(notReadonlyChanged(bool)), actPaste, SLOT(setEnabled(bool)));
        }
    }
    //std::cout<<"qobject_cast ... ";
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    //std::cout<<m<<" ... ";
    //if (m) std::cout<<m->model()<<" ... ";
    rebuildPlotWidgets();
    //std::cout<<m<<" done\n";
}

void QFRDRPlotEditor::rawDataChanged() {
}

void QFRDRPlotEditor::readSettings() {
    if (settings) {
        //std::cout<<"QFRDRPlotEditor::readSettings()\n";
        currentPlotDir=settings->getQSettings()->value("rawtableeditor/currentPlotDir", currentPlotDir).toString();
    }
}

void QFRDRPlotEditor::writeSettings() {
    if (settings) {
        settings->getQSettings()->setValue("rawtableeditor/currentPlotDir", currentPlotDir);
        settings->getQSettings()->sync();
    }
}

void QFRDRPlotEditor::addPlot() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
        }
    }

}

void QFRDRPlotEditor::deleteCurrentPlot() {
    QFRDRTable* m=qobject_cast<QFRDRTable*>(current);
    if (m) {
        if (m->model()) {
        }
    }

}

void QFRDRPlotEditor::deletePlot(int i)
{
}

void QFRDRPlotEditor::rebuildPlotWidgets()
{
}






