#include "qehtreeview.h"

QEHTreeView::QEHTreeView(QWidget * parent):
  QTreeView(parent)
{
};

QEHTreeView::~QEHTreeView() {
};

void QEHTreeView::keyPressEvent ( QKeyEvent * event ) {
    emit keyPressed(event);
    QTreeView::keyPressEvent(event);
}

void QEHTreeView::keyReleaseEvent ( QKeyEvent * event ) {
    emit keyReleased(event);
}


