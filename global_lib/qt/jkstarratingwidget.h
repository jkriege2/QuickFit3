#ifndef JKSTARRATINGWIDGET_H
#define JKSTARRATINGWIDGET_H

#include <QWidget>
#include <QIcon>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QFrame>

/*! \brief a rating widget which displays five stars where the user may select how many are selected

*/
class JKStarRatingWidget : public QFrame {
        Q_OBJECT
        Q_PROPERTY(int data READ rating WRITE setRating )
        Q_PROPERTY(int rating READ rating WRITE setRating USER true)
    public:
        /** Default constructor */
        JKStarRatingWidget(QWidget* parent=NULL);
        /** Default destructor */
        virtual ~JKStarRatingWidget();
        int rating();
        void setRating(int value);
        int maximum();
        void setMaximum(int value);
        void setFrameStyle(int);
        void setFrameShape(Shape);
        void setFrameShadow(Shadow);
        void setLineWidth(int);
        void setMidLineWidth(int);
        void setFrameRect(const QRect &);
    protected:
        QPixmap starImage;
        QPixmap noStarImage;
        QPixmap darkStarImage;
        int m_rating;
        int m_maximum;
        void setWidgetSizes();
        virtual void paintEvent ( QPaintEvent * event );
        virtual void mouseReleaseEvent ( QMouseEvent * event );
    private:
};

#endif // JKSTARRATINGWIDGET_H
