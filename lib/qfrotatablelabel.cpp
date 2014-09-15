#include "qfrotatablelabel.h"


QFRotatableLabel::QFRotatableLabel(QWidget *parent, Qt::WindowFlags f):
    QFrame(parent, f)
{
    //setSizePolicy(QSizePolicy());
    setMouseTracking(true);
    m_orientation=NoRotation;
    m_text="";
    m_alignment=Qt::AlignLeft|Qt::AlignVCenter;
    m_textFormat=Qt::AutoText;
    m_wordWrap=false;
    setText("");
}

QFRotatableLabel::QFRotatableLabel(const QString &text, QWidget *parent, Qt::WindowFlags f):
    QFrame(parent, f)
{
    setMouseTracking(true);
    m_orientation=NoRotation;
    m_text="";
    m_alignment=Qt::AlignLeft|Qt::AlignVCenter;
    m_textFormat=Qt::AutoText;
    m_wordWrap=false;
    setText(text);
}

QFRotatableLabel::Rotation QFRotatableLabel::orientation() const
{
    return m_orientation;
}

void QFRotatableLabel::setOrientation(Rotation o)
{
    Rotation old=m_orientation;
    m_orientation=o;
    /*if (((m_orientation==NoRotation || m_orientation==UpsideDownRotation) && (old==ClockwiseRotation || old==CounterClockwiseRotation)) ||
        ((old==NoRotation || old==UpsideDownRotation) && (m_orientation==ClockwiseRotation || m_orientation==CounterClockwiseRotation))) {


        // switch orientation
        QSizePolicy sp=sizePolicy();
        QSizePolicy::Policy pv,ph;
        pv=sp.verticalPolicy();
        ph=sp.horizontalPolicy();
        bool dv=sp.hasHeightForWidth();
        bool dh=sp.hasWidthForHeight();
        uchar sv=sp.verticalStretch();
        uchar sh=sp.horizontalStretch();
        sp.setHeightForWidth(dh);
        sp.setWidthForHeight(dv);
        sp.setHorizontalStretch(sv);
        sp.setVerticalStretch(sh);
        sp.setHorizontalPolicy(pv);
        sp.setVerticalPolicy(ph);
        setSizePolicy(sp);
        resize(QSize(height(), width()));
    }
*/
    updateGeometry();
    repaint();
    //qDebug()<<"QFRotatableLabel::setOrientation: "<<m_orientation<<m_text<<size();
}

void QFRotatableLabel::setText(const QString &txt)
{
    m_text=txt;
    isRichText = m_textFormat == Qt::RichText
                        || (m_textFormat == Qt::AutoText && Qt::mightBeRichText(txt));
    isPixmap=false;
    if (isRichText) document.setHtml(txt);
    else document.setPlainText(txt);
    //qDebug()<<"QFRotatableLabel::setText  "<<m_pix.isNull()<<m_pix.size()<<isPixmap<<m_text;
    updateGeometry();
    repaint();
}

void QFRotatableLabel::setPixmap(const QPixmap &pix)
{
    m_pix=pix;
    isPixmap=true;
    //qDebug()<<"QFRotatableLabel::setPixmap "<<m_pix.isNull()<<m_pix.size()<<isPixmap<<m_text;
    updateGeometry();
    repaint();
}

void QFRotatableLabel::setTextFormat(Qt::TextFormat p)
{
    m_textFormat=p;
    updateGeometry();
    repaint();
}

void QFRotatableLabel::setAlignment(Qt::Alignment p)
{
    m_alignment=p;
    updateGeometry();
    repaint();
}

void QFRotatableLabel::setWordWrap(bool on)
{
    m_wordWrap=on;
    updateGeometry();
    repaint();
}

void QFRotatableLabel::clear()
{
    setText("");
}

void QFRotatableLabel::setNum(int num)
{
    QString str;
    str.setNum(num);
    setText(str);
}

void QFRotatableLabel::setNum(double num)
{
    QString str;
    str.setNum(num);
    setText(str);
}

void QFRotatableLabel::enterEvent(QEvent *ev)
{
    QFrame::enterEvent(ev);
    emit mouseEntered();
}

void QFRotatableLabel::leaveEvent(QEvent *ev)
{
    QFrame::leaveEvent(ev);
    emit mouseLeft();
}

void QFRotatableLabel::mousePressEvent(QMouseEvent *event)
{
    QFrame::mousePressEvent(event);
    if (event->button()==Qt::LeftButton) {
        emit clicked();
    }
}

void QFRotatableLabel::paintEvent(QPaintEvent *)
{
    //qDebug()<<"QFRotatableLabel::paintEvent("<<objectName()<<"):  m_orientation="<<m_orientation<<size()<<contentsRect()<<" isPixmap="<<isPixmap<<m_pix.isNull()<<m_pix.size()<<"  isRichText"<<isRichText<<m_text;
    QRectF lr = layoutRect().toAlignedRect();
    QStyleOption opt;
    opt.initFrom(this);
    QStyle *style = QWidget::style();
    QPainter painter(this);
    drawFrame(&painter);
    QRect cr = contentsRect();
    QFontMetrics fm=fontMetrics();
    painter.translate(cr.topLeft());
    cr.setTopLeft(QPoint(0,0));

    painter.translate(double(cr.width())/2.0, double(cr.height())/2.0);
    painter.rotate((int)m_orientation);

    double dx=-lr.width()/2.0;
    double dy=-lr.height()/2.0;
    if (m_alignment & Qt::AlignLeft) {
        if (m_orientation==UpsideDownRotation || m_orientation==NoRotation) {
            dx=-cr.width()/2.0;
        } else {
            dx=-cr.height()/2.0;
        }
    }
    if (m_alignment & Qt::AlignRight) {
        if (m_orientation==UpsideDownRotation || m_orientation==NoRotation) {
            dx=-cr.width()/2.0+(cr.width()-lr.width());
        } else {
            dx=-cr.height()/2.0+(cr.height()-lr.width());
        }
    }
    if (m_alignment & Qt::AlignTop) {
        if (m_orientation==UpsideDownRotation || m_orientation==NoRotation) {
            dy=-cr.height()/2.0;
        } else {
            dy=-cr.width()/2.0;
        }
    }
    if (m_alignment & Qt::AlignBottom) {
        if (m_orientation==UpsideDownRotation || m_orientation==NoRotation) {
            dy=-cr.height()/2.0+(cr.height()-lr.height());
        } else {
            dy=-cr.width()/2.0+(cr.width()-lr.height());
        }
    }
    if (isPixmap) {
        painter.translate(dx, dy);
    } else {
        painter.translate(dx+1, dy+1);
        //painter.translate(dx+fm.width('x')/2.0, dy+fm.width('x')/2.0);
    }
    //cr.adjust(d->margin, d->margin, -d->margin, -d->margin);
    //int align = QStyle::visualAlignment(layoutDirection(), QFlag(m_alignment));


    painter.save();
    painter.setClipRect(lr.toRect());
    if (!isPixmap) {
        //QRectF lr = layoutRect().toAlignedRect();
        if (!isRichText) {
            /*QStyleOption opt;
            opt.initFrom(this);
            style->drawItemText(&painter, lr.toRect(), Qt::AlignTop|Qt::AlignLeft, opt.palette, isEnabled(), m_text, foregroundRole());*/
            painter.setFont(font());
            painter.setPen(palette().color(QPalette::Foreground));
            if (!isEnabled()) painter.setPen(palette().color(QPalette::Disabled, QPalette::Foreground));
            painter.drawText(lr.toRect(),Qt::AlignBottom|Qt::AlignLeft,m_text);
        } else {

            /*QAbstractTextDocumentLayout::PaintContext context;
            if (!isEnabled()) {
                context.palette = opt.palette;
                context.palette.setColor(QPalette::Text, context.palette.light().color());
                painter.save();
                painter.translate(1, 1);
                //painter.setClipRect(lr.translated(-lr.x() - 1, -lr.y() - 1));
                QAbstractTextDocumentLayout *layout = document.documentLayout();
                layout->draw(&painter, context);
                painter.restore();
            }

            // Adjust the palette
            context.palette = opt.palette;

            if (foregroundRole() != QPalette::Text && isEnabled())
                context.palette.setColor(QPalette::Text, context.palette.color(foregroundRole()));

            painter.save();
            //painter.translate(lr.topLeft());
            //painter.setClipRect(lr);
            QAbstractTextDocumentLayout *layout = document.documentLayout();
            layout->draw(&painter, context);
            painter.restore();*/

            document.drawContents(&painter, lr.toRect());
        }
    } else if (!m_pix.isNull()) {
        QStyleOption opt;
        QPixmap pix=m_pix;
        opt.initFrom(this);
        if (!isEnabled()) {
            pix = style->generatedIconPixmap(QIcon::Disabled, pix, &opt);
        }
        painter.drawPixmap(lr.toRect(), pix);
        //style->drawItemPixmap(&painter, lr.toRect(), align, pix);
    }
    painter.restore();
}


QString QFRotatableLabel::text() const
{
    return m_text;
}

const QPixmap *QFRotatableLabel::pixmap() const
{
    return &m_pix;
}

Qt::TextFormat QFRotatableLabel::textFormat() const
{
    return m_textFormat;
}

Qt::Alignment QFRotatableLabel::alignment() const
{
    return m_alignment;
}

bool QFRotatableLabel::wordWrap() const
{
    return m_wordWrap;
}

QSize QFRotatableLabel::sizeHint() const
{
    QSize size;
    if (!isPixmap) {
        const QFontMetrics& fm = fontMetrics();
        size=QSize(fm.width(m_text)+5, fm.height()+1);
        if (m_orientation==ClockwiseRotation || m_orientation==CounterClockwiseRotation)
            size.transpose();
        return size;
    } else {
        size= m_pix.size()+QSize(2,2);
    }
    return size;
}

QSize QFRotatableLabel::minimumSizeHint() const {
    return sizeHint();
}

QRectF QFRotatableLabel::layoutRect() const
{
    if (isPixmap) {
        return QRectF(QPointF(0,0), QSizeF(m_pix.size()));
    } else {
        if (isRichText)  return QRectF(QPointF(0,0), document.documentLayout()->documentSize());
        else return QRectF(QPointF(0,0), QSizeF(fontMetrics().width(m_text), fontMetrics().height()));
    }
    /*QRectF cr = contentsRect();
    // Caculate y position manually
    qreal rh = document.documentLayout()->documentSize().height();
    qreal yo = 0;
    if (m_alignment & Qt::AlignVCenter)
        yo = qMax((cr.height()-rh)/2, qreal(0));
    else if (m_alignment & Qt::AlignBottom)
        yo = qMax(cr.height()-rh, qreal(0));
    return QRectF(cr.x(), yo + cr.y(), cr.width(), cr.height());*/

}
