#include "qfplotter.h"


QFPlotter::QFPlotter(QWidget *parent) :
    JKQtPlotter(parent)
{
}

QFPlotter::QFPlotter(bool datastore_internal, QWidget *parent, JKQTPdatastore *datast):
    JKQtPlotter(datastore_internal, parent, datast)
{
}
