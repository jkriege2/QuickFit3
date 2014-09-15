#include "qfcollapsibleframe.h"

QFCollapsibleFrame::QFCollapsibleFrame(QWidget *parent) :
    QFrame(parent)
{
    init();
}

QFCollapsibleFrame::QFCollapsibleFrame(const QString &title, QWidget *parent):
    QFrame(parent)
{
    init();
    setTitle(title);
}

QFCollapsibleFrame::QFCollapsibleFrame(const QPixmap &pix, const QString &title, QWidget *parent):
    QFrame(parent)
{
    init();
    setTitle(title);
    setIcon(pix);
}


QFCollapsibleFrame::QFCollapsibleFrame(QFCollapsibleFrame::HeaderPosition pos, QWidget *parent):
    QFrame(parent)
{
    init();
    setHeaderPosition(pos);
}

QFCollapsibleFrame::QFCollapsibleFrame(const QString &title, QFCollapsibleFrame::HeaderPosition pos, QWidget *parent):
    QFrame(parent)
{
    init();
    setTitle(title);
    setHeaderPosition(pos);
}

QFCollapsibleFrame::QFCollapsibleFrame(const QPixmap &pix, const QString &title, QFCollapsibleFrame::HeaderPosition pos, QWidget *parent):
    QFrame(parent)
{
    init();
    setTitle(title);
    setIcon(pix);
    setHeaderPosition(pos);
}

void QFCollapsibleFrame::init()
{

    m_pos=North;
    m_iconOpened=QPixmap(":/lib/collapse_opened.png");
    m_iconClosed=QPixmap(":/lib/collapse_closed.png");
    m_iconOpenedHover=QPixmap(":/lib/collapse_openedhover.png");
    m_iconClosedHover=QPixmap(":/lib/collapse_closedhover.png");

    m_opened=true;


    mainLayout=new QBoxLayout(QBoxLayout::TopToBottom);
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2,2,2,2);
    frameHeader=new QFrame(this);
    headerLayout=new QBoxLayout(QBoxLayout::LeftToRight);
    headerLayout->setContentsMargins(0,0,0,0);
    headerLayout->setSpacing(6);
    frameHeader->setLayout(headerLayout);
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Plain);
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

    QFrame::setLayout(mainLayout);
}

QString QFCollapsibleFrame::title() const
{
    return labTitle->text();
    //return m_title;
}

bool QFCollapsibleFrame::isOpened() const
{
    return m_opened;
}

const QPixmap* QFCollapsibleFrame::icon() const
{
    return labIcon->pixmap();
    //return m_icon;
}

int QFCollapsibleFrame::separation() const
{
    return headerLayout->spacing();
}

/*int QFCollapsibleFrame::separation() const
{
    return m_separation;
}*/

int QFCollapsibleFrame::headerSeparation() const
{
    return mainLayout->spacing();
    //return m_headerSeparation;
}

QFCollapsibleFrame::HeaderPosition QFCollapsibleFrame::headerPos() const
{
    return m_pos;
}

void QFCollapsibleFrame::setLayout(QLayout *layout)
{
    if (!privateMain) {
        mainLayout->removeWidget(widMain);
        widMain=new QWidget(this);
        mainLayout->insertWidget(1,widMain,1);
        privateMain=true;
    }
    widMain->setLayout(layout);
    //invalidate();
}

QLayout *QFCollapsibleFrame::layout()
{
    return widMain->layout();
}

void QFCollapsibleFrame::setWidget(QWidget *widget)
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

QWidget *QFCollapsibleFrame::widget() const
{
    return widMain;
}

QFormLayout *QFCollapsibleFrame::setFormLayout()
{
    QFormLayout* l=new QFormLayout();
    setLayout(l);
    return l;
}

QHBoxLayout *QFCollapsibleFrame::setHBoxLayout()
{
    QHBoxLayout* l=new QHBoxLayout();
    setLayout(l);
    return l;
}

QVBoxLayout *QFCollapsibleFrame::setVBoxLayout()
{
    QVBoxLayout* l=new QVBoxLayout();
    setLayout(l);
    return l;
}

QGridLayout *QFCollapsibleFrame::setGridLayout()
{
    QGridLayout* l=new QGridLayout();
    setLayout(l);
    return l;
}

void QFCollapsibleFrame::setTitle(const QString &text)
{
    labTitle->setText(text+"  ");
}

void QFCollapsibleFrame::setIcon(const QPixmap &icon)
{
    labIcon->setPixmap(icon);
}

void QFCollapsibleFrame::setOpened(bool opened)
{
    bool o=m_opened;
    m_opened=opened;
    if (opened!=o) invalidate();
}

void QFCollapsibleFrame::setSeparation(int separation)
{
    headerLayout->setSpacing(separation);
}


void QFCollapsibleFrame::setHeaderSeparation(int separation)
{
    mainLayout->setSpacing(separation);
}

void QFCollapsibleFrame::setHeaderPosition(QFCollapsibleFrame::HeaderPosition pos)
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

void QFCollapsibleFrame::invalidate()
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


void QFCollapsibleFrame::buttonEntered()
{
    if (m_opened) {
        labButton->setPixmap(m_iconOpenedHover);
    } else {
        labButton->setPixmap(m_iconClosedHover);
    }
}

void QFCollapsibleFrame::buttonLeft()
{
    if (m_opened) {
        labButton->setPixmap(m_iconOpened);
    } else {
        labButton->setPixmap(m_iconClosed);
    }
}


void QFCollapsibleFrame::toggle()
{
    m_opened=!m_opened;
    //qDebug()<<"toggle(): opened="<<m_opened;
    invalidate();
    emit toggled(m_opened);
}


