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
    qDebug()<<"QFRotatableLabel::setOrientation: "<<m_orientation<<m_text<<size();
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
    qDebug()<<"QFRotatableLabel::paintEvent("<<objectName()<<"):  m_orientation="<<m_orientation<<size()<<contentsRect()<<" isPixmap="<<isPixmap<<m_pix.isNull()<<m_pix.size()<<"  isRichText"<<isRichText<<m_text;
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
    if (m_orientation==UpsideDownRotation) {
        painter.translate(cr.width(), cr.height());
        painter.rotate(180);
        painter.translate(fm.width('x'),0);
    } else if (m_orientation==ClockwiseRotation) {
        QColor cred(Qt::red); cred.setAlphaF(0.5);
        QColor cgreen(Qt::green); cgreen.setAlphaF(0.5);
        QColor cblue(Qt::blue); cblue.setAlphaF(0.5);
        painter.setBrush(cred);
        painter.fillRect(lr, cred);
        painter.translate(0,cr.width());
        painter.rotate(-90);
        painter.setBrush(cgreen);
        painter.fillRect(lr, cgreen);
        painter.translate(fm.width('x'),0);
        painter.setBrush(cblue);
        painter.fillRect(lr, cblue);

    } else if (m_orientation==CounterClockwiseRotation) {
        painter.translate(cr.width(),0);
        painter.rotate(90);
    }
    //cr.adjust(d->margin, d->margin, -d->margin, -d->margin);
    int align = QStyle::visualAlignment(layoutDirection(), QFlag(m_alignment));


    if (!isPixmap) {
        //QRectF lr = layoutRect().toAlignedRect();
        if (!isRichText) {
            QStyleOption opt;
            opt.initFrom(this);
            int flags = align;
            style->drawItemText(&painter, cr, flags, opt.palette, isEnabled(), m_text, foregroundRole());
        } else {

            QAbstractTextDocumentLayout::PaintContext context;
            if (!isEnabled()) {
                context.palette = opt.palette;
                context.palette.setColor(QPalette::Text, context.palette.light().color());
                painter.save();
                painter.translate(cr.x() + 1, cr.y() + 1);
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
            painter.translate(lr.topLeft());
            painter.setClipRect(lr.translated(-lr.x(), -lr.y()));
            QAbstractTextDocumentLayout *layout = document.documentLayout();
            layout->draw(&painter, context);
            painter.restore();
        }
    } else if (!m_pix.isNull()) {
        QStyleOption opt;
        QPixmap pix=m_pix;
        opt.initFrom(this);
        if (!isEnabled())
            pix = style->generatedIconPixmap(QIcon::Disabled, pix, &opt);
        style->drawItemPixmap(&painter, cr, align, pix);
    }
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
        size=QSize(fm.width(m_text), fm.height());
        if (m_orientation==ClockwiseRotation || m_orientation==CounterClockwiseRotation)
            size.transpose();
        return size;
    } else {
        size= m_pix.size();
    }
    return size;
}

QSize QFRotatableLabel::minimumSizeHint() const {
    return sizeHint();
}

QRectF QFRotatableLabel::layoutRect() const
{
    QRectF cr = contentsRect();
    // Caculate y position manually
    qreal rh = document.documentLayout()->documentSize().height();
    qreal yo = 0;
    if (m_alignment & Qt::AlignVCenter)
        yo = qMax((cr.height()-rh)/2, qreal(0));
    else if (m_alignment & Qt::AlignBottom)
        yo = qMax(cr.height()-rh, qreal(0));
    return QRectF(cr.x(), yo + cr.y(), cr.width(), cr.height());

}
