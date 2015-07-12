#include "testmain.h"
#include <QtGui>

#include "jkstarratingwidget.h"
#include "qenhancedlineedit.h"
#include "qspecialtoolbutton.h"
#include "jkstyledbutton.h"
#include "jkcharacterscreenkeyboard.h"
#include "qmodernprogresswidget.h"
#include <QStandardItemModel>

TestMain::TestMain(QWidget* parent):
    QMainWindow(parent)
{
    progress=0;
    prg_up=true;

    tabMain=new QTabWidget(this);
    setCentralWidget(tabMain);

    QWidget* w=new QWidget(this);
    JKStarRatingWidget* rating=new JKStarRatingWidget(w);
    rating->move(10,10);

    tabMain->addTab(w, "JKStarRatingWidget");

    w=new QWidget(this);
    QFormLayout* l=new QFormLayout(w);
    w->setLayout(l);
    l->addRow("<b>with special tool buttons:</b>", new QWidget());
    QEnhancedLineEdit* le=new QEnhancedLineEdit(w);
    QFileSelectToolButton* b1=new QFileSelectToolButton(w);
    b1->setBuddy(le);
    le->addButton(b1);
    QFileExecuteToolButton* b2=new QFileExecuteToolButton(w);
    b2->setBuddy(le);
    le->addButton(b2);
    l->addRow("&File:", le);

    le=new QEnhancedLineEdit(w);
    QWWWToolButton* b3=new QWWWToolButton(w);
    b3->setBuddy(le);
    le->addButton(b3);
    l->addRow("&Webpage:", le);

    l->addRow("<b>Now with JKStyledButton:</b>", new QWidget());

    le=new QEnhancedLineEdit(w);
    QIcon icNetwork(":/jkstyledbutton/network.png");
    icNetwork.addFile(":/jkstyledbutton/network_disabled.png", QSize(), QIcon::Disabled);
    icNetwork.addFile(":/jkstyledbutton/network_hover.png", QSize(), QIcon::Active);
    icNetwork.addFile(":/jkstyledbutton/network_pressed.png", QSize(), QIcon::Selected);
    JKStyledButton* b4=new JKStyledButton(icNetwork, le);
    //b4->setBuddy(le);
    le->addButton(b4);
    l->addRow("&Webpage, no function:", le);

    le=new QEnhancedLineEdit(w);
    JKStyledButton* b5=new JKStyledButton(icNetwork, le);
    //b5->setBuddy(le);
    b5->setEnabled(false);
    le->addButton(b5);
    l->addRow("&Webpage, disabled Icon:", le);

    le=new QEnhancedLineEdit(w);
    JKStyledButton* b6=new JKStyledButton(JKStyledButton::OpenURL, le, le);
    le->addButton(b6);
    le->setText("http://www.jkrieger.de/");
    l->addRow("&Webpage, action:", le);


    le=new QEnhancedLineEdit(w);
    b6=new JKStyledButton(JKStyledButton::SelectFile, le, le);
    le->addButton(b6);
    b6=new JKStyledButton(JKStyledButton::ExecuteFile, le, le);
    le->addButton(b6);
    le->setText("litsoz3widgets_test.exe");
    l->addRow("&File:", le);

    tabMain->addTab(w, "QEnhancedLineEdit");


    JKCharacterScreenKeyboard* jkkeyb=new JKCharacterScreenKeyboard(0);
    jkkeyb->show();
    jkkeyb->setFocusPolicy(Qt::NoFocus);
    jkkeyb->setKBFocusWidget(this);
    jkkeyb->setCharacters("ABCÄÖÜäöüß");



    w=new QWidget(this);
    QGridLayout* gl=new QGridLayout(w);
    w->setLayout(gl);
    QModernProgressWidget* prg1=new QModernProgressWidget(w);
    prg1->setSpin(true);
    prg1->setMode(QModernProgressWidget::GradientRing);
    gl->addWidget(prg1, 0,0);

    prg1=new QModernProgressWidget(w);
    prg1->setSpin(true);
    prg1->setMode(QModernProgressWidget::Circles);
    gl->addWidget(prg1, 0,1);
    prg1=new QModernProgressWidget(w);
    prg1->setSpin(true);
    prg1->setMode(QModernProgressWidget::Strokes);
    gl->addWidget(prg1, 1,0);

    prg2=new QModernProgressWidget(w);
    prg2->setMode(QModernProgressWidget::GradientRing);
    prg2->setDisplayPercent(true);
    prg2->setStartColor(prg2->stopColor());
    gl->addWidget(prg2, 2,0);

    prg3=new QModernProgressWidget(w);
    prg3->setMode(QModernProgressWidget::Circles);
    prg3->setDisplayPercent(true);
    prg3->setStartColor(prg3->stopColor());
    gl->addWidget(prg3, 2,1);

    prg4=new QModernProgressWidget(w);
    prg4->setMode(QModernProgressWidget::Strokes);
    prg4->setDisplayPercent(true);
    prg4->setStartColor(prg4->stopColor());
    gl->addWidget(prg4, 3,0);

    tabMain->addTab(w, tr("modern progress"));

    tabMain->setCurrentIndex(2);

    dlg=new QModernProgressDialog("Test label text <b> with some bold</b> text<br>&nbsp;&nbsp;also in different lines!", "Cancel", NULL);
    dlg->progressWidget()->setPercentageMode(QModernProgressWidget::Value);
    dlg->progressWidget()->setSuffix("°C");
    dlg->progressWidget()->setPrecision(1);
    dlg->setWindowModality(Qt::NonModal);
    connect(dlg, SIGNAL(canceled()), dlg, SLOT(hide()));

    QTimer::singleShot(100, this, SLOT(incProgress()));

    textEdit=new QTextEdit(this);
    tabMain->addTab(textEdit, tr("QTextEdit"));
    // SETUP TEXT ////////////////////////////////////////////////////////////////////////
    QObject *svgInterface = new QSvgTextObject;
    int SvgTextFormat=QTextFormat::UserObject + 1;
    textEdit->document()->documentLayout()->registerHandler(SvgTextFormat, svgInterface);
    insertSVG(textEdit->textCursor(), SvgTextFormat, QString(":/test.svg"));

    QObject *picInterface = new QPictureTextObject;
    int PicTextFormat=QTextFormat::UserObject + 2;
    textEdit->document()->documentLayout()->registerHandler(PicTextFormat, picInterface);
    QPicture pic;
    QPainter* p=new QPainter(&pic);
    p->drawRect(5,5,10,10);
    p->drawText(10,10, "Hello World!");
    p->drawRect(0,0,100,10);
    delete p;
    insertQPicture(textEdit->textCursor(), PicTextFormat, pic);
    insertQPicture(textEdit->textCursor(), PicTextFormat, pic, QSizeF(100, 20));
    insertQPicture(textEdit->textCursor(), PicTextFormat, pic, QSizeF(200, 40));
    insertQPicture(textEdit->textCursor(), PicTextFormat, pic);


    QPrinter* printer=new QPrinter;
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName("test_out.pdf");
    textEdit->print(printer);
    delete printer;
    // SETUP TEXT ////////////////////////////////////////////////////////////////////////

    QEnhancedTableView* table=new QEnhancedTableView(this);
    tabMain->addTab(table, tr("enhanced tableview"));
    QStandardItemModel* model=new QStandardItemModel(this);

    for (int r=0; r<100; r++) {
        QList<QStandardItem*> row;
        for (int c=0; c<55; c++) {

            if (c==r) {
                QStandardItem* it=new QStandardItem(tr("long it at position (%1, %2)").arg(r).arg(c));
                it->setCheckable(true);
                it->setCheckState((c%2)?Qt::Checked:Qt::Unchecked);
                row.append(it);
            } else {
                QStandardItem* it=new QStandardItem(tr("(%1, %2)").arg(r).arg(c));
                if (abs(c-r)==2) it->setBackground(QColor("red"));
                if (abs(c-r)==3) it->setBackground(QColor("lightblue"));
                if (abs(c-r)==1) it->setBackground(QColor("lightgreen"));
                row.append(it);
            }

        }
        model->appendRow(row);
    }

    table->setModel(model);
    table->resizeColumnsToContents();
    printer=new QPrinter;
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName("table_test.pdf");
    table->print(printer);
    delete printer;

    printer=new QPrinter;
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName("table_onepagewide.pdf");
    table->print(printer, true, false);
    delete printer;

    printer=new QPrinter;
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName("table_onepagehigh.pdf");
    table->print(printer, false, true);
    delete printer;

    printer=new QPrinter;
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName("table_onepage.pdf");
    table->print(printer, true, true);
    delete printer;

    QPicture tablePic;
    QPainter picPaint(&tablePic);
    table->paint(picPaint, QRect(0,0,500,300));
    picPaint.end();

    QObject *pictureInterface = new QPictureTextObject;
    int PictureTextFormat=QTextFormat::UserObject + 2;
    textEdit->document()->documentLayout()->registerHandler(PictureTextFormat, pictureInterface);
    insertQPicture(textEdit->textCursor(), PictureTextFormat, tablePic, tablePic.boundingRect().size());
    printer=new QPrinter;
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName("textwithtable.pdf");
    textEdit->print(printer);
    delete printer;


    tabMain->setCurrentIndex(tabMain->count()-1);
}

TestMain::~TestMain()
{
    //dtor
}

void TestMain::incProgress() {
    if (prg_up) progress++;
    else progress--;
    if (progress>100) { progress=100; prg_up=false; }
    if (progress<0) { progress=0; prg_up=true; }
    prg2->setValue(progress);
    prg3->setValue(100-progress);
    prg4->setValue(progress);
    dlg->setValue((double)progress*0.37);
    QTimer::singleShot(50, this, SLOT(incProgress()));
}

void TestMain::showEvent(QShowEvent* event) {
    dlg->openDelayed();
}
