#ifndef QFPLOTTER_H
#define QFPLOTTER_H

#include "jkqtplotter.h"
#include "libwid_imexport.h"
#include <QToolBar>

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

        QToolBar* createToolbar(QWidget* parent=NULL, const QString& name=QString(""));
        virtual void populateToolbar(QToolBar* toolbar) const;

        QAction* get_actCopyToTable() const;
        QAction* get_actHelp() const;
    signals:
        
    public slots:
    protected:
        void initQFPlotter();
        virtual void modifyContextMenu(QMenu* menu);

        
};

#endif // QFPLOTTER_H
