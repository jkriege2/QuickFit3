#include <QtGui/QPainter>
#include "widget.h"
#include "qpaintengine_cairo_p.h"

bool qt_use_cairo()
{
    static signed char checked_env = -1;
    if(checked_env == -1)
        checked_env = (qgetenv("QT_USE_CAIRO") == "1") ? 1 : 0;
    return checked_env;
}
 
TestWidget::TestWidget(QWidget *parent)
:QWidget(parent)
{
    if (qt_use_cairo())
    {
        pe = new QCairoPaintEngine();
        setAttribute(Qt::WA_PaintOnScreen);
    }
}

TestWidget::~TestWidget()
{
    if (qt_use_cairo())
        delete pe;
}

QPaintEngine* TestWidget::paintEngine() const
{
  if (qt_use_cairo())
      return pe;
 
  return QWidget::paintEngine();
}

void TestWidget::paintEvent(QPaintEvent*)
{
    QPixmap left(":/res/images/button_normal_cap_left.png");
    QPixmap center(":/res/images/button_normal_stretch.png");
    QPixmap right(":/res/images/button_normal_cap_right.png");
    QPixmap logo(":/res/images/qt-logo.png");

    QPainter p(this);
	p.setRenderHints(QPainter::Antialiasing, true);
	p.setOpacity(0.5);

    p.translate(10, 0);
	p.setPen(QPen(Qt::NoPen));

	p.setBrush(QBrush(left));
    p.drawRect(0, 0, left.width(), left.height());

	p.setBrush(QBrush(center));
    p.drawRect(left.width(), 0, 200 - left.width() - right.width(), center.height());

	p.setBrush(QBrush(right));
    p.drawRect(200 - right.width(), 0, right.width(), right.height());
    
    p.translate(220, 0);
    p.drawPixmap(0, 0, left);
    p.drawTiledPixmap(left.width(), 0, 200 - left.width() - right.width(), center.height(), center);
    p.drawPixmap(200 - right.width(), 0, right);
    
    p.translate(-220, left.height() + 10);
    p.setBrush(QBrush(logo));
    p.setBrushOrigin(-10,-10);
    p.drawRect(0, 0, 200, 200);

    p.drawTiledPixmap(220, 0, 200, 200, logo, 10, 10);

    p.translate(0, 250);
    p.setPen(QPen(QBrush(Qt::red, Qt::DiagCrossPattern), 1));
    QFont font = p.font();
    font.setPointSize(48);
    p.setFont(font);
    p.drawText(0, 0, "This is a test.");

    p.translate(0, 10);
    p.setBrush(QBrush(Qt::red, Qt::DiagCrossPattern));
    p.drawRect(0, 0,200, 200);
}
