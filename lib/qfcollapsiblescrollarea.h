#ifndef QFCollapsibleScrollArea_H
#define QFCollapsibleScrollArea_H

#include <QFrame>

#include "lib_imexport.h"
#include "programoptions.h"
#include "qfrotatablelabel.h"
#include "QVBoxLayout"

/*! \brief a QScrollArea, which has a tile+icon and a button, that collapses/expands its contents
    \ingroup qf3lib_widgets

 */
class QFLIB_EXPORT QFCollapsibleScrollArea : public QScrollArea
{
        Q_OBJECT
    public:
        explicit QFCollapsibleScrollArea(QWidget *parent = 0);
        explicit QFCollapsibleScrollArea(const QString& title, QWidget *parent = 0);
        explicit QFCollapsibleScrollArea(const QPixmap& pix, const QString& title, QWidget *parent = 0);

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
        QFormLayout *setFormLayout();
        QHBoxLayout *setHBoxLayout();
        QVBoxLayout *setVBoxLayout();
        QGridLayout *setGridLayout();
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



        void init();
    protected:
        //virtual void paintEvent(QPaintEvent* event);
        //int getHeaderHeight() const;
        //int getHeaderWidth() const;
        //virtual void myDrawFrame(QPainter *p);
};

#endif // QFCollapsibleScrollArea_H
