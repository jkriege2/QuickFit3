#ifndef QFPLOTTER_H
#define QFPLOTTER_H

#include "jkqtplotter.h"
#include "libwid_imexport.h"

class QFPlotterPrivate; // forward

class QFWIDLIB_EXPORT QFPlotter : public JKQtPlotter
{
        Q_OBJECT
    protected:
        QFPlotterPrivate* p;
    public:
        friend class QFPlotterPrivate;
        explicit QFPlotter(bool datastore_internal, QWidget* parent=NULL, JKQTPdatastore* datast=NULL);
        explicit QFPlotter(QWidget *parent = 0);
        
    signals:
        
    public slots:
    protected:
        void initQFPlotter();
        virtual void modifyContextMenu(QMenu* menu);
        
};

#endif // QFPLOTTER_H
