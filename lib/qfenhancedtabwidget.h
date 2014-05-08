#ifndef QFENHANCEDTABWIDGET_H
#define QFENHANCEDTABWIDGET_H
#include "lib_imexport.h"
#include <QTabWidget>

class QFLIB_EXPORT QFEnhancedTabWidget : public QTabWidget
{
        Q_OBJECT
    public:
        explicit QFEnhancedTabWidget(QWidget *parent = 0);
        
    signals:
        
    public slots:
        void repaintWidget(int i);
        
};

#endif // QFENHANCEDTABWIDGET_H
