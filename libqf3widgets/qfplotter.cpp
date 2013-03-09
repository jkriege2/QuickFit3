#include "qfplotter.h"
#include "qfplotterprivate.h"
#include "programoptions.h"

QFPlotter::QFPlotter(QWidget *parent) :
    JKQtPlotter(parent)
{
    p=new QFPlotterPrivate(this);
    get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
}

QFPlotter::QFPlotter(bool datastore_internal, QWidget *parent, JKQTPdatastore *datast):
    JKQtPlotter(datastore_internal, parent, datast)
{
    p=new QFPlotterPrivate(this);
    get_plotter()->set_userSettigsFilename(ProgramOptions::getInstance()->getIniFilename());
}
