#ifndef QENHANCEDTABLEVIEW_H
#define QENHANCEDTABLEVIEW_H

#include <QTableView>
#include <QPrinter>
#include <QLabel>
#include <QCheckBox>
/*! \brief this class extends the QTableView
    \ingroup qf3lib_widgets

*/

class QEnhancedTableView : public QTableView
{
        Q_OBJECT
    public:
        QEnhancedTableView(QWidget* parent=NULL);
        virtual ~QEnhancedTableView();
        void print(QPrinter* printer, bool onePageWide=false, bool onePageHigh=false);

        void paint(QPainter& painter, QRect pageRec=QRect());
        QSizeF getTotalSize() const;
    public slots:
        void copySelectionToExcel();

        void print();
    protected:
        virtual void keyPressEvent(QKeyEvent* event);
        virtual bool event(QEvent* event);

        void paint(QPainter &painter, double scale, int page, double hhh, double vhw, const QList<int>& pageCols, const QList<int>& pageRows, QPrinter* p=NULL);

        QPrinter* getPrinter(QPrinter* printerIn=NULL, bool *localPrinter=NULL);
    private:
};

#endif // QENHANCEDTABLEVIEW_H
