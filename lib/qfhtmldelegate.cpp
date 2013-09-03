#include "qfhtmldelegate.h"
#include <QElapsedTimer>
#include <QDebug>
#include <QTextEdit>
#include <QVariant>
#include <QTextDocument>
#include <QPainter>
#include <QApplication>
#include <QBrush>
#include <QPixmapCache>
#include <QTime>
#include <QDate>
#include <QDateTime>

QFHTMLDelegate::QFHTMLDelegate(QObject* parent):
    QStyledItemDelegate(parent)
{
    m_displayRichTextEditor=true;
    rxHTML=QRegExp("(<\\s*\\w+.*>)|(<\\s*/\\s*\\w+\\s*>)|(\\&\\w+\\;)");
    rxHTML.setMinimal(true);
    m_printMode=false;
}

QFHTMLDelegate::~QFHTMLDelegate()
{
    //dtor
}

void QFHTMLDelegate::setPrintMode(bool enabled)
{
    m_printMode=enabled;
}

bool QFHTMLDelegate::getPrintMode() const
{
    return m_printMode;
}

bool QFHTMLDelegate::displayRichTextEditor() const {
    return m_displayRichTextEditor;
}

void QFHTMLDelegate::setDisplayRichTextEditor(bool d) {
    m_displayRichTextEditor=d;
}

void QFHTMLDelegate::drawCheck(QPainter *painter,
                              const QStyleOptionViewItem &option,
                              const QRect &rect, Qt::CheckState state) const
{
    if (!rect.isValid())
        return;

    if (!m_printMode) {
        QStyleOptionViewItem opt(option);
        opt.rect = rect;
        opt.state = opt.state & ~QStyle::State_HasFocus;

        switch (state) {
            case Qt::Unchecked:
                opt.state |= QStyle::State_Off;
                break;
            case Qt::PartiallyChecked:
                opt.state |= QStyle::State_NoChange;
                break;
            case Qt::Checked:
                opt.state |= QStyle::State_On;
                break;
        }
        QStyle *style = QApplication::style();
        style->drawPrimitive(QStyle::PE_IndicatorViewItemCheck, &opt, painter, NULL);
    } else {
        double lw=qMax(0.1, double(rect.width())/15.0);
        double offset=double(rect.width())/10.0;
        painter->save();
        QPen p=QPen(option.palette.color(QPalette::Dark));
        p.setWidthF(lw);
        QRectF r=QRectF(QPointF(rect.topLeft())+QPointF(lw+offset,lw+offset), QPointF(rect.bottomRight())-QPointF(lw+offset,lw+offset));
        painter->setPen(p);
        painter->setBrush(QBrush(option.palette.color(QPalette::QPalette::Base)));
        painter->drawRect(r);


        if (state==Qt::Checked) {
            p.setColor(option.palette.color(QPalette::WindowText));
            r=QRectF(QPointF(r.topLeft())+QPointF(lw,lw), QPointF(r.bottomRight())-QPointF(lw,lw));
            painter->setPen(p);
            painter->drawLine(r.topLeft(), r.bottomRight());
            painter->drawLine(r.topRight(), r.bottomLeft());
        }

        painter->restore();
    }
}

QString QFHTMLDelegate::calcDisplayedText(const QModelIndex &index, const QStyleOptionViewItem &option, QPoint& offset, bool& isHTML, QPixmap* imgOut) const {
    QVariant data=index.data(Qt::DisplayRole).toString();
    QVariant check=index.data(Qt::CheckStateRole);
    QPixmap img=QPixmap();
    QVariant iv=index.data(Qt::DecorationRole);
    if (iv.isValid() && iv.type()==QVariant::Icon) img=iv.value<QIcon>().pixmap(16);
    if (iv.isValid() && iv.type()==QVariant::Pixmap) img=iv.value<QPixmap>();
    offset=QPoint(2,0);
    //qDebug()<<option.palette.base()<<option.palette.background();
    if (check.isValid()) {
        offset=QPoint(option.rect.height()+2, 0);
    }

    if (!img.isNull()) {
        offset.setX(offset.x()+img.width()+2);
        if (imgOut) *imgOut=img;
    }
    isHTML=false;

    if (data.type()==QVariant::String || data.type()==QVariant::StringList) {
        if (data.toString().contains(rxHTML)) {
            QVariant fv=index.data(Qt::FontRole);
            QFont f=option.font;
            if (fv.isValid()) f=fv.value<QFont>();
            QString pre, post;
            pre="<font color=\""+option.palette.text().color().name()+"\">";
            post="</font>";
            if (option.state & QStyle::State_Selected) {
                pre="<font color=\""+option.palette.highlightedText().color().name()+"\">";
                post="</font>";
            }
            if (f.bold()) {
                pre=pre+"<b>";
                post="</b>"+post;
            }
            if (f.italic()) {
                pre=pre+"<i>";
                post="</i>"+post;
            }
            if (f.underline()) {
                pre=pre+"<u>";
                post="</u>"+post;
            }

            isHTML=true;
            return pre+data.toString()+post;
        } else {

            return data.toString();
        }
    } else if (data.type()==QVariant::String) {
        return data.toString();
    } else if (data.type()==QVariant::Double) {
        return QLocale::system().toString(data.toDouble(),'g', 6);
    } else if (data.type()==QVariant::UInt || data.type()==QVariant::ULongLong) {
        return QLocale::system().toString(data.toULongLong());
    } else if (data.type()==QVariant::Int || data.type()==QVariant::LongLong) {
        return QLocale::system().toString(data.toLongLong());
    } else if (data.type()==QVariant::Time) {
        return QLocale::system().toString(data.toTime());
    } else if (data.type()==QVariant::Date) {
        return QLocale::system().toString(data.toDate());
    } else if (data.type()==QVariant::DateTime) {
        return QLocale::system().toString(data.toDateTime());
    } else {
        return "";
    }
}


void QFHTMLDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QElapsedTimer et;
    et.start();
    QVariant data=index.data(Qt::DisplayRole).toString();
    //qDebug()<<"   get data dT="<<double(et.nsecsElapsed())/1000000.0<<"ms";
    QVariant check=index.data(Qt::CheckStateRole);
    //qDebug()<<"   get check dT="<<double(et.nsecsElapsed())/1000000.0<<"ms";
    QBrush back=index.data(Qt::BackgroundRole).value<QBrush>();
    //qDebug()<<"   get back dT="<<double(et.nsecsElapsed())/1000000.0<<"ms";
    QPoint offset=QPoint(2,0);
    //qDebug()<<option.palette.base()<<option.palette.background();
    if (index.data(Qt::BackgroundRole).isValid()) {
        //qDebug()<<"   fill back dT="<<double(et.nsecsElapsed())/1000000.0<<"ms";
        painter->fillRect(option.rect, back);
    }
    QPixmap img=QPixmap();
    bool isHTML=false;
    QString text=calcDisplayedText(index, option, offset, isHTML, &img);
    //qDebug()<<"   calcDisplayedText dT="<<double(et.nsecsElapsed())/1000000.0<<"ms";

    //qDebug()<<"    text="<<text<<" offset="<<offset<<" isHTML="<<isHTML<<" img-size="<<img.size();

    if (option.state & QStyle::State_Selected) {
        //qDebug()<<"   fill rect dT="<<double(et.nsecsElapsed())/1000000.0<<"ms";
        painter->fillRect(option.rect, option.palette.highlight());
    }
    if (check.isValid()) {
        //qDebug()<<"   draw check dT="<<double(et.nsecsElapsed())/1000000.0<<"ms";
        drawCheck(painter, option, QRect(option.rect.left(),option.rect.top(),option.rect.height(),option.rect.height()), check.toInt()!=0?Qt::Checked:Qt::Unchecked);
    }
    if (!img.isNull()) {
        //qDebug()<<"   draw pixmap dT="<<double(et.nsecsElapsed())/1000000.0<<"ms";
        painter->drawPixmap(option.rect.topLeft()+QPoint(2,0), img);
    }
    if (isHTML) {
        //qDebug()<<"   draw html dT="<<double(et.nsecsElapsed())/1000000.0<<"ms";
        painter->save();
        painter->translate(option.rect.topLeft()+offset);

        QRect r(QPoint(0, 0), option.rect.size()-QSize(offset.x(), 0));
        if (m_printMode) {
            QTextDocument doc;
            doc.setHtml(text);
            doc.drawContents(painter, r);
        } else {
            QString id=QString::number(r.width())+"_"+QString::number(r.height())+"_"+text;
            QPixmap pix;
            if (!QPixmapCache::find(id, &pix)) {
                //qDebug()<<"   draw html: create pixmap dT="<<double(et.nsecsElapsed())/1000000.0<<"ms";
                {
                    QImage img=QImage(r.width(), r.height(), QImage::Format_ARGB32_Premultiplied);
                    img.fill(Qt::transparent);
                    QPainter pixp;
                    pixp.begin(&img);
                    QTextDocument doc;
                    doc.setHtml(text);
                    doc.drawContents(&pixp, r);
                    pixp.end();
                    pix=QPixmap::fromImage(img);
                }
                /*bool ok=*/
                QPixmapCache::insert(id, pix);;
                //qDebug()<<" inserted pixmap: "<<ok<<"   cache_limit="<<QPixmapCache::cacheLimit();
            }
            painter->drawPixmap(0,0, pix);
        }

        painter->restore();

    } else if (!text.isEmpty()) {
        //qDebug()<<"   draw text dT="<<double(et.nsecsElapsed())/1000000.0<<"ms";
        painter->save();
        QRect r=option.rect;
        r.setLeft(r.left()+offset.x());
        QVariant fv=index.data(Qt::FontRole);
        QVariant fc=index.data(Qt::TextColorRole);
        QFont f=option.font;
        if (fv.isValid()) f=fv.value<QFont>();
        painter->setFont(f);
        QColor textcol=option.palette.text().color();
        if (fc.isValid()) textcol=fc.value<QColor>();
        painter->setPen(textcol);
        int flags=Qt::AlignLeft|Qt::AlignVCenter|Qt::TextSingleLine;
        if (option.textElideMode==Qt::ElideNone) {
            painter->drawText(r, flags, text);
        } else {
            QFontMetrics fm(f);
            QString txtEl=fm.elidedText(text, option.textElideMode, r.width(), flags);
            painter->drawText(r, flags, txtEl);
        }
        painter->restore();
    } else if (data.isValid() && !data.isNull() && !(text.isEmpty() && (data.type()==QVariant::String || QVariant::StringList))){
        //qDebug()<<"   !!!QStyledItemDelegate::paint!!! dT="<<double(et.nsecsElapsed())/1000000.0<<"ms";
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize QFHTMLDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    //qDebug()<<" QFHTMLDelegate::sizeHint()";
    QVariant data=index.data(Qt::DisplayRole).toString();

    bool isHTML=false;
    QPoint offset(0,0);
    QString text=calcDisplayedText(index, option, offset, isHTML);


    if (isHTML) {
        QTextDocument doc;
        doc.setHtml(data.toString());
        return doc.size().toSize();
    } else if (!text.isEmpty()){
        QVariant fv=index.data(Qt::FontRole);
        QFont f=option.font;
        if (fv.isValid()) f=fv.value<QFont>();
        QFontMetrics fm(f);
        return fm.size(Qt::TextSingleLine, text);
    } else if (data.type()==QVariant::String || data.type()==QVariant::StringList) {
        return QSize(0,0);
    } else if (!data.isValid() || data.isNull()) {
        return QSize(0,0);
    }
    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget *QFHTMLDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QVariant data=index.data(Qt::DisplayRole).toString();
    if (m_displayRichTextEditor&&(data.type()==QVariant::String)) {
        return new QTextEdit(parent);
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void QFHTMLDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QVariant data=index.data(Qt::DisplayRole).toString();
    if (m_displayRichTextEditor&&(data.type()==QVariant::String)) {
        QTextEdit *te=static_cast<QTextEdit*>(editor);
        te->setHtml(data.toString());
        return;
    }
    QStyledItemDelegate::setEditorData(editor, index);
}

void QFHTMLDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QTextEdit *te=qobject_cast<QTextEdit*>(editor);
    if (m_displayRichTextEditor&&te) {
        model->setData(index, te->toHtml());
        return ;
    }
    QStyledItemDelegate::setModelData(editor, model, index);
}

void QFHTMLDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QTextEdit *te=qobject_cast<QTextEdit*>(editor);
    QVariant check=index.data(Qt::CheckStateRole);
    QPoint offset=QPoint(0,0);
    if (check.isValid()) {
        offset=QPoint(option.rect.height()+2,0);
    }
    if (m_displayRichTextEditor&&te) {
        editor->setGeometry(QRect(offset+option.rect.topLeft(), QSize(option.rect.width()-offset.x(), option.rect.height())));
        return;
    }
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}
