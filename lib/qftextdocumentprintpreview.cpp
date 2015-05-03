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

#include "qftextdocumentprintpreview.h"
#include <QPageSetupDialog>
#include <QCoreApplication>
#include <QTextBlock>


static inline void qt_setupActionIcon(QAction *action, const QLatin1String &name)
{
    QLatin1String imagePrefix(":/trolltech/dialogs/qprintpreviewdialog/images/");
    QIcon icon;
    icon.addFile(imagePrefix + name + QLatin1String("-24.png"), QSize(24, 24));
    icon.addFile(imagePrefix + name + QLatin1String("-32.png"), QSize(32, 32));
    action->setIcon(icon);
}

class ZoomFactorValidator : public QDoubleValidator
{
public:
    ZoomFactorValidator(QObject* parent)
        : QDoubleValidator(parent) {}
    ZoomFactorValidator(qreal bottom, qreal top, int decimals, QObject *parent)
        : QDoubleValidator(bottom, top, decimals, parent) {}

    State validate(QString &input, int &pos) const
    {
        bool replacePercent = false;
        if (input.endsWith(QLatin1Char('%'))) {
            input = input.left(input.length() - 1);
            replacePercent = true;
        }
        State state = QDoubleValidator::validate(input, pos);
        if (replacePercent)
            input += QLatin1Char('%');
        const int num_size = 4;
        if (state == Intermediate) {
            int i = input.indexOf(QLocale::system().decimalPoint());
            if ((i == -1 && input.size() > num_size)
                || (i != -1 && i > num_size))
                return Invalid;
        }
        return state;
    }
};


QFTextDocumentPrintPreview::QFTextDocumentPrintPreview(QPrinter * printer, QTextDocument* txt, QWidget *parent, bool hasFontSizeEdit) :
    QDialog(parent)
{


    this->printer=printer;
    this->txt=txt;
    mainLayout=new QVBoxLayout();
    setLayout(mainLayout);
    if (hasFontSizeEdit) {
        QToolBar* toolbar1=new QToolBar("tb1", this);
        mainLayout->addWidget(toolbar1, 1);

        QDoubleSpinBox* spinFSize=new QDoubleSpinBox(this);
        spinFSize->setRange(1,256);
        spinFSize->setValue(7);
        toolbar1->addWidget(new QLabel(tr("font size: ")));
        toolbar1->addWidget(spinFSize);
        connect(spinFSize, SIGNAL(valueChanged(double)), this, SLOT(changeFontSize(double)));
    }
    QToolBar* toolbar=new QToolBar("tb", this);
    mainLayout->addWidget(toolbar, 1);
    preview=new QPrintPreviewWidget(printer, this);
    connect(preview, SIGNAL(paintRequested(QPrinter*)), this, SLOT(on_preview_paintRequested(QPrinter*)));
    mainLayout->addWidget(preview, 1);
    buttons=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    mainLayout->addWidget(buttons);
    connect(txt, SIGNAL(contentsChanged()), preview, SLOT(updatePreview()));

    initActions();


    zoomFactor = new QComboBox;
    zoomFactor->setEditable(true);
    zoomFactor->setMinimumContentsLength(7);
    zoomFactor->setInsertPolicy(QComboBox::NoInsert);

    zoomEditor = new QLineEdit;
    zoomEditor->setValidator(new ZoomFactorValidator((qreal)1.0,(qreal)1000.0, 1, zoomEditor));
    zoomFactor->setLineEdit(zoomEditor);
    static const short factorsX2[] = { 25, 50, 100, 200, 250, 300, 400, 800, 1600 };
    for (int i = 0; i < int(sizeof(factorsX2) / sizeof(factorsX2[0])); ++i)
        zoomFactor->addItem(tr("%1%").arg(factorsX2[i] / 2.0));
    QObject::connect(zoomFactor->lineEdit(), SIGNAL(editingFinished()), this, SLOT(zoomFactorChanged()));
    QObject::connect(zoomFactor, SIGNAL(currentIndexChanged(int)), this, SLOT(zoomFactorChanged()));

    toolbar->addAction(fitWidthAction);
    toolbar->addAction(fitPageAction);
    toolbar->addSeparator();
    toolbar->addWidget(zoomFactor);
    toolbar->addAction(zoomOutAction);
    toolbar->addAction(zoomInAction);
    toolbar->addSeparator();
    toolbar->addAction(portraitAction);
    toolbar->addAction(landscapeAction);
    toolbar->addSeparator();
    toolbar->addAction(firstPageAction);
    toolbar->addAction(prevPageAction);
    toolbar->addSeparator();
    toolbar->addAction(singleModeAction);
    toolbar->addAction(facingModeAction);
    toolbar->addAction(overviewModeAction);
    toolbar->addSeparator();
    toolbar->addAction(pageSetupAction);
    setWindowFlags(windowFlags()|Qt::WindowMinMaxButtonsHint);
}



QFTextDocumentPrintPreview::~QFTextDocumentPrintPreview()
{

}

void QFTextDocumentPrintPreview::on_preview_paintRequested(QPrinter *printer)
{
    if (txt) {
        txt->print(printer);
    }
}

void QFTextDocumentPrintPreview::changeFontSize(double size)
{
    emit setDocumentFontSize(txt, size);
}

void QFTextDocumentPrintPreview::zoomFactorChanged()
{
    QString text = zoomFactor->lineEdit()->text();
    bool ok;
    qreal factor = text.remove(QLatin1Char('%')).toFloat(&ok);
    factor = qMax(qreal(1.0), qMin(qreal(1000.0), factor));
    if (ok) {
        preview->setZoomFactor(factor/100.0);
        zoomFactor->setEditText(QString::fromLatin1("%1%").arg(factor));
        setFitting(false);
    }
}

void QFTextDocumentPrintPreview::navigate(QAction *action)
{
    int curPage = preview->currentPage();
    if (action == prevPageAction)
        preview->setCurrentPage(curPage - 1);
    else if (action == nextPageAction)
        preview->setCurrentPage(curPage + 1);
    else if (action == firstPageAction)
        preview->setCurrentPage(1);
    else if (action == lastPageAction)
        preview->setCurrentPage(preview->pageCount());
    updateNavActions();
}

void QFTextDocumentPrintPreview::fit(QAction * action)
{
    setFitting(true);
    if (action == fitPageAction)
        preview->fitInView();
    else
        preview->fitToWidth();
}

void QFTextDocumentPrintPreview::setMode(QAction * action)
{
    if (action == overviewModeAction) {
        preview->setViewMode(QPrintPreviewWidget::AllPagesView);
        setFitting(false);
        fitGroup->setEnabled(false);
        navGroup->setEnabled(false);
        //pageNumEdit->setEnabled(false);
        //pageNumLabel->setEnabled(false);
    } else if (action == facingModeAction) {
        preview->setViewMode(QPrintPreviewWidget::FacingPagesView);
    } else {
        preview->setViewMode(QPrintPreviewWidget::SinglePageView);
    }
    if (action == facingModeAction || action == singleModeAction) {
        fitGroup->setEnabled(true);
        navGroup->setEnabled(true);
        //pageNumEdit->setEnabled(true);
        //pageNumLabel->setEnabled(true);
        setFitting(true);
    }
}

void QFTextDocumentPrintPreview::pageSetup()
{

    QPageSetupDialog pageSetup(printer, this);
    if (pageSetup.exec() == QDialog::Accepted) {
        // update possible orientation changes
        if (preview->orientation() == QPrinter::Portrait) {
            portraitAction->setChecked(true);
            preview->setPortraitOrientation();
        }else {
            landscapeAction->setChecked(true);
            preview->setLandscapeOrientation();
        }
    }
}

void QFTextDocumentPrintPreview::zoomIn()
{
    setFitting(false);
    preview->zoomIn();
    updateZoomFactor();
}

void QFTextDocumentPrintPreview::zoomOut()
{
    setFitting(false);
    preview->zoomOut();
    updateZoomFactor();
}

void QFTextDocumentPrintPreview::print()
{
    preview->print();
}

void QFTextDocumentPrintPreview::initActions()
{
    // Navigation
    navGroup = new QActionGroup(this);
    navGroup->setExclusive(false);
    nextPageAction = navGroup->addAction(QCoreApplication::translate("QPrintPreviewDialog", "Next page"));
    prevPageAction = navGroup->addAction(QCoreApplication::translate("QPrintPreviewDialog", "Previous page"));
    firstPageAction = navGroup->addAction(QCoreApplication::translate("QPrintPreviewDialog", "First page"));
    lastPageAction = navGroup->addAction(QCoreApplication::translate("QPrintPreviewDialog", "Last page"));
    qt_setupActionIcon(nextPageAction, QLatin1String("go-next"));
    qt_setupActionIcon(prevPageAction, QLatin1String("go-previous"));
    qt_setupActionIcon(firstPageAction, QLatin1String("go-first"));
    qt_setupActionIcon(lastPageAction, QLatin1String("go-last"));
    QObject::connect(navGroup, SIGNAL(triggered(QAction*)), this, SLOT(navigate(QAction*)));


    fitGroup = new QActionGroup(this);
    fitWidthAction = fitGroup->addAction(QCoreApplication::translate("QPrintPreviewDialog", "Fit width"));
    fitPageAction = fitGroup->addAction(QCoreApplication::translate("QPrintPreviewDialog", "Fit page"));
    fitWidthAction->setObjectName(QLatin1String("fitWidthAction"));
    fitPageAction->setObjectName(QLatin1String("fitPageAction"));
    fitWidthAction->setCheckable(true);
    fitPageAction->setCheckable(true);
    qt_setupActionIcon(fitWidthAction, QLatin1String("fit-width"));
    qt_setupActionIcon(fitPageAction, QLatin1String("fit-page"));
    QObject::connect(fitGroup, SIGNAL(triggered(QAction*)), this, SLOT(fit(QAction*)));

    // Zoom
    zoomGroup = new QActionGroup(this);
    zoomInAction = zoomGroup->addAction(QCoreApplication::translate("QPrintPreviewDialog", "Zoom in"));
    zoomOutAction = zoomGroup->addAction(QCoreApplication::translate("QPrintPreviewDialog", "Zoom out"));
    qt_setupActionIcon(zoomInAction, QLatin1String("zoom-in"));
    qt_setupActionIcon(zoomOutAction, QLatin1String("zoom-out"));
    QObject::connect(zoomInAction, SIGNAL(triggered(bool)), this, SLOT(zoomIn()));
    QObject::connect(zoomOutAction, SIGNAL(triggered(bool)), this, SLOT(zoomOut()));

    // Portrait/Landscape
    orientationGroup = new QActionGroup(this);
    portraitAction = orientationGroup->addAction(QCoreApplication::translate("QPrintPreviewDialog", "Portrait"));
    landscapeAction = orientationGroup->addAction(QCoreApplication::translate("QPrintPreviewDialog", "Landscape"));
    portraitAction->setCheckable(true);
    landscapeAction->setCheckable(true);
    qt_setupActionIcon(portraitAction, QLatin1String("layout-portrait"));
    qt_setupActionIcon(landscapeAction, QLatin1String("layout-landscape"));
    QObject::connect(portraitAction, SIGNAL(triggered(bool)), preview, SLOT(setPortraitOrientation()));
    QObject::connect(landscapeAction, SIGNAL(triggered(bool)), preview, SLOT(setLandscapeOrientation()));

    // Display mode
    modeGroup = new QActionGroup(this);
    singleModeAction = modeGroup->addAction(QCoreApplication::translate("QPrintPreviewDialog", "Show single page"));
    facingModeAction = modeGroup->addAction(QCoreApplication::translate("QPrintPreviewDialog", "Show facing pages"));
    overviewModeAction = modeGroup->addAction(QCoreApplication::translate("QPrintPreviewDialog", "Show overview of all pages"));
    qt_setupActionIcon(singleModeAction, QLatin1String("view-page-one"));
    qt_setupActionIcon(facingModeAction, QLatin1String("view-page-sided"));
    qt_setupActionIcon(overviewModeAction, QLatin1String("view-page-multi"));
    singleModeAction->setObjectName(QLatin1String("singleModeAction"));
    facingModeAction->setObjectName(QLatin1String("facingModeAction"));
    overviewModeAction->setObjectName(QLatin1String("overviewModeAction"));

    singleModeAction->setCheckable(true);
    facingModeAction->setCheckable(true);
    overviewModeAction->setCheckable(true);
    QObject::connect(modeGroup, SIGNAL(triggered(QAction*)), this, SLOT(setMode(QAction*)));

    // Print
    printerGroup = new QActionGroup(this);
    printAction = printerGroup->addAction(QCoreApplication::translate("QPrintPreviewDialog", "Print"));
    pageSetupAction = printerGroup->addAction(QCoreApplication::translate("QPrintPreviewDialog", "Page setup"));
    qt_setupActionIcon(printAction, QLatin1String("print"));
    qt_setupActionIcon(pageSetupAction, QLatin1String("page-setup"));
    QObject::connect(printAction, SIGNAL(triggered(bool)), this, SLOT(print()));
    QObject::connect(pageSetupAction, SIGNAL(triggered(bool)), this, SLOT(pageSetup()));

    // Initial state:
    fitPageAction->setChecked(true);
    singleModeAction->setChecked(true);
    if (preview->orientation() == QPrinter::Portrait)
        portraitAction->setChecked(true);
    else
        landscapeAction->setChecked(true);
}


void QFTextDocumentPrintPreview::updateNavActions()
{
    int curPage = preview->currentPage();
    int numPages = preview->pageCount();
    nextPageAction->setEnabled(curPage < numPages);
    prevPageAction->setEnabled(curPage > 1);
    firstPageAction->setEnabled(curPage > 1);
    lastPageAction->setEnabled(curPage < numPages);
    //pageNumEdit->setText(QString::number(curPage));
}

void QFTextDocumentPrintPreview::updatePageNumLabel()
{

    /*int numPages = preview->pageCount();
    int maxChars = QString::number(numPages).length();
    pageNumLabel->setText(QString::fromLatin1("/ %1").arg(numPages));
    int cyphersWidth = fontMetrics().width(QString().fill(QLatin1Char('8'), maxChars));
    int maxWidth = pageNumEdit->minimumSizeHint().width() + cyphersWidth;
    pageNumEdit->setMinimumWidth(maxWidth);
    pageNumEdit->setMaximumWidth(maxWidth);
    pageNumEdit->setValidator(new QIntValidator(1, numPages, pageNumEdit));*/
    // any old one will be deleted later along with its parent pageNumEdit
}

void QFTextDocumentPrintPreview::updateZoomFactor()
{
    zoomFactor->lineEdit()->setText(QString().sprintf("%.1f%%", preview->zoomFactor()*100));
}


void QFTextDocumentPrintPreview::setFitting(bool on)
{
    if (isFitting() == on)
        return;
    fitGroup->setExclusive(on);
    if (on) {
        QAction* action = fitWidthAction->isChecked() ? fitWidthAction : fitPageAction;
        action->setChecked(true);
        if (fitGroup->checkedAction() != action) {
            // work around exclusitivity problem
            fitGroup->removeAction(action);
            fitGroup->addAction(action);
        }
    } else {
        fitWidthAction->setChecked(false);
        fitPageAction->setChecked(false);
    }
}


bool QFTextDocumentPrintPreview::isFitting()
{
    return (fitGroup->isExclusive()
            && (fitWidthAction->isChecked() || fitPageAction->isChecked()));
}
