#include "qfevaluationeditor.h"
#include "qfevaluationitem.h"
#include "qfevaluationpropertyeditor.h"
#include <QCloseEvent>
#include <QDebug>

QFEvaluationEditor::QFEvaluationEditor(QFPluginServices* services, QFEvaluationPropertyEditor *propEditor, QWidget *parent):
    QWidget(parent)
{
    current=NULL;
    this->propertyEditor=propEditor;
    peID=0;
    this->services=services;
    actSaveReport=new QAction(QIcon(":/lib/savereport.png"), tr("Save &Report"), this);
    actSaveReport->setToolTip(tr("save the evaluation results as a report in PDF or PostScript format"));
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
        disconnect(current, SIGNAL(resultsChanged()), this, SLOT(resultsChanged()));
    }
    current=c;
    this->peID=peID;
    //std::cout<<"connecting widgets ... \n";
    connectWidgets(current, old);
    //std::cout<<"connecting widgets ... done\n";
    connect(c, SIGNAL(resultsChanged()), this, SLOT(resultsChanged()));
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
    /* it is often a good idea to have a possibility to save or print a report about the fit results.
       This is implemented in a generic way here.    */

    QString currentSaveDirectory="";
    if (settings && current) settings->getQSettings()->value(QString("%2/evaluationeditor%1/last_report_directory").arg(peID).arg(current->getType()), currentSaveDirectory).toString();

    QString fn = QFileDialog::getSaveFileName(this, tr("Save Report"),
                                currentSaveDirectory,
                                tr("PDF File (*.pdf);;PostScript File (*.ps)"));

    if (!fn.isEmpty()) {
        currentSaveDirectory=QFileInfo(fn).absolutePath();
        if (settings && current) settings->getQSettings()->setValue(QString("%2/evaluationeditor%1/last_report_directory").arg(peID).arg(current->getType()), currentSaveDirectory);
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        QFileInfo fi(fn);
        QPrinter* printer=new QPrinter();
        printer->setPaperSize(QPrinter::A4);
        printer->setPageMargins(15,15,15,15,QPrinter::Millimeter);
        printer->setOrientation(QPrinter::Portrait);
        printer->setOutputFormat(QPrinter::PdfFormat);
        if (fi.suffix().toLower()=="ps") printer->setOutputFormat(QPrinter::PostScriptFormat);
        printer->setOutputFileName(fn);
        QTextDocument* doc=new QTextDocument();
        doc->setTextWidth(printer->pageRect().size().width());
        createReportDoc(doc);
        doc->print(printer);
        delete doc;
        delete printer;
        QApplication::restoreOverrideCursor();
    }
}

void QFEvaluationEditor::printReport() {
    /* it is often a good idea to have a possibility to save or print a report about the fit results.
       This is implemented in a generic way here.    */
    QPrinter* p=new QPrinter();

    p->setPageMargins(15,15,15,15,QPrinter::Millimeter);
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
