#ifndef QFPLOTTERPRIVATE_H
#define QFPLOTTERPRIVATE_H

#include <QObject>
#include <QAction>
class QFPlotter; // forward

class QFPlotterPrivate : public QObject
{
        Q_OBJECT
    public:
        friend class QFPlotter;
        explicit QFPlotterPrivate( QFPlotter* plotter, QObject *parent = 0);
        
    signals:
        
    public slots:
        void copyToTable();

    protected:
        QAction* actCopyToTable;
        QFPlotter* plotter;
        
};

#endif // QFPLOTTERPRIVATE_H
