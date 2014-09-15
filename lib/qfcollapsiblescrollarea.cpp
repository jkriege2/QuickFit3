#include "qfcollapsiblescrollarea.h"

QFCollapsibleScrollArea::QFCollapsibleScrollArea(QWidget *parent) :
    QScrollArea(parent)
{
    init();
}

QFCollapsibleScrollArea::QFCollapsibleScrollArea(const QString &title, QWidget *parent):
    QScrollArea(parent)
{
    init();
    setTitle(title);
}

QFCollapsibleScrollArea::QFCollapsibleScrollArea(const QPixmap &pix, const QString &title, QWidget *parent):
    QScrollArea(parent)
{
    init();
    setTitle(title);
    setIcon(pix);
}

void QFCollapsibleScrollArea::init() {
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAlignment(Qt::AlignTop|Qt::AlignLeft);
    setWidgetResizable(true);
    setFrameShape(QFrame::NoFrame);
    m_iconOpened=QPixmap(":/lib/collapse_opened.png");
    m_iconClosed=QPixmap(":/lib/collapse_closed.png");
    m_iconOpenedHover=QPixmap(":/lib/collapse_openedhover.png");
    m_iconClosedHover=QPixmap(":/lib/collapse_closedhover.png");

    /*m_border=0;
    m_separation=6;
    m_headerSeparation=2;
    m_title="";
    m_icon=QPixmap();*/
    m_opened=true;


    mainLayout=new QVBoxLayout();
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2,2,2,2);
    frameHeader=new QFrame(this);
    headerLayout=new QHBoxLayout();
    headerLayout->setContentsMargins(0,0,0,0);
    headerLayout->setSpacing(6);
    frameHeader->setLayout(headerLayout);
    frameAll=new QFrame(this);
    frameAll->setFrameShape(QFrame::StyledPanel);
    frameAll->setFrameShadow(QFrame::Plain);
    labIcon=new QFRotatableLabel(this);
    labTitle=new QFRotatableLabel(this);
    QFont tf=labTitle->font();
    tf.setBold(true);
    tf.setPointSize(tf.pointSize()+2);
    labTitle->setFont(tf);
    labButton=new QFRotatableLabel(this);
    connect(labButton, SIGNAL(mouseEntered()), this, SLOT(buttonEntered()));
    connect(labButton, SIGNAL(mouseLeft()), this, SLOT(buttonLeft()));
    connect(labButton, SIGNAL(clicked()), this, SLOT(toggle()));
    connect(labIcon, SIGNAL(clicked()), this, SLOT(toggle()));
    connect(labTitle, SIGNAL(clicked()), this, SLOT(toggle()));
    labButton->setPixmap(m_iconOpened);
    labButton->setToolTip(tr("hide/show this section"));
    headerLayout->addWidget(labIcon, 0);
    headerLayout->addWidget(labTitle, 1);
    headerLayout->addWidget(labButton, 0);
    mainLayout->addWidget(frameHeader);

    widMain=new QWidget(this);
    privateMain=true;
    mainLayout->addWidget(widMain, 1);
    mainLayout->addStretch();
    frameAll->setLayout(mainLayout);
    QScrollArea::setWidget(frameAll);
}

QFormLayout *QFCollapsibleScrollArea::setFormLayout()
{
    QFormLayout* l=new QFormLayout();
    setLayout(l);
    return l;
}

QHBoxLayout *QFCollapsibleScrollArea::setHBoxLayout()
{
    QHBoxLayout* l=new QHBoxLayout();
    setLayout(l);
    return l;
}

QVBoxLayout *QFCollapsibleScrollArea::setVBoxLayout()
{
    QVBoxLayout* l=new QVBoxLayout();
    setLayout(l);
    return l;
}

QGridLayout *QFCollapsibleScrollArea::setGridLayout()
{
    QGridLayout* l=new QGridLayout();
    setLayout(l);
    return l;
}

QString QFCollapsibleScrollArea::title() const
{
    return labTitle->text();
    //return m_title;
}

bool QFCollapsibleScrollArea::isOpened() const
{
    return m_opened;
}

const QPixmap* QFCollapsibleScrollArea::icon() const
{
    return labIcon->pixmap();
    //return m_icon;
}

int QFCollapsibleScrollArea::separation() const
{
    return headerLayout->spacing();
}

/*int QFCollapsibleScrollArea::separation() const
{
    return m_separation;
}*/

int QFCollapsibleScrollArea::headerSeparation() const
{
    return mainLayout->spacing();
    //return m_headerSeparation;
}

void QFCollapsibleScrollArea::setLayout(QLayout *layout)
{
    widMain->setLayout(layout);
    //invalidate();
}

QLayout *QFCollapsibleScrollArea::layout()
{
    return widMain->layout();
}

void QFCollapsibleScrollArea::setWidget(QWidget *widget)
{
    if (widMain) {
        mainLayout->removeWidget(widMain);
        if (privateMain && widMain) {
            delete widMain;
        }
    }
    if (widget) mainLayout->insertWidget(1, widget,1);
    privateMain=false;
    widMain=widget;
    invalidate();
}

QWidget *QFCollapsibleScrollArea::widget() const
{
    return widMain;
}

/*QSize QFCollapsibleScrollArea::sizeHint() const
{
    QSize fs=QFrame::sizeHint();
    int hh=getHeaderHeight();
    int hw=getHeaderWidth();
    QSize s;
    if (m_opened) {
        s=QSize(qMax(fs.width(), hw), fs.height()+hh);
    } else {
        s=QSize(hw, hh);
    }
    return s;
}

QSize QFCollapsibleScrollArea::minimumSizeHint() const
{
    int hh=getHeaderHeight();
    int hw=getHeaderWidth();
    QSize fs=QFrame::minimumSizeHint();
    return QSize(qMax(fs.width(), hw), fs.height()+hh);
}*/

void QFCollapsibleScrollArea::setTitle(const QString &text)
{
    labTitle->setText(text+"  ");
    //m_title=text;
    //invalidate();
}

void QFCollapsibleScrollArea::setIcon(const QPixmap &icon)
{
    labIcon->setPixmap(icon);
    //m_icon=icon;
    //invalidate();
}

void QFCollapsibleScrollArea::setOpened(bool opened)
{
    bool o=m_opened;
    m_opened=opened;
    if (opened!=o) invalidate();
}

void QFCollapsibleScrollArea::setSeparation(int separation)
{
    headerLayout->setSpacing(separation);
}

/*void QFCollapsibleScrollArea::setSeparation(int separation)
{
    m_separation=separation;
    invalidate();
}*/

void QFCollapsibleScrollArea::setHeaderSeparation(int separation)
{
    mainLayout->setSpacing(separation);
    /*m_headerSeparation=separation;
    invalidate();*/
}

void QFCollapsibleScrollArea::invalidate()
{
    widMain->setVisible(m_opened);
    if (m_opened) {
        labButton->setPixmap(m_iconOpened);
        setMaximumHeight(16777215);
    } else {
        labButton->setPixmap(m_iconClosed);
        setMaximumHeight(frameHeader->height()+10);
    }
    updateGeometry();
    repaint();
}


void QFCollapsibleScrollArea::buttonEntered()
{
    if (m_opened) {
        labButton->setPixmap(m_iconOpenedHover);
    } else {
        labButton->setPixmap(m_iconClosedHover);
    }
}

void QFCollapsibleScrollArea::buttonLeft()
{
    if (m_opened) {
        labButton->setPixmap(m_iconOpened);
    } else {
        labButton->setPixmap(m_iconClosed);
    }
}

void QFCollapsibleScrollArea::toggle()
{
    m_opened=!m_opened;
    //qDebug()<<"toggle(): opened="<<m_opened;
    invalidate();
    emit toggled(m_opened);
}

/*void QFCollapsibleScrollArea::myDrawFrame(QPainter *p)
{
    int hh=getHeaderHeight();
    //int hw=getHeaderWidth();
    Q_D(QFrame);
    QStyleOptionFrameV3 opt;
    opt.init(this);
    int frameShape  = d->frameStyle & QFrame::Shape_Mask;
    int frameShadow = d->frameStyle & QFrame::Shadow_Mask;
    opt.frameShape = Shape(int(opt.frameShape) | frameShape);
    opt.rect = frameRect();
    opt.rect.setHeight(opt.rect.height()-hh);
    opt.rect.setTop(opt.rect.top()+hh);
    switch (frameShape) {
        case QFrame::Box:
        case QFrame::HLine:
        case QFrame::VLine:
        case QFrame::StyledPanel:
        case QFrame::Panel:
            opt.lineWidth = d->lineWidth;
            opt.midLineWidth = d->midLineWidth;
            break;
        default:
            // most frame styles do not handle customized line and midline widths
            // (see updateFrameWidth()).
            opt.lineWidth = d->frameWidth;
            break;
    }

    if (frameShadow == Sunken)
        opt.state |= QStyle::State_Sunken;
    else if (frameShadow == Raised)
        opt.state |= QStyle::State_Raised;

    style()->drawControl(QStyle::CE_ShapedFrame, &opt, p, this);
}


void QFCollapsibleScrollArea::paintEvent(QPaintEvent *event)
{
    int hh=getHeaderHeight();
    int hw=getHeaderWidth();
    QPainter paint(this);
    int x=0;
    int y=0;
    QFontMetrics fm(font());
    if (m_icon.width()>0) {
        paint.drawPixmap(x,y,m_icon);
        x=x+m_icon.width()+m_separation;
    }
    paint.drawText(QRect(x, y, fm.width(m_title), hh),Qt::AlignLeft|Qt::AlignBottom,  m_title);

    if (m_opened) {
        paint.drawPixmap(width()-m_iconOpened.width(), hh-m_iconOpened.height(), m_iconOpened);
        paint.translate(0,hh);
        myDrawFrame(&paint);
    } else {
        paint.drawPixmap(width()-m_iconClosed.width(), hh-m_iconClosed.height(), m_iconClosed);
    }
}

int QFCollapsibleScrollArea::getHeaderHeight() const
{
    int h=m_iconOpened.height();
    h=qMax(h,m_iconClosed.height());
    h=qMax(h,m_iconOpenedHover.height());
    h=qMax(h,m_iconClosedHover.height());
    QFontMetrics fm(font());
    h=qMax(h,fm.height());
    return h+m_headerSeparation;
}

int QFCollapsibleScrollArea::getHeaderWidth() const
{
    int hiw=m_iconOpened.width();
    h=qMax(h,m_iconClosed.width());
    h=qMax(h,m_iconOpenedHover.width());
    h=qMax(h,m_iconClosedHover.width());
    QFontMetrics fm(font());
    int w=fm.width(m_title)+m_separation+hiw;
    if (m_icon.width()>0) w=w+m_icon.width()+m_separation;
    return qMax(width(), w);
}*/
