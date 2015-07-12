#include "qcompleterplaintextedit.h"

QCompleterPlainTextEdit::QCompleterPlainTextEdit(QWidget* parent):
    QPlainTextEdit(parent), c(0)
{
    //ctor
    appendContents=true;
//    singleLine=false;
}

QCompleterPlainTextEdit::~QCompleterPlainTextEdit()
{
    //dtor
}

 void QCompleterPlainTextEdit::setCompleter(QCompleter *completer)
 {
     if (c)
         QObject::disconnect(c, 0, this, 0);

     c = completer;

     if (!c)
         return;

     c->setWidget(this);
     c->setCompletionMode(QCompleter::PopupCompletion);
     c->setCaseSensitivity(Qt::CaseInsensitive);
     QObject::connect(c, SIGNAL(activated(const QString&)),
                      this, SLOT(insertCompletion(const QString&)));
 }


  void QCompleterPlainTextEdit::insertCompletion(const QString& completion)
 {
     if (c->widget() != this)
         return;
     QTextCursor tc = textCursor();
     int extra = completion.length() - c->completionPrefix().length();
     tc.movePosition(QTextCursor::Left);
     tc.movePosition(QTextCursor::EndOfWord);
     tc.insertText(completion.right(extra));
     setTextCursor(tc);
 }

 QString QCompleterPlainTextEdit::textUnderCursor() const
 {
     QTextCursor tc = textCursor();
     tc.select(QTextCursor::WordUnderCursor);
     return tc.selectedText();
 }

 void QCompleterPlainTextEdit::focusInEvent(QFocusEvent *e)
 {
     if (c)
         c->setWidget(this);
     QPlainTextEdit::focusInEvent(e);
 }

void QCompleterPlainTextEdit::focusOutEvent(QFocusEvent* event) {
    if (completer() && appendContents) {
        int r=completer()->model()->rowCount();
        QStringList c=this->toPlainText().split("\n");
        completer()->model()->insertRows(r, c.size());
        for (int i=0; i<c.size(); i++) {
            completer()->model()->setData(completer()->model()->index(r+i,0), c[i].trimmed());
        }
    }
    QPlainTextEdit::focusOutEvent(event);
}


 void QCompleterPlainTextEdit::keyPressEvent(QKeyEvent *e)
 {
     if (c && c->popup()->isVisible()) {
         // The following keys are forwarded by the completer to the widget
        switch (e->key()) {
        case Qt::Key_Tab:
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Backtab:
             e->ignore();
             return; // let the completer do default behavior
        default:
            break;
        }
     }/* else if (singleLine) {
        switch (e->key()) {
            case Qt::Key_Tab:
            case Qt::Key_Enter:
            case Qt::Key_Return:
                focusNextChild();
                break;
        }
     }*/

     bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
     if (!c || !isShortcut) // dont process the shortcut when we have a completer
         QPlainTextEdit::keyPressEvent(e);

     const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
     if (!c || (ctrlOrShift && e->text().isEmpty()))
         return;

     static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
     bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
     QString completionPrefix = textUnderCursor();

     if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 1
                       || eow.contains(e->text().right(1)))) {
         c->popup()->hide();
         return;
     }

     if (completionPrefix != c->completionPrefix()) {
         c->setCompletionPrefix(completionPrefix);
         c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
     }
     QRect cr = cursorRect();
     cr.setWidth(c->popup()->sizeHintForColumn(0)
                 + c->popup()->verticalScrollBar()->sizeHint().width());
     c->complete(cr); // popup it up!
 }


