#include "qfelidedlabel.h"
#include <QPainter>
#include <QTextLayout>
#include <QDebug>

QFElidedLabel::QFElidedLabel(QWidget *parent, Qt::WindowFlags f):
    QLabel(parent, f)
{
    setSizePolicy(QSizePolicy::Ignored, sizePolicy().verticalPolicy());
    elided=true;
    m_elidedMode=Qt::ElideMiddle;
    update();
}

QFElidedLabel::QFElidedLabel(const QString &text, QWidget *parent, Qt::WindowFlags f):
    QLabel(text, parent, f)
{
    setSizePolicy(QSizePolicy::Ignored, sizePolicy().verticalPolicy());
    elided=true;
    m_elidedMode=Qt::ElideMiddle;
    update();
}

Qt::TextElideMode QFElidedLabel::elidedMode() const
{
    return m_elidedMode;
}

void QFElidedLabel::setElided(bool elided)
{
    this->elided=elided;
    update();
}

void QFElidedLabel::setElidedMode(Qt::TextElideMode elidedMode)
{
    m_elidedMode=elidedMode;
    update();
}

void QFElidedLabel::paintEvent(QPaintEvent *event)
{
    if (elided) {
         QFrame::paintEvent(event);

         QPainter painter(this);
         QFontMetrics fontMetrics = painter.fontMetrics();

         bool didElide = false;
         int lineSpacing = fontMetrics.lineSpacing();
         int y = 0;

         QTextLayout textLayout(text(), painter.font());
         QTextOption opt=textLayout.textOption();
         opt.setAlignment(alignment());
         opt.setWrapMode((wordWrap())?QTextOption::WordWrap:QTextOption::NoWrap);

         textLayout.setTextOption(opt);
         textLayout.beginLayout();
         forever {
             QTextLine line = textLayout.createLine();

             if (!line.isValid())
                 break;

             line.setLineWidth(width());
             int nextLineY = y + lineSpacing;

             if (height() >= nextLineY + lineSpacing) {
                 line.draw(&painter, QPoint(0, y));
                 y = nextLineY;
             } else {
                 QString lastLine = text().mid(line.textStart());
                 QString elidedLastLine = fontMetrics.elidedText(lastLine, m_elidedMode, width());
                 painter.drawText(QPoint(0, y + fontMetrics.ascent()), elidedLastLine);
                 line = textLayout.createLine();
                 didElide = line.isValid();
                 break;
             }
         }
         textLayout.endLayout();


    } else {
        QLabel::paintEvent(event);
    }
}

