/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFTEXTDOCUMENTPRINTPREVIEW_H
#define QFTEXTDOCUMENTPRINTPREVIEW_H

#include <QDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QTextDocument>
#include <QPrintPreviewWidget>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QActionGroup>
#include <QToolBar>
#include <QToolButton>
#include <QAction>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QIntValidator>
#include "lib_imexport.h"


class QFLIB_EXPORT QFTextDocumentPrintPreview : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFTextDocumentPrintPreview(QPrinter *printer, QTextDocument *txt, QWidget *parent=NULL, bool hasFontSizeEdit=false);
        ~QFTextDocumentPrintPreview();
    public slots:
        void print();

    protected slots:
        void on_preview_paintRequested(QPrinter* printer);
        void changeFontSize(double size);
        void zoomFactorChanged();
        void navigate(QAction* action);
        void fit(QAction* action);
        void setMode(QAction* action);
        void pageSetup();
        void zoomIn();
        void zoomOut();
        void updateNavActions();
        void updatePageNumLabel();
        void updateZoomFactor();
    signals:
        void setDocumentFontSize(QTextDocument* txt, double fontSize);

    private:
        QPrinter * printer;
        QTextDocument* txt;
        QPrintPreviewWidget* preview;
        QDialogButtonBox* buttons;
        QVBoxLayout* mainLayout;
        QComboBox* zoomFactor;
        QLineEdit *zoomEditor;

        void initActions();
        void setFitting(bool on);
        bool isFitting();

        // actions:
        QActionGroup* navGroup;
        QAction *nextPageAction;
        QAction *prevPageAction;
        QAction *firstPageAction;
        QAction *lastPageAction;

        QActionGroup* fitGroup;
        QAction *fitWidthAction;
        QAction *fitPageAction;

        QActionGroup* zoomGroup;
        QAction *zoomInAction;
        QAction *zoomOutAction;

        QActionGroup* orientationGroup;
        QAction *portraitAction;
        QAction *landscapeAction;

        QActionGroup* modeGroup;
        QAction *singleModeAction;
        QAction *facingModeAction;
        QAction *overviewModeAction;

        QActionGroup *printerGroup;
        QAction *printAction;
        QAction *pageSetupAction;
};

#endif // QFTEXTDOCUMENTPRINTPREVIEW_H
