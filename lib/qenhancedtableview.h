#ifndef QENHANCEDTABLEVIEW_H
#define QENHANCEDTABLEVIEW_H

#include <QTableView>

/*! \brief this class extends the QTableView
    \ingroup qf3lib_widgets

*/

class QEnhancedTableView : public QTableView {
        Q_OBJECT
    public:
        QEnhancedTableView(QWidget* parent=NULL);
        virtual ~QEnhancedTableView();
    signals:
        void keyPressed(int key, Qt::KeyboardModifiers modifiers, QString text);
    public slots:
        void copySelectionToExcel();
    protected:
        virtual void keyPressEvent(QKeyEvent* event);
    private:
};

#endif // QENHANCEDTABLEVIEW_H
