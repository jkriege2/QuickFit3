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
        enum HeaderPosition {
            North=0,
            West=1,
            South=2,
            East=3
        };

        explicit QFCollapsibleScrollArea(QWidget *parent = 0);
        explicit QFCollapsibleScrollArea(const QString& title, QWidget *parent = 0);
        explicit QFCollapsibleScrollArea(const QPixmap& pix, const QString& title, QWidget *parent = 0);
        explicit QFCollapsibleScrollArea(HeaderPosition pos, QWidget *parent = 0);
        explicit QFCollapsibleScrollArea(const QString& title, HeaderPosition pos, QWidget *parent = 0);
        explicit QFCollapsibleScrollArea(const QPixmap& pix, const QString& title, HeaderPosition pos, QWidget *parent = 0);

        QString title() const;
        bool isOpened() const;
        const QPixmap *icon() const;
        int separation() const;
        int headerSeparation() const;
        HeaderPosition headerPos() const;

        /** \brief use a layout for the contents
         *
         *  If you used setWidget() before, the explicitly set widget is removed and replaced by an internal new QWidget
         */
        void setLayout(QLayout* layout);
        QLayout* layout();

        /** \brief use a widget for the contents */
        void setWidget(QWidget* widget);
        QWidget* widget() const;

        /** \brief add a QFormLayout (using setLayout) to this widget and return a pointer to it, so you can add widgets */
        QFormLayout *setFormLayout();
        /** \brief add a QHBoxLayout (using setLayout) to this widget and return a pointer to it, so you can add widgets */
        QHBoxLayout *setHBoxLayout();
        /** \brief add a QVBoxLayout (using setLayout) to this widget and return a pointer to it, so you can add widgets */
        QVBoxLayout *setVBoxLayout();
        /** \brief add a QGridLayout (using setLayout) to this widget and return a pointer to it, so you can add widgets */
        QGridLayout *setGridLayout();
    signals:
        void toggled(bool opened);
    public slots:
        /** \brief title of the box */
        void setTitle(const QString &title);
        /** \brief icon for the box */
        void setIcon(const QPixmap& icon);
        /** \brief set opened state */
        void setOpened(bool opened);
        /** \brief separation between header items (pixels) */
        void setSeparation(int separation);
        /** \brief separation between header and widgets */
        void setHeaderSeparation(int separation);
        /** \brief position of the header */
        void setHeaderPosition(HeaderPosition pos);
        /** \brief toggle opened state */
        void toggle();


        void invalidate();
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

        HeaderPosition m_pos;

        QBoxLayout* mainLayout;
        QBoxLayout* headerLayout;
        QWidget* widMain;
        bool privateMain;

    protected:
        virtual void init();
        //virtual void paintEvent(QPaintEvent* event);
        //int getHeaderHeight() const;
        //int getHeaderWidth() const;
        //virtual void myDrawFrame(QPainter *p);
};

#endif // QFCollapsibleScrollArea_H
