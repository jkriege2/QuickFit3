#ifndef QFPLOTTERPRIVATE_H
#define QFPLOTTERPRIVATE_H

#include <QObject>
class QFPlotter; // forward

class QFPlotterPrivate : public QObject
{
        Q_OBJECT
    public:
        friend class QFPlotter;
        explicit QFPlotterPrivate(QObject *parent = 0);
        
    signals:
        
    public slots:
        
};

#endif // QFPLOTTERPRIVATE_H
