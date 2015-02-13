/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#include "qfevaluationeditor.h"
#include "qfevaluationitem.h"
#include "qfevaluationpropertyeditor.h"
#include <QCloseEvent>
#include <QDebug>
#include <QProgressDialog>
#if (QT_VERSION > QT_VERSION_CHECK(5, 2, 0))
#  include <QPdfWriter>
#endif

QFEvaluationEditor::QFEvaluationEditor(QFPluginServices* services, QFEvaluationPropertyEditor *propEditor, QWidget *parent):
    QWidget(parent)
{
    current=NULL;
    this->propertyEditor=propEditor;
    peID=0;
    this->services=services;
    actSaveReport=new QAction(QIcon(":/lib/savereport.png"), tr("Save &Report"), this);
    actSaveReport->setToolTip(tr("save the evaluation results as a report in PDF, HTML, ... format"));
    connect(actSaveReport, SIGNAL(triggered()), this, SLOT(saveReport()));
    actPrintReport=new QAction(QIcon(":/lib/printreport.png"), tr("&Print Report"), this);
    actPrintReport->setToolTip(tr("print the evaluation results as a report"));
    connect(actPrintReport, SIGNAL(triggered()), this, SLOT(printReport()));
}

QFEvaluationEditor::~QFEvaluationEditor()
{
}

void QFEvaluationEditor::setCurrent(QFEvaluationItem* c, int peID) {
    if (current) saveSettings();
    QFEvaluationItem* old=current;
    if (current) {
        disconnect(current, SIGNAL(resultsChanged(QFRawDataRecord*,QString,QString)), this, SLOT(resultsChanged(QFRawDataRecord*,QString,QString)));
    }
    current=c;
    this->peID=peID;
    //std::cout<<"connecting widgets ... \n";
    connectWidgets(current, old);
    //std::cout<<"connecting widgets ... done\n";
    connect(c, SIGNAL(resultsChanged(QFRawDataRecord*,QString,QString)), this, SLOT(resultsChanged(QFRawDataRecord*,QString,QString)));
}

void QFEvaluationEditor::sendEditorCommand(const QString &command, const QVariant &param1, const QVariant &param2, const QVariant &param3, const QVariant &param4, const QVariant &param5)
{
}

void QFEvaluationEditor::closeEvent( QCloseEvent * event ) {
    if (settings) writeSettings();
    event->accept();
}

bool QFEvaluationEditor::event(QEvent * ev) {
    QTime t;
    t.start();
    //qDebug()<<"~~~ QFEvaluationEditor::event("<<ev->type()<<")";
    bool ok=QWidget::event(ev);
    //qDebug()<<"~~~ QFEvaluationEditor::event("<<ev->type()<<") done: "<<t.elapsed()<<" ms";
    return ok;
}


void QFEvaluationEditor::saveReport() {

    QTextDocument* doc=new QTextDocument();
    /*QPrinter* printer=new QPrinter();//QPrinter::HighResolution);
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setPaperSize(QPrinter::A4);
    printer->setPageMargins(10,10,10,10,QPrinter::Millimeter);
    printer->setOrientation(QPrinter::Portrait);
    printer->setColorMode(QPrinter::Color);
    doc->setTextWidth(printer->pageRect().size().width());*/
    createReportDoc(doc);
    qfSaveReport(doc, QString("QuickFit %1 Help: %2 Report").arg(qfInfoVersionFull()).arg(current->getName()), QString("%2/evaluationeditor%1/").arg(peID).arg(current->getType()), this);
    delete doc;

    /* it is often a good idea to have a possibility to save or print a report about the fit results.
       This is implemented in a generic way here.    */
    /*
    QString currentSaveDirectory="";
    if (settings && current) currentSaveDirectory=settings->getQSettings()->value(QString("%2/evaluationeditor%1/last_report_directory").arg(peID).arg(current->getType()), currentSaveDirectory).toString();

    QStringList filters;
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        filters<<tr("PDF File (*.pdf)");
    #else
        filters<<tr("PDF File (*.pdf)");
        filters<<tr("PostScript File (*.ps)");
    #endif
    int firstDForm=filters.size();
    QList<QByteArray> dforms=QTextDocumentWriter::supportedDocumentFormats();
    for (int i=0; i<dforms.size(); i++) {
        filters<<QString("%1 File (*.%2)").arg(QString::fromLatin1(dforms[i])).arg(QString::fromLatin1(dforms[i].toLower()));
    }
    QString selFilter="";
    QString fn = qfGetSaveFileNameSet(QString("%2/evaluationeditor%1/").arg(peID).arg(current->getType()), this, tr("Save Report"),
                                currentSaveDirectory,
                                filters.join(";;"), &selFilter);
    int filterID=filters.indexOf(selFilter);

    if (!fn.isEmpty()) {
        settings->getQSettings()->setValue(QString("%2/evaluationeditor%1/last_report_directory").arg(peID).arg(current->getType()), currentSaveDirectory);
        currentSaveDirectory=QFileInfo(fn).absolutePath();
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        QProgressDialog progress(tr("Exporting ..."), "", 0, 100, NULL);
        progress.setWindowModality(Qt::WindowModal);
        //progress.setHasCancel(false);
        progress.setLabelText(tr("saving report <br> to '%1' ...").arg(fn));
        progress.setValue(50);
        progress.show();

        QFileInfo fi(fn);        
        QTextDocument* doc=new QTextDocument();
        QPrinter* printer=new QPrinter();//QPrinter::HighResolution);
        printer->setOutputFormat(QPrinter::PdfFormat);
        printer->setPaperSize(QPrinter::A4);
        printer->setPageMargins(10,10,10,10,QPrinter::Millimeter);
        printer->setOrientation(QPrinter::Portrait);
        printer->setColorMode(QPrinter::Color);
        doc->setTextWidth(printer->pageRect().size().width());
        createReportDoc(doc);
        if (filterID==0
        #if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
            ||filterID==1
        #endif
            ) {
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
            if (filterID==1) printer->setOutputFormat(QPrinter::PostScriptFormat);
#endif
#if (QT_VERSION > QT_VERSION_CHECK(5, 2, 0))
            QPdfWriter pdf(fn);
            pdf.setPageMargins(QMarginsF(10,10,10,10),QPageLayout::Millimeter);
            pdf.setPageOrientation(QPageLayout::Portrait);
            pdf.setCreator(QString("QuickFit %1").arg(qfInfoVersionFull()));
            pdf.setPageSize(QPageSize(QPageSize::A4));
            pdf.setTitle(QString("QuickFit %1 Help: %2 Report").arg(qfInfoVersionFull()).arg(current->getName()));
            pdf.setResolution(300);
            doc->print(&pdf);
#else
            printer->setColorMode(QPrinter::Color);
            printer->setOutputFileName(fn);
            doc->print(printer);
#endif
        } else if (filterID>firstDForm) {
            QTextDocumentWriter writer(fn, dforms.value(filterID-firstDForm, "HTML"));
            writer.write(doc);
        }
        //qDebug()<<doc->toHtml();
        delete doc;
        delete printer;
        progress.accept();
        QApplication::restoreOverrideCursor();
    }
    */
}

void QFEvaluationEditor::printReport() {
    /* it is often a good idea to have a possibility to save or print a report about the fit results.
       This is implemented in a generic way here.    */
    QPrinter* p=new QPrinter();

    p->setPageMargins(10,10,10,10,QPrinter::Millimeter);
    p->setOrientation(QPrinter::Portrait);
    QPrintDialog *dialog = new QPrintDialog(p, this);
    dialog->setWindowTitle(tr("Print Report"));
    if (dialog->exec() != QDialog::Accepted) {
        delete p;
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QTextDocument* doc=new QTextDocument();
    doc->setTextWidth(p->pageRect().size().width());
    createReportDoc(doc);
    doc->print(p);
    delete p;
    delete doc;
    QApplication::restoreOverrideCursor();
}

void QFEvaluationEditor::createReportDoc(QTextDocument *doc) {
    doc->setHtml(tr("<center>THIS PLUGIN DOES NOT CREATE REPORTS!</center>"));
}
