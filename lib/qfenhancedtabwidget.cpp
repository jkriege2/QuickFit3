#include "qfenhancedtabwidget.h"
#include <QLayout>

QFEnhancedTabWidget::QFEnhancedTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(repaintWidget(int)));
}

void QFEnhancedTabWidget::repaintWidget(int i)
{
    if (widget(i)) {
        if (widget(i)->layout()) {
            widget(i)->layout()->invalidate();
        }
        widget(i)->show();
        widget(i)->repaint();
    }
}
