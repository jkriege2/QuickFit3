#ifndef QVISIBLEHANDLESPLITTER_H
#define QVISIBLEHANDLESPLITTER_H

#include <QSplitter>
#include <QSplitterHandle>
#include "lib_imexport.h"

/*! \brief a QSplitter that has a really visible Splitter handle
    \ingroup tools_qt


 */
class QFLIB_EXPORT QVisibleHandleSplitter : public QSplitter {
        Q_OBJECT
    public:
        explicit QVisibleHandleSplitter(Qt::Orientation orientation, QWidget *parent = 0);
        explicit QVisibleHandleSplitter(QWidget *parent = 0);
        void setDecorated(bool enabled);
        void setHoverEffect(bool enabled);
        void setGripWidth(int width);
    signals:

    public slots:

    protected:
        QSplitterHandle *createHandle();

};



/*! \brief a QSplitterHandle that is really visible Splitter handle (used by QVisibleHandleSplitter)
    \ingroup tools_qt


 */
class QFLIB_EXPORT QVisibleGripSplitterHandle: public QSplitterHandle {
        Q_OBJECT
    public:
        QVisibleGripSplitterHandle(Qt::Orientation orientation, QSplitter * parent=NULL);
        void setDecorated(bool enabled) { m_drawGrip=enabled; }
        bool decorated() const { return m_drawGrip; }
        void setHoverEffect(bool enabled) { m_lightup=enabled; }
        bool hoverEffect() const { return m_lightup; }
        void setGripWidth(int width) { m_gripHeight=width; }
        int gripWidth() const { return m_gripHeight; }
    protected:
        virtual void leaveEvent(QEvent* event);
        virtual void enterEvent(QEvent* event);
        virtual void paintEvent(QPaintEvent* event);
        int m_gripHeight;
        bool m_inside;
        bool m_lightup;
        bool m_drawGrip;
};

#endif // QVISIBLEHANDLESPLITTER_H
