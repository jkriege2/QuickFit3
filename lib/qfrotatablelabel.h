#ifndef QFENHANCEDLABEL_H
#define QFENHANCEDLABEL_H

#include <QWidget>
#include <QFrame>
#include <QPixmap>
#include <QString>
#include <QTextDocument>
#include "lib_imexport.h"
#include "programoptions.h"

/*! \brief an enhanced label
    \ingroup qf3lib_widgets

 */
class QFLIB_EXPORT QFRotatableLabel : public QFrame
{
        Q_OBJECT
    public:
        enum Rotation {
            NoRotation=0,
            UpsideDownRotation=180,
            ClockwiseRotation=90,
            CounterClockwiseRotation=-90
        };

        explicit QFRotatableLabel(QWidget *parent=0, Qt::WindowFlags f=0);
        explicit QFRotatableLabel(const QString &text, QWidget *parent=0, Qt::WindowFlags f=0);

        Rotation orientation() const;
        QString text() const;
        const QPixmap* pixmap() const;

        Qt::TextFormat textFormat() const;

        Qt::Alignment alignment() const;

        bool wordWrap() const;
        virtual QSize minimumSizeHint() const;
        virtual QSize sizeHint() const;
    signals:
        void mouseEntered();
        void mouseLeft();
        void clicked();
    public slots:
        void setOrientation(Rotation o);
        void setText(const QString& txt);
        void setPixmap(const QPixmap& pix);
        void setTextFormat(Qt::TextFormat);
        void setAlignment(Qt::Alignment);
        void setWordWrap(bool on);
        void clear();
        void setNum(int num);
        void setNum(double num);
    protected:
        virtual void enterEvent( QEvent * ev );
        virtual void leaveEvent ( QEvent * ev );
        virtual void mousePressEvent ( QMouseEvent * event );
        virtual void paintEvent(QPaintEvent* event);

        QRectF layoutRect() const;
    private:
        Rotation m_orientation;
        QString m_text;
        QPixmap m_pix;
        Qt::TextFormat m_textFormat;
        Qt::Alignment m_alignment;
        bool m_wordWrap;
        bool isRichText;
        bool isPixmap;
        QTextDocument document;

};

#endif // QFENHANCEDLABEL_H
