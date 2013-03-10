#include "qfplotter.h"
#include "qfplotterprivate.h"
#include "programoptions.h"

QFPlotter::QFPlotter(QWidget *parent) :
    JKQtPlotter(parent)
{
    initQFPlotter();
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
    toolbar->insertAction(plotter->get_actCopyMatlab(), p->actCopyToTable);

}

void QFPlotter::modifyContextMenu(QMenu *menu)
{
    menu->insertAction(plotter->get_actCopyMatlab(), p->actCopyToTable);
}
