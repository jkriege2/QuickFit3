/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QENHANCEDTABLEVIEW_H
#define QENHANCEDTABLEVIEW_H

#include "lib_imexport.h"
#include <QTableView>
#include <QTextDocument>
#include <QMenu>
#include <QToolBar>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrinterInfo>

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
        void getVariantDataTableValueError(int copyrole, int errorrole, QList<QList<QVariant> >& data, QStringList& colnames, QStringList& rownames, const QString& copyPrefix=QString(""), const QString& errorPrefix=QString("error: ")) const;
        void getVariantDataTableMedianQuantiles(int medianrole, int q25role, int q75role, QList<QList<QVariant> >& data, QStringList& colnames, QStringList& rownames, const QString& medianPrefix=QString("median: "), const QString& q25Prefix=QString("Q25%: "), const QString& q75Prefix=QString("Q75%: ")) const;

    signals:
        void keyPressed(int key, Qt::KeyboardModifiers modifiers, QString text);
    public slots:
        void print();
        void save(int copyrole=Qt::EditRole, bool flipped=false, bool storeHead=true);
        void saveExtended(int copyrole=Qt::EditRole, bool flipped=false, bool storeHead=true);
        void saveFlipped(int copyrole=Qt::EditRole);
        void saveExtendedFlipped(int copyrole=Qt::EditRole);
        void copyAsImage();
        void saveAsImage();
        void copySelectionToExcel(int copyrole=Qt::EditRole, bool storeHead=true, bool flipped=false);
        void copySelectionToExcelNoHead(int copyrole=Qt::EditRole);
        void copySelectionToCSV(int copyrole=Qt::EditRole, bool storeHead=true, bool flipped=false);
        void copySelectionToCSVNoHead(int copyrole=Qt::EditRole);
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
        QAction* actCopyCSV;
        QAction* actCopyCSVNoHead;
        QAction* actCopyMatlab;
        QAction* actPrint;
        QAction* actCopyImage;
        QAction* actSaveImage;
        QAction* actSave;
        QAction* actSaveExtended;
        QAction* actSaveFlipped;
        QAction* actSaveExtendedFlipped;
        QMenu* menuSave;
        QMenu* menuCopyCSV;
        QMenu* menuCopyExcel;
        QMenu* menuCopyMatlab;
        bool noCopyShortcut;
};

#endif // QENHANCEDTABLEVIEW_H
