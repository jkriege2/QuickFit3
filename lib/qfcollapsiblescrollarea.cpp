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

QFCollapsibleScrollArea::QFCollapsibleScrollArea(QFCollapsibleScrollArea::HeaderPosition pos, QWidget *parent):
    QScrollArea(parent)
{
    init();
    setHeaderPosition(pos);
}

QFCollapsibleScrollArea::QFCollapsibleScrollArea(const QString &title, QFCollapsibleScrollArea::HeaderPosition pos, QWidget *parent):
    QScrollArea(parent)
{
    init();
    setTitle(title);
    setHeaderPosition(pos);
}

QFCollapsibleScrollArea::QFCollapsibleScrollArea(const QPixmap &pix, const QString &title, QFCollapsibleScrollArea::HeaderPosition pos, QWidget *parent):
    QScrollArea(parent)
{
    init();
    setTitle(title);
    setIcon(pix);
    setHeaderPosition(pos);
}

void QFCollapsibleScrollArea::init() {
    m_pos=North;
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


    mainLayout=new QBoxLayout(QBoxLayout::TopToBottom);
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2,2,2,2);
    frameHeader=new QFrame(this);
    headerLayout=new QBoxLayout(QBoxLayout::LeftToRight);
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

    widMain=new QWidget(this);
    privateMain=true;
    mainLayout->addWidget(frameHeader);
    mainLayout->addWidget(widMain);
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

QFCollapsibleScrollArea::HeaderPosition QFCollapsibleScrollArea::headerPos() const
{
    return m_pos;
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
    if (widget) mainLayout->insertWidget(1,widget,1);
    privateMain=false;
    widMain=widget;
    invalidate();
    setHeaderPosition(headerPos());
}

QWidget *QFCollapsibleScrollArea::widget() const
{
    return widMain;
}


void QFCollapsibleScrollArea::setTitle(const QString &text)
{
    labTitle->setText(text+"  ");
}

void QFCollapsibleScrollArea::setIcon(const QPixmap &icon)
{
    labIcon->setPixmap(icon);
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


void QFCollapsibleScrollArea::setHeaderSeparation(int separation)
{
    mainLayout->setSpacing(separation);

}

void QFCollapsibleScrollArea::setHeaderPosition(QFCollapsibleScrollArea::HeaderPosition pos)
{
    m_pos=pos;



    switch (m_pos) {
        case South: {
                mainLayout->setDirection(QBoxLayout::BottomToTop);
                labIcon->setOrientation(QFRotatableLabel::NoRotation);
                labTitle->setOrientation(QFRotatableLabel::NoRotation);
                labButton->setOrientation(QFRotatableLabel::UpsideDownRotation);
                headerLayout->setDirection(QBoxLayout::LeftToRight);
            }
            break;
        case West: {
                mainLayout->setDirection(QBoxLayout::LeftToRight);
                labIcon->setOrientation(QFRotatableLabel::CounterClockwiseRotation);
                labTitle->setOrientation(QFRotatableLabel::CounterClockwiseRotation);
                labButton->setOrientation(QFRotatableLabel::CounterClockwiseRotation);
                headerLayout->setDirection(QBoxLayout::BottomToTop);
            }
            break;
        case East: {

                mainLayout->setDirection(QBoxLayout::RightToLeft);
                labIcon->setOrientation(QFRotatableLabel::ClockwiseRotation);
                labTitle->setOrientation(QFRotatableLabel::ClockwiseRotation);
                labButton->setOrientation(QFRotatableLabel::ClockwiseRotation);
                headerLayout->setDirection(QBoxLayout::BottomToTop);
            }
            break;
        default:
        case North: {

                mainLayout->setDirection(QBoxLayout::TopToBottom);
                labIcon->setOrientation(QFRotatableLabel::NoRotation);
                labTitle->setOrientation(QFRotatableLabel::NoRotation);
                labButton->setOrientation(QFRotatableLabel::NoRotation);
                headerLayout->setDirection(QBoxLayout::LeftToRight);
            }
            break;
    }
    invalidate();
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

