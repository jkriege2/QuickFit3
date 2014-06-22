#ifndef TEST_WIDGET_H
#define TEST_WIDGET_H

#include <QtGui/QWidget>

class TestWidget : public QWidget
{
    Q_OBJECT

public:
    TestWidget(QWidget *parent = 0);
    ~TestWidget();
    
    QPaintEngine *paintEngine() const;
     
protected:
    void paintEvent(QPaintEvent *e);

private:
    QPaintEngine *pe;
};

#endif
