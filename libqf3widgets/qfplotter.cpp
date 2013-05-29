#include "qfplotter.h"
#include "qfplotterprivate.h"
#include "programoptions.h"

QFPlotter::QFPlotter(QWidget *parent) :
    JKQtPlotter(parent)
{
    initQFPlotter();
}

QToolBar *QFPlotter::createToolbar(QWidget *parent, const QString &name)
{
    QString n=name;
    if (n.isEmpty()) n=objectName()+"_toolbar";
    QToolBar* tool=new QToolBar(n, parent);
    populateToolbar(tool);
    return tool;
}

QFPlotter::QFPlotter(bool datastore_internal, QWidget *parent, JKQTPdatastore *datast):
    JKQtPlotter(datastore_internal, parent, datast)
{
    initQFPlotter();
}

void QFPlotter::initQFPlotter()
{
    p=new QFPlotterPrivate(this, this);
    get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
    //toolbar->insertAction(plotter->get_actCopyMatlab(), p->actCopyToTable);
    toolbar->clear();
    toolbar->addSeparator();
    toolbar->addSeparator();
    populateToolbar(toolbar);

}

void QFPlotter::modifyContextMenu(QMenu *menu)
{
    menu->insertAction(plotter->get_actCopyMatlab(), p->actCopyToTable);
    menu->addSeparator();
    menu->addAction(p->actHelp);
}

void QFPlotter::populateToolbar(QToolBar *toolbar) const
{
    JKQtPlotter::populateToolbar(toolbar);
    toolbar->insertAction(plotter->get_actCopyMatlab(), p->actCopyToTable);
    toolbar->addSeparator();
    toolbar->addAction(p->actHelp);
}
