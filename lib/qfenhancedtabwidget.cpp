#include "qfenhancedtabwidget.h"

QFEnhancedTabWidget::QFEnhancedTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(repaintWidget(int)));
}

void QFEnhancedTabWidget::repaintWidget(int i)
{
    if (widget(i)) {
        widget(i)->show();
        widget(i)->repaint();
    }
}
