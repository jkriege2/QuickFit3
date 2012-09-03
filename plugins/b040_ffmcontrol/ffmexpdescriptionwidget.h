#ifndef FFMEXPDESCRIPTIONWIDGET_H
#define FFMEXPDESCRIPTIONWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QtGui>
#include "qtlogfile.h"

class FFMExpDescriptionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FFMExpDescriptionWidget(QWidget *parent = 0);
    ~FFMExpDescriptionWidget();
protected:
    QGridLayout *layout;

    QFrame *fr1;
    QGridLayout *frl1;
    QFrame *fr2;
    QGridLayout *frl2;
    QFrame *fr3;
    QGridLayout *frl3;
    QFrame *fr4;
    QGridLayout *frl4;


    QLineEdit *Number;
    QLineEdit *Title;

    QLineEdit *Person;
    QLineEdit *Page;

    QTextEdit *Sample;

    QTextEdit *Description;



signals:

public slots:

};

#endif // FFMEXPDESCRIPTIONWIDGET_H
