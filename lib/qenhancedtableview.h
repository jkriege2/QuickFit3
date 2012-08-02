#ifndef QENHANCEDTABLEVIEW_H
#define QENHANCEDTABLEVIEW_H

#include "lib_imexport.h"
#include <QTableView>

/*! \brief this class extends the QTableView
    \ingroup qf3lib_widgets

    This enhanced table view adds some functionality to the Qt class:
      - return HTML code that describes the table
      - the selected cells may be copied to Excel using the clipboard (Excel will recognize number !)
    .
*/
class QFLIB_EXPORT QEnhancedTableView : public QTableView {
        Q_OBJECT
    public:
        QEnhancedTableView(QWidget* parent=NULL);
        virtual ~QEnhancedTableView();

        /** \brief return the contents of the table view as HTML fragment */
        QString toHtml(int borderWidth=1, bool non_breaking=false, int fontSizePt=-1) const;
    signals:
        void keyPressed(int key, Qt::KeyboardModifiers modifiers, QString text);
    public slots:
        void copySelectionToExcel(int copyrole=Qt::EditRole, bool storeHead=true);
        void copySelectionToExcelNoHead(int copyrole=Qt::EditRole);
        void copySelectionAsValueErrorToExcel(int valuerole=Qt::EditRole, int errorrole=Qt::EditRole, bool storeHead=true, Qt::Orientation orientation=Qt::Horizontal);
        void copySelectionAsMedianQuantilesToExcel(int medianrole=Qt::EditRole, int q25role=-1, int q75role=-1, bool storeHead=true, Qt::Orientation orientation=Qt::Horizontal);
    protected:
        virtual void keyPressEvent(QKeyEvent* event);
    private:
};

#endif // QENHANCEDTABLEVIEW_H
