#include "qfenhancedlabel.h"
#include <QDesktopServices>


QFEnhancedLabel::QFEnhancedLabel(QWidget *parent, Qt::WindowFlags f):
    QLabel(parent, f)
{
    underlineOnHover=false;
    setMouseTracking(true);
}

QFEnhancedLabel::QFEnhancedLabel(const QString &text, QWidget *parent, Qt::WindowFlags f):
    QLabel(text, parent, f)
{
    underlineOnHover=false;
    setMouseTracking(true);
}

void QFEnhancedLabel::setUnderlineOnHover(bool enabled)
{
    underlineOnHover=enabled;
}

void QFEnhancedLabel::setOpenUrl(QUrl url)
{
    openUrl=url;
}

void QFEnhancedLabel::mouseReleaseEvent(QMouseEvent *event)
{
    QLabel::mouseReleaseEvent(event);
    emit clicked();
    if (openUrl.isValid() && !openUrl.isEmpty()) QDesktopServices::openUrl(openUrl);
}

void QFEnhancedLabel::leaveEvent(QEvent *event)
{
    QFont f=font();
    f.setUnderline(false);
    setFont(f);
}

void QFEnhancedLabel::enterEvent(QEvent *event)
{
    QFont f=font();
    f.setUnderline(true);
    setFont(f);
}
