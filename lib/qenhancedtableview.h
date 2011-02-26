#ifndef QENHANCEDTABLEVIEW_H
#define QENHANCEDTABLEVIEW_H

#include <QTableView>

/*! \brief this class extends the QTableView
    \ingroup qf3lib_widgets

*/

class QEnhancedTableView : public QTableView
{
    public:
        QEnhancedTableView(QWidget* parent=NULL);
        virtual ~QEnhancedTableView();
    public slots:
        void copySelectionToExcel();
    protected:
        virtual void keyPressEvent(QKeyEvent* event);
    private:
};

#endif // QENHANCEDTABLEVIEW_H
