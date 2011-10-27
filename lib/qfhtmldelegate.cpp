#include "qfhtmldelegate.h"
#include <QDebug>
#include <QTextEdit>
#include <QVariant>
#include <QTextDocument>
#include <QPainter>
#include <QApplication>
#include <QBrush>

QFHTMLDelegate::QFHTMLDelegate(QObject* parent):
    QStyledItemDelegate(parent)
{
    m_displayRichTextEditor=true;
}

QFHTMLDelegate::~QFHTMLDelegate()
{
    //dtor
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
}


void QFHTMLDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QVariant data=index.data(Qt::DisplayRole).toString();
    QVariant check=index.data(Qt::CheckStateRole);
    QBrush back=index.data(Qt::BackgroundRole).value<QBrush>();
    QPoint offset=QPoint(0,0);
    //qDebug()<<option.palette.base()<<option.palette.background();
    if (check.isValid()) {
        offset=QPoint(option.rect.height()+2, 0);
    }
    if (index.data(Qt::BackgroundRole).isValid()) {
        painter->fillRect(option.rect, back);
    }
    if (data.type()==QVariant::String) {


        QString pre, post;
        pre="<font color=\""+option.palette.text().color().name()+"\">";
        post="</font>";
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
            pre="<font color=\""+option.palette.highlightedText().color().name()+"\">";
            post="</font>";
        }

        QTextDocument doc;
        //std::cout<<"drawing "<<QString(pre+data.toString()+post).toStdString()<<std::endl;
        doc.setHtml(pre+data.toString()+post);
        painter->save();

        if (check.isValid()) {
            drawCheck(painter, option, QRect(option.rect.left(),option.rect.top(),option.rect.height(),option.rect.height()), check.toInt()!=0?Qt::Checked:Qt::Unchecked);
        }

        painter->translate(option.rect.topLeft()+offset);
        QRect r(QPoint(0, 0), option.rect.size()-QSize(offset.x(), 0));
        doc.drawContents(painter, r);


        painter->restore();

    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize QFHTMLDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    //qDebug()<<" QFHTMLDelegate::sizeHint()";
    QVariant data=index.data(Qt::DisplayRole).toString();
    if (data.type()==QVariant::String) {
        QTextDocument doc;
        doc.setHtml(data.toString());
        return doc.size().toSize();
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
