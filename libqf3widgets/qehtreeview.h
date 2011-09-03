#ifndef QEHTREEVIEW_H
#define QEHTREEVIEW_H

#include <QTreeView>
#include "libwid_imexport.h"

/*! \brief this class extends the QTreeView by including signals that will be
           emitted by several internal events (key events ...).
    \ingroup qf3lib_widgets

  This is usefull, if you want to trigger actions by a keypress ...
*/

class QFWIDLIB_EXPORT QEHTreeView : public QTreeView {
        Q_OBJECT
    public:
        /** \brief Default constructor */
        QEHTreeView(QWidget * parent = NULL);
        /** \brief Default destructor */
        virtual ~QEHTreeView();
    signals:
        void keyPressed(QKeyEvent * event);
        void keyReleased(QKeyEvent * event);
    protected:
        virtual void keyPressEvent ( QKeyEvent * event );
        virtual void keyReleaseEvent ( QKeyEvent * event );
    private:
};

#endif // QEHTREEVIEW_H
