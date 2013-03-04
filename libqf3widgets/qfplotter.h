#ifndef QFPLOTTER_H
#define QFPLOTTER_H

#include "jkqtplotter.h"
#include "libwid_imexport.h"


class QFWIDLIB_EXPORT QFPlotter : public JKQtPlotter
{
        Q_OBJECT
    public:
        explicit QFPlotter(bool datastore_internal, QWidget* parent=NULL, JKQTPdatastore* datast=NULL);
        explicit QFPlotter(QWidget *parent = 0);
        
    signals:
        
    public slots:
    protected:
        
};

#endif // QFPLOTTER_H
