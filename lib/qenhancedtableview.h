#ifndef QENHANCEDTABLEVIEW_H
#define QENHANCEDTABLEVIEW_H

#include "lib_imexport.h"
#include <QTableView>
#include <QTextDocument>
#include <QMenu>
#include <QToolBar>

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
        QEnhancedTableView(QWidget* parent=NULL, bool noCopyShortcut=false);
        virtual ~QEnhancedTableView();

        /** \brief return the contents of the table view as HTML fragment */
        QString toHtml(int borderWidth=1, bool non_breaking=false, int fontSizePt=-1) const;
        QAction* getActCopyExcel() const;
        QAction* getActCopyExcelNoHead() const;
        QAction* getActCopyMatlab() const;
        QAction* getActPrint() const;
        QAction* getActSave() const;
        QAction* getActSaveExtended() const;

        void addActionsToToolbar(QToolBar* tb) const;
        void addActionsToMenu(QMenu* m) const;

        void print(QPrinter* printer, bool onePageWide=false, bool onePageHigh=false);

        void paint(QPainter& painter, QRect pageRec=QRect());
        QSizeF getTotalSize() const;

        void getVariantDataTable(int copyrole, QList<QList<QVariant> >& csvData, QStringList& colnames, QStringList& rownames) const;

    signals:
        void keyPressed(int key, Qt::KeyboardModifiers modifiers, QString text);
    public slots:
        void print();
        void save(int copyrole=Qt::EditRole, bool flipped=false, bool storeHead=true);
        void saveExtended(int copyrole=Qt::EditRole, bool flipped=false, bool storeHead=true);
        void saveFlipped(int copyrole=Qt::EditRole);
        void saveExtendedFlipped(int copyrole=Qt::EditRole);
        void copyAsImage();
        void copySelectionToExcel(int copyrole=Qt::EditRole, bool storeHead=true, bool flipped=false);
        void copySelectionToExcelNoHead(int copyrole=Qt::EditRole);
        void copySelectionToMatlabNoHead(int copyrole=Qt::EditRole, bool flipped=false);

        void copySelectionToExcelExpanded(int copyrole=Qt::EditRole, bool storeHead=true, bool flipped=false);
        void copySelectionToMatlabExpandedNoHead(int copyrole=Qt::EditRole, bool flipped=false);


        void copySelectionAsValueErrorToExcel(int valuerole=Qt::EditRole, int errorrole=Qt::EditRole, bool storeHead=true, Qt::Orientation orientation=Qt::Horizontal);
        void copySelectionAsMedianQuantilesToExcel(int medianrole=Qt::EditRole, int q25role=-1, int q75role=-1, bool storeHead=true, Qt::Orientation orientation=Qt::Horizontal);
    protected:
        virtual void keyPressEvent(QKeyEvent* event);
        void paint(QPainter &painter, double scale, int page, double hhh, double vhw, const QList<int>& pageCols, const QList<int>& pageRows, QPrinter* p=NULL);

        QPrinter* getPrinter(QPrinter* printerIn=NULL, bool *localPrinter=NULL);
    protected slots:
        void changeHTMLFontSize(QTextDocument* txt, double fontSize);
    private:
        QAction* actCopyExcel;
        QAction* actCopyExcelNoHead;
        QAction* actCopyMatlab;
        QAction* actPrint;
        QAction* actCopyImage;
        QAction* actSave;
        QAction* actSaveExtended;
        QAction* actSaveFlipped;
        QAction* actSaveExtendedFlipped;
        QMenu* menuSave;
        bool noCopyShortcut;
};

#endif // QENHANCEDTABLEVIEW_H
