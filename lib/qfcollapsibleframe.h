#ifndef QFCOLLAPSIBLEFRAME_H
#define QFCOLLAPSIBLEFRAME_H

#include <QFrame>

#include "lib_imexport.h"
#include "programoptions.h"
#include "qfrotatablelabel.h"
#include "QVBoxLayout"

/*! \brief a frame, which has a tile+icon and a button, that collapses/expands its contents
    \ingroup qf3lib_widgets

 */
class QFLIB_EXPORT QFCollapsibleFrame : public QScrollArea
{
        Q_OBJECT
    public:
        explicit QFCollapsibleFrame(QWidget *parent = 0);

        QString title() const;
        bool isOpened() const;
        const QPixmap *icon() const;
        int separation() const;
        int headerSeparation() const;

        void setLayout(QLayout* layout);
        QLayout* layout();

        void setWidget(QWidget* widget);
        QWidget* widget() const;

        //virtual QSize sizeHint() const;
        //virtual QSize minimumSizeHint() const;
    signals:
        void toggled(bool opened);
    public slots:
        void setTitle(const QString &title);
        void setIcon(const QPixmap& icon);
        void setOpened(bool opened);
        void setSeparation(int separation);
        void setHeaderSeparation(int separation);

        void invalidate();
        void toggle();
    protected slots:
        void buttonEntered();
        void buttonLeft();
    private:
        //QString m_title;
        //QPixmap m_icon;
        bool m_opened;
        QPixmap m_iconOpened;
        QPixmap m_iconClosed;
        QPixmap m_iconOpenedHover;
        QPixmap m_iconClosedHover;
        //int m_border;
        //int m_separation;
        //int m_headerSeparation;

        QFRotatableLabel* labIcon;
        QFRotatableLabel* labTitle;
        QFRotatableLabel* labButton;
        QFrame* frameHeader;
        QFrame* frameAll;
        QVBoxLayout* mainLayout;
        QHBoxLayout* headerLayout;
        QWidget* widMain;
        bool privateMain;



    protected:
        //virtual void paintEvent(QPaintEvent* event);
        //int getHeaderHeight() const;
        //int getHeaderWidth() const;
        //virtual void myDrawFrame(QPainter *p);
};

#endif // QFCOLLAPSIBLEFRAME_H
