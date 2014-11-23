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


/*
  Name: CompleterTextEditWidget.cpp
  Copyright: (c) 2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/
*/


#include "qfcompletertextedit.h"

#include <QCompleter>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QtDebug>
#include <QApplication>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QScrollBar>
#include <QMessageBox>
#include <QPainter>
#include <QToolTip>





QFCompleterTextEditNumberBar::QFCompleterTextEditNumberBar(QTextEdit* edtr, QWidget* parent):
    QWidget(parent)
{
    markerWidth=16;
    linenumberColor=QColor("black");
    linenumberFont.setFamily("Courier New");
    linenumberFont.setPointSize(10);
    linenumberFont.setFixedPitch(true);
    linenumberColumnColor=QColor("gainsboro");
    markerColumnColor=QColor("darkgray");
    errorMarkerColor=QColor("red");
    warningMarkerColor=QColor("green");
    infoMarkerColor=QColor("blue");
    setEditor(edtr);
    setFixedWidth(fontMetrics().width(QString("0000") + markerWidth+4));
}

QFCompleterTextEditNumberBar::~QFCompleterTextEditNumberBar() {
}

void QFCompleterTextEditNumberBar::readSettings(QSettings& settings) {
    markerWidth=settings.value("editor/marker_width", 16).toInt();
    linenumberFont.setPointSize(settings.value("editor/linenumber_fontsize", 10).toInt());
    linenumberFont.setFamily(settings.value("editor/linenumber_fontname", "Courier New").toString());
    linenumberColor=QColor(settings.value("editor/linenumber_color", "black").toString());
    linenumberColumnColor=QColor(settings.value("editor/linenumber_column_color", "gainsboro").toString());
    markerColumnColor=QColor(settings.value("editor/marker_column_color", "gray").toString());
    errorMarkerColor=QColor(settings.value("editor/errormarker_color", "red").toString());
    warningMarkerColor=QColor(settings.value("editor/warningmarker_color", "green").toString());
    infoMarkerColor=QColor(settings.value("editor/infomarker_color", "blue").toString());
}

void QFCompleterTextEditNumberBar::addMarker(int line, itemType type, QString message) {
    itemData d;
    d.line=line;
    d.message=message;
    d.type=type;
    d.rect=QRect(0,0,0,0);
    markers.insert(line, d);
    update();
}

void QFCompleterTextEditNumberBar::setEditor(QTextEdit* edtr) {
    editor=edtr;
    disconnect(0, 0, this, SLOT(update()));
    connect(editor->document()->documentLayout(), SIGNAL(update(const QRectF &)), this, SLOT(update()));
    connect(editor->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(update()));
}

bool QFCompleterTextEditNumberBar::event(QEvent *event) {
    if (event->type() == QEvent::ToolTip) {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        // now we have to check whether any marker is at this position
        int item=-1;
        bool found=false;
        QMapIterator<int, itemData> i(markers);
        while (i.hasNext() && !found) {
            i.next();
            itemData d=i.value();
            if (d.rect.contains(helpEvent->pos())) {
                item=d.line;
            }
        }

        if (item>-1)
            QToolTip::showText(helpEvent->globalPos(), markers[item].message);
        else
            QToolTip::hideText();
    }
    return QWidget::event(event);
}

void QFCompleterTextEditNumberBar::paintEvent(QPaintEvent *event) {
    QTextDocument* doc=editor->document();
    QAbstractTextDocumentLayout *layout = doc->documentLayout();
    qreal yPosition=editor->verticalScrollBar()->value();
    qreal vpHeight=editor->viewport()->height();

    QPainter p(this);
    p.setFont(linenumberFont);
    int linenumberWidth=QString::number(doc->blockCount()).size()*p.fontMetrics().width("0")+4;
    int markerheight=p.fontMetrics().ascent()+2;

    // set the width of the widget
    setFixedWidth(linenumberWidth+markerWidth+2);

    // first we draw the background
    p.setBrush(QBrush(linenumberColumnColor));
    p.setPen(QPen(linenumberColumnColor));
    p.drawRect(0,0,width(),vpHeight);
    p.setPen(QPen(markerColumnColor));
    p.setBrush(QBrush(markerColumnColor));
    p.drawRect(linenumberWidth,0,width()-linenumberWidth,vpHeight);

    // reset the rect of all markers
    QMutableMapIterator<int, itemData> i(markers);
    while (i.hasNext()) {
        i.next();
        itemData d=i.value();
        d.rect=QRect(0,0,0,0);
        i.setValue(d);
    }

    // now we draw the line numbers
    p.setPen(QPen(linenumberColor));
    for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next()) {
        QRectF brect=layout->blockBoundingRect(it);
        qreal bottompos=brect.y()+brect.height();
        markerheight=brect.height()-8;
        // we end this loop if the current block lies below the viewport
        if (brect.y() > yPosition+ vpHeight)
            break;
        // if we are inside the viewport, we have to paint a line number for this line
        if (bottompos >= yPosition) {
            QString txt = QString::number(it.blockNumber()+1);
            p.drawText(1, brect.y()-yPosition, linenumberWidth-2, brect.height(), Qt::AlignRight|Qt::AlignVCenter, txt);
            if (markers.contains(it.blockNumber()+1)) {
                itemData d=markers[it.blockNumber()+1];
                QRect markerrect=QRect(linenumberWidth+2, brect.y()-yPosition+4, width()-linenumberWidth-4, markerheight);
                markers[it.blockNumber()+1].rect=markerrect;
                if (d.type==mtInfo) {
                    //p.drawImage(linenumberWidth+2, brect.y()-yPosition, QIcon(":/event_info.png"));
                    p.setBrush(infoMarkerColor);
                    QPen pe=p.pen();
                    pe.setColor(QColor("black"));
                    pe.setCosmetic(true);
                    pe.setWidth(1);
                    p.setPen(pe);
                    p.drawRect(markerrect);
                } else if (d.type==mtError) {
                    //p.drawImage(linenumberWidth+2, brect.y()-yPosition, QIcon(":/event_error.png"));
                    p.setBrush(errorMarkerColor);
                    QPen pe=p.pen();
                    pe.setColor(QColor("black"));
                    pe.setCosmetic(true);
                    pe.setWidth(1);
                    p.setPen(pe);
                    p.drawRect(markerrect);
                } else {
                    //p.drawImage(linenumberWidth+2, brect.y()-yPosition, QIcon(":/event_warning.png"));
                    p.setBrush(warningMarkerColor);
                    QPen pe=p.pen();
                    pe.setColor(QColor("black"));
                    pe.setCosmetic(true);
                    pe.setWidth(1);
                    p.setPen(pe);
                    p.drawRect(markerrect);
                }
            }
        }
    }

}










QFCompleterTextEditWidget::QFCompleterTextEditWidget(QWidget *parent)
    : QTextEdit(parent), c(0)
{
    commentString="//";
    currentlySearching=false;
    currentlyReplacing=false;
    searchPhrase="";
    replacePhrase="";
    searchFromStart=false;
    matchCase=false;
    wholeWords=false;
    replaceAll=false;
    askBeforeReplace=false;
    tabwidth=2;
    setLineWrapMode(QTextEdit::NoWrap);
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(cursorChanged()));
}

QFCompleterTextEditWidget::~QFCompleterTextEditWidget()
{
}

void QFCompleterTextEditWidget::setCompleter(QCompleter *completer) {
     if (c)
         QObject::disconnect(c, 0, this, 0);

     c = completer;

     if (!c)
         return;

     c->setWidget(this);
     c->setCompletionMode(QCompleter::PopupCompletion);
     c->setCaseSensitivity(Qt::CaseInsensitive);
     QObject::connect(c, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)));
 }

 QCompleter *QFCompleterTextEditWidget::completer() const
 {
     return c;
 }

 void QFCompleterTextEditWidget::insertCompletion(const QString& completion1)
 {
     if (c->widget() != this)
         return;
     QString completion=completion1;
     completion=completion.remove(QChar('°'),  Qt::CaseInsensitive);
     QTextCursor tc = textCursor();
     int extra = completion.length() - c->completionPrefix().length();
     tc.movePosition(QTextCursor::Left);
     tc.movePosition(QTextCursor::EndOfWord);
     tc.insertText(completion.right(extra));

     // handle special character '°' in insertted text completion1
     unsigned char mcount=completion1.count(QChar('°'), Qt::CaseInsensitive);
     int p1=completion1.indexOf(QChar('°'), Qt::CaseInsensitive);
     int p2=completion1.indexOf(QChar('°'), p1+1, Qt::CaseInsensitive);
     if (mcount==1) {
         tc.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, completion.size()-p1);
     } else if (mcount>1) {
         tc.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, completion.size()-p2+1);
         tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, p2-p1-1);
     }
     setTextCursor(tc);
     //tc.insertText(QString("  %1:  %2--%3  ").arg(mcount).arg(p1).arg(p2));
 }

 QString QFCompleterTextEditWidget::textUnderCursor() const
 {
     QTextCursor tc = textCursor();
     tc.select(QTextCursor::WordUnderCursor);
     return tc.selectedText();
 }

 QString QFCompleterTextEditWidget::validTextUnderCursor() const
 {
     QTextCursor tc = textCursor();
     QTextCursor t1=tc;
     t1.select(QTextCursor::WordUnderCursor);
     QString w=t1.selectedText().trimmed();
     //std::cout<<"sel "<<w.toAscii().data()<<std::endl;
     QString cutl("1234567890*+!-?&!§$%&/()=?}][{\\~#'.:,;<>^");
     QString cutlb("*+!-?&!§$%&/()=?}][{\\~#'.:,;<>^");
     while (w.size()>0 && cutl.contains(w[0])) {
         w=w.remove(0,1);
         //std::cout<<"rf  "<<w.toAscii().data()<<std::endl;
     }
     while (w.size()>0 && cutlb.contains(w[w.size()-1])) {
         w=w.remove(w.size()-1,1);
         //std::cout<<"rb  "<<w.toAscii().data()<<std::endl;
     }
     int bn=tc.blockNumber();
     while (tc.blockNumber()==bn && !validwords.contains(w) &&  tc.position()>0) {
        if (!tc.movePosition(QTextCursor::Left)) break;
        t1=tc;
        t1.select(QTextCursor::WordUnderCursor);
        w=t1.selectedText().trimmed();
        //std::cout<<w.toAscii().data()<<std::endl;
         while (w.size()>0 && cutl.contains(w[0])) {
             w=w.remove(0,1);
             //std::cout<<"rf  "<<w.toAscii().data()<<std::endl;
         }
         while (w.size()>0 && cutlb.contains(w[w.size()-1])) {
             w=w.remove(w.size()-1,1);
             //std::cout<<"rb  "<<w.toAscii().data()<<std::endl;
         }
     }
     if (validwords.contains(w)) return w;
     else return "";
 }

 void QFCompleterTextEditWidget::focusInEvent(QFocusEvent *e)
 {
     if (c)
         c->setWidget(this);
     QTextEdit::focusInEvent(e);
 }

 void QFCompleterTextEditWidget::gotoLine(int line)
 {
    QTextCursor c(textCursor());
    c.beginEditBlock();
    c.movePosition(QTextCursor::Start);
    if (line>1)
        c.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line-1);
    c.endEditBlock();
    setTextCursor(c);
 }

void QFCompleterTextEditWidget::cursorChanged() {
    emit cursorMoved(getLineNumber(), getColumnNumber());
    QString s=validTextUnderCursor().trimmed();
    //std::cout<<s.toAscii().data()<<std::endl;
    if (currentTextUnderCursor!=s) {
        currentTextUnderCursor=s;
        emit wordUnderCursorChanged(s);
    }

    /*std::cout<<"document()->documentLayout()->documentSize() = "<<document()->documentLayout()->documentSize().width()<<"   "<<document()->documentLayout()->documentSize().height()<<std::endl;
    std::cout<<"verticalScrollBar()->value() = "<<verticalScrollBar()->value()<<std::endl;
    std::cout<<"verticalScrollBar()->min  max = "<<verticalScrollBar()->minimum()<<"  "<<verticalScrollBar()->maximum()<<std::endl;
    std::cout<<"viewport()->height() = "<<viewport()->height()<<std::endl;
    std::cout<<"viewport()->width() = "<<viewport()->width()<<std::endl;*/
}

void QFCompleterTextEditWidget::processTab(QKeyEvent *e) {
    QTextCursor c(textCursor());
    if (c.selectionStart() == c.selectionEnd()) {
        c.insertText(tabToSpaces()); // no selection: simply insert tab spaces
        setTextCursor(c);
    } else {
        indentInc();
    }
}

 void QFCompleterTextEditWidget::keyPressEvent(QKeyEvent *event) {
     if (c && c->popup()->isVisible()) {
         // The following keys are forwarded by the completer to the widget
        switch (event->key()) {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Escape:
            case Qt::Key_Tab:
            case Qt::Key_Backtab:
                 event->ignore();
                 return; // completer widgets handles these keys!
            default:
                break;
        }
     } else {
        // if the completer is not visible, we may treat some special
        // key press events (convert TAB to spaces ...
        if (event->key()==Qt::Key_Tab) { // convert TAB to <tabwidth> spaces
            processTab(event);
            return; // let the completer do default behavior
        } else if (event->key()==Qt::Key_Backtab || (event->key()==Qt::Key_Tab && event->modifiers()==Qt::ShiftModifier)) {
            indentDec();
            return; // let the completer do default behavior
        } else if (event->key()==Qt::Key_Return || event->key()==Qt::Key_Enter) {
            // indent when last non-space character in current line is '{'
            QTextCursor tc = textCursor();

            // if text has been selected, the user wants to overwrite this text
            // with a linebreak, so we first delete the text
            if (tc.hasSelection()) tc.deleteChar();
            //tc.select(QTextCursor::LineUnderCursor);
            tc.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
            QString line=tc.selectedText();

            // count leading spaces
            int lspaces=0;
            QChar* data=line.data();
            while (data->isSpace()) {
                ++data;
                lspaces++;
            }
            // get right-most non-space character
            int indx=line.size()-1;
            data=line.data();
            if (indx>=0) while (data[indx].isSpace() && (indx>0)) {
                indx--;
            }
            QChar last=data[indx];

            if (last=='{') {
                QTextCursor c(textCursor());
                c.insertText("\n"+QString(lspaces, QChar(' '))+tabToSpaces());
                setTextCursor(c);
            } else if (last=='}' && line.indexOf('}')==lspaces) {
                // '}' is the first non-space character in this line
                QTextCursor c(textCursor());
                QTextCursor c1(textCursor());
                c1.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
                QString left=c1.selectedText();
                if (lspaces>0 && left=="}") {
                    long clspaces=lspaces; // the number of leading spaces in the current line
                    long indention=qMax((long)0, (long)lspaces-(long)tabwidth); // new number of leading spaces
                    // here we try to find the matching '{' and reduce the indention to the
                    // indention of the matching '{'. If the matching '{' was not found,
                    // the indention will not be reduced
                    QTextCursor cc(textCursor());
                    int cnt=1;
                    int pos=cc.selectionStart()-2;
                    while ((cnt>0) && (pos>=0)) {
                        cc.setPosition(pos);
                        cc.setPosition(pos+1, QTextCursor::KeepAnchor);
                        QString curChar=cc.selectedText();
                        if (curChar=="{") cnt--;
                        if (curChar=="}") cnt++;
                        //std::cout<<"'"<<(char*)curChar.toAscii().data()<<"'  cnt="<<cnt<<"  pos="<<pos<<std::endl;
                        pos--;
                    }
                    // here we found the matching '{' and count its leading spaces
                    if (pos>=0){
                        cc.select(QTextCursor::LineUnderCursor);
                        lspaces=0;
                        QChar* data=cc.selectedText().data();
                        while (data->isSpace()) {
                            ++data;
                            lspaces++;
                        }
                        indention=lspaces;
                    }
                    //std::cout<<"indention="<<indention<<"   clspaces="<<clspaces<<"    lspaces="<<lspaces<<std::endl;
                    c=textCursor();
                    c.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
                    c.insertText(QString(indention, QChar(' '))+line.right(qMax((long)1,(long)line.size()-clspaces))+"\n"+QString(indention, QChar(' ')));
                    //c.movePosition(QTextCursor::Left);
                    //c.insertText("\n"+QString(lspaces, QChar(' ')));
                } else {
                    c=textCursor();
                    c.insertText("\n");
                }
                setTextCursor(c);
            } else {
                QTextCursor c(textCursor());
                c.insertText("\n"+QString(lspaces, QChar(' ')));
                setTextCursor(c);
            }
            return;
        }
     }


     bool isShortcut = ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_E); // CTRL+E
     if (!c || !isShortcut) // dont process the shortcut when we have a completer
         QTextEdit::keyPressEvent(event);

     const bool ctrlOrShift = event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
     if (!c || (ctrlOrShift && event->text().isEmpty()))
         return;

     static QString eow("~!@#$%^&*()+{}|:\"<>?,./;'[]\\-="); // end of word
     bool hasModifier = (event->modifiers() != Qt::NoModifier) && !ctrlOrShift;
     QString completionPrefix = textUnderCursor();
     /*QMessageBox::information(this, "", tr("completionPrefix=%1\nisShortcut=%2\nctrlOrShif=%3\nhasModifier=%4")
                                            .arg(completionPrefix)
                                            .arg(isShortcut)
                                            .arg(ctrlOrShift)
                                            .arg(hasModifier)
                                            );*/

     if (!isShortcut && (hasModifier || event->text().isEmpty()|| completionPrefix.length() < 2
                       || eow.contains(event->text().right(1)))) {
         c->popup()->hide();
         return;
     }
     /*QMessageBox::information(this, "", tr("c->completionPrefix=%1\ncompletionPrefix=%2")
                                            .arg(c->completionPrefix())
                                            .arg(completionPrefix)
                                            );*/
     if (completionPrefix != c->completionPrefix()) {
         c->setCompletionPrefix(completionPrefix);
         /*QMessageBox::information(this, "", tr("c->completionModel()->rowCount()=%1")
                                            .arg(c->completionModel()->rowCount())
                                            );*/
         c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
     }
     QRect cr = cursorRect();
     cr.setWidth(c->popup()->sizeHintForColumn(0)
                 + c->popup()->verticalScrollBar()->sizeHint().width());
     c->complete(cr); // popup it up!
     /*QMessageBox::information(this, "", tr("cr.width=%1\ncr.height=%2\ncr.x=%3\ncr.y=%4")
                                            .arg(cr.width())
                                            .arg(cr.height())
                                            .arg(cr.x())
                                            .arg(cr.y())
                                            );*/
}

void QFCompleterTextEditWidget::indentInc(){
    QTextCursor c(textCursor());
    if (c.selectionStart() == c.selectionEnd())
        c.select(QTextCursor::LineUnderCursor);
    if (c.selectionStart() != c.selectionEnd()) {
        // now we have to iterate through all selected blocks (lines) and indent them
        QTextCursor c1(c);
        c1.setPosition(c.selectionStart());
        QTextCursor c2(c);
        c2.setPosition(c.selectionEnd());
        c1.beginEditBlock();
        while (c1.blockNumber() <= c2.blockNumber()) {
            c1.movePosition(QTextCursor::StartOfBlock);
            c1.insertText(tabToSpaces());
            if (!c1.movePosition(QTextCursor::NextBlock))
                break;
        }
        c1.endEditBlock();
        setTextCursor(c);
    }
}

void QFCompleterTextEditWidget::indentDec(){
    QTextCursor c(textCursor());
    if (c.selectionStart() == c.selectionEnd())
        c.select(QTextCursor::LineUnderCursor);
    if (c.selectionStart() != c.selectionEnd()) {
        // now we have to iterate through all selected blocks (lines) and indent them
        QTextCursor c1(c);
        c1.setPosition(c.selectionStart());
        QTextCursor c2(c);
        c2.setPosition(c.selectionEnd());
        c1.beginEditBlock();
        while (c1.blockNumber() <= c2.blockNumber()) {
            c1.movePosition(QTextCursor::StartOfBlock);
            QString line=c1.block().text();
            if (line[0]=='\t') { c1.deleteChar(); }
            else {
                unsigned int count=0;
                while (count<((tabwidth==0)?4:tabwidth) && line[0].isSpace()) {
                    c1.deleteChar();
                    line=c1.block().text();
                    count++;
                }
            }
            if (!c1.movePosition(QTextCursor::NextBlock))
                break;
        }
        c1.endEditBlock();
    }
}

void QFCompleterTextEditWidget::comment(){
    QTextCursor c(textCursor());
    if (c.selectionStart() == c.selectionEnd()) {
        c.movePosition(QTextCursor::StartOfLine);
        c.insertText(commentString+" ");
        //setTextCursor(c);
    } else {
        // now we have to iterate through all selected blocks (lines) and indent them
        QTextCursor c1(c);
        c1.setPosition(c.selectionStart());
        QTextCursor c2(c);
        c2.setPosition(c.selectionEnd());
        c1.beginEditBlock();
        while (c1.blockNumber() <= c2.blockNumber()) {
            c1.movePosition(QTextCursor::StartOfBlock);
            c1.insertText(commentString+" ");
            if (!c1.movePosition(QTextCursor::NextBlock))
                break;
        }
        c1.endEditBlock();
        setTextCursor(c);
    }
}

QString QFCompleterTextEditWidget::removeComments(QString t) {
    int pos1=0;
    int i=0;
    while (i<t.size() && t[i].isSpace()) {
        //std::cout<<i<<"/"<<t.size()<<":  '"<<t[i].toAscii()<<"'"<<std::endl;
        i++;
    }
    QString t1=t.right(t.size()-i);
    if (t1.startsWith(commentString+" ")) {
        return t.left(i)+t1.right(t1.size()-(commentString.size()+1));
    } else if  (t1.startsWith(commentString)) {
        return t.left(i)+t1.right(t1.size()-commentString.size());
    } else {
        return t;
    }
}

void QFCompleterTextEditWidget::uncomment(){
    QTextCursor c(textCursor());
    if (c.selectionStart() == c.selectionEnd()) {
        c.select(QTextCursor::LineUnderCursor);
        c.insertText(removeComments(c.selectedText()));
        //setTextCursor(c);
    } else {
        // now we have to iterate through all selected blocks (lines) and indent them
        QTextCursor c1(c);
        c1.setPosition(c.selectionStart());
        QTextCursor c2(c);
        c2.setPosition(c.selectionEnd());
        c1.beginEditBlock();
        while (c1.blockNumber() <= c2.blockNumber()) {
            c1.select(QTextCursor::BlockUnderCursor);
            //std::cout<<"'"<<c1.selectedText().toAscii().data()<<"'  =>  '"<<removeComments(c1.selectedText()).toAscii().data()<<"'"<<std::endl;
            c1.insertText(removeComments(c1.selectedText()));
            if (!c1.movePosition(QTextCursor::NextBlock))
                break;
        }
        c1.endEditBlock();
        setTextCursor(c);
    }
}

bool QFCompleterTextEditWidget::findFirst(QString phrase, bool searchFromStart, bool matchCase, bool wholeWords){
    this->searchFromStart=searchFromStart;
    this->matchCase=matchCase;
    this->wholeWords=wholeWords;
    this->searchPhrase=phrase;
    currentlySearching=true;
    currentlyReplacing=false;
    // move cursor to home position of first line
    QTextCursor c(textCursor());
    c.clearSelection();
    if (searchFromStart) {
        c.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
        while (!c.atStart()) {
            c.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor);
        }
    }
    setTextCursor(c);

    // find the next occurence
    QTextDocument::FindFlags flags=0;
    if (matchCase) flags |= QTextDocument::FindCaseSensitively;
    if (wholeWords) flags |= QTextDocument::FindWholeWords;
    if (find(phrase, flags)) {
        emit findNextAvailable(true);
        return true;
    }else {
        emit findNextAvailable(false);
        return false;
    }
}

bool QFCompleterTextEditWidget::findNext(){
    if (!currentlySearching) return false;

    // find the next occurence
    QTextDocument::FindFlags flags=0;
    if (matchCase) flags |= QTextDocument::FindCaseSensitively;
    if (wholeWords) flags |= QTextDocument::FindWholeWords;
    if (find(searchPhrase, flags)) {
        emit findNextAvailable(true);
        return true;
    }else {
        emit findNextAvailable(false);
        return false;
    }
}

bool QFCompleterTextEditWidget::replaceFirst(QString phrase, QString replaceBy, bool searchFromStart, bool matchCase, bool wholeWords, bool replaceAll, bool askBeforeReplace){
    this->searchFromStart=searchFromStart;
    this->matchCase=matchCase;
    this->wholeWords=wholeWords;
    this->searchPhrase=phrase;
    this->replacePhrase=replaceBy;
    this->replaceAll=replaceAll;
    this->askBeforeReplace=askBeforeReplace;
    currentlySearching=false;
    currentlyReplacing=true;

    emit findNextAvailable(true);


    QTextCursor c(textCursor());
    c.clearSelection();
    if (searchFromStart) {
        c.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
        while (!c.atStart()) {
            c.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor);
        }
    }
    setTextCursor(c);

    // find the next occurence
    QTextDocument::FindFlags flags=0;
    if (matchCase) flags |= QTextDocument::FindCaseSensitively;
    if (wholeWords) flags |= QTextDocument::FindWholeWords;
    if (replaceAll) {
        long count=0;
        bool stopped=false;
        while (find(phrase, flags) && !stopped) {
            if (askBeforeReplace) {
                QMessageBox::StandardButton ret;
                ret = QMessageBox::question(this, tr("Find & Replace ..."),
                             tr("Replace this occurence?"),
                             QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
                if (ret == QMessageBox::Cancel) {
                    stopped=true;
                } else if (ret == QMessageBox::Yes) {
                    QTextCursor c(textCursor());
                    c.insertText(replacePhrase);
                    setTextCursor(c);
                    count++;
                }

            } else {
                QTextCursor c(textCursor());
                c.insertText(replacePhrase);
                setTextCursor(c);
                count++;
            }
        }
        if (count>0) QMessageBox::information(this, tr("Find & Replace ..."),
                             tr("Replaced %1 occurences of '%2' ...")
                             .arg(count)
                             .arg(searchPhrase));
        emit findNextAvailable(false);
        return (count>0);
    } else {
        if (!find(searchPhrase, flags)) {
            /*QMessageBox::information(this, tr("Find & Replace ..."),
                             tr("Did not find '%1' ...")
                             .arg(searchPhrase));*/
            emit findNextAvailable(false);
            return false;
        } else {
            if (askBeforeReplace) {
                QMessageBox::StandardButton ret;
                ret = QMessageBox::question(this, tr("Find & Replace ..."),
                             tr("Replace this occurence?"),
                             QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
                if (ret == QMessageBox::Cancel) {
                    emit findNextAvailable(false);
                    return true;
                } else if (ret == QMessageBox::Yes) {
                    QTextCursor c(textCursor());
                    c.insertText(replacePhrase);
                    setTextCursor(c);
                    emit findNextAvailable(true);
                    return true;
                } else if (ret == QMessageBox::No) {
                    emit findNextAvailable(true);
                    return true;
                }

            } else {
                QTextCursor c(textCursor());
                c.insertText(replacePhrase);
                setTextCursor(c);
                emit findNextAvailable(true);
                return true;
            }
        }
    }
    emit findNextAvailable(false);
    return false;
}

bool QFCompleterTextEditWidget::replaceNext(){
    if (!currentlyReplacing) return false;


    QTextDocument::FindFlags flags=0;
    if (matchCase) flags |= QTextDocument::FindCaseSensitively;
    if (wholeWords) flags |= QTextDocument::FindWholeWords;
    if (!find(searchPhrase, flags)) {
        emit findNextAvailable(false);
        return false;
    } else {
        if (askBeforeReplace) {
            QMessageBox::StandardButton ret;
            ret = QMessageBox::question(this, tr("Find & Replace ..."),
                         tr("Replace this occurence?"),
                         QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            if (ret == QMessageBox::Cancel) {
                emit findNextAvailable(false);
                return false;
            } else if (ret == QMessageBox::Yes) {
                QTextCursor c(textCursor());
                c.insertText(replacePhrase);
                setTextCursor(c);
            }

        } else {
            QTextCursor c(textCursor());
            c.insertText(replacePhrase);
            setTextCursor(c);
        }
    }
    return true;
}









QFCompleterTextEdit::QFCompleterTextEdit(QWidget* parent):
    QWidget(parent)
{
    editor = new QFCompleterTextEditWidget(this);
    editor->setFrameStyle(QFrame::NoFrame);
    QFont f("Monospace");
    f.setStyleHint(QFont::TypeWriter);
    editor->setFontFamily(f.family());
    editor->setTabwidth(2);

    numbers = new QFCompleterTextEditNumberBar(editor, this);
    layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);
    layout->addWidget(numbers);
    layout->addWidget(editor);
}

void QFCompleterTextEdit::readSettings(QSettings& settings) {
    numbers->readSettings(settings);
    editor->setFontFamily(settings.value("editor/fontname", "Courier New").toString());
    editor->setFontPointSize(settings.value("editor/fontsize", 10).toDouble());
    editor->setTabwidth(settings.value("editor/tabwidth", 2).toInt());
    QFont f=editor->font();
    f.setFixedPitch(true);
    editor->setFont(f);
}
