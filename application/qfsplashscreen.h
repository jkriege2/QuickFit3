#ifndef QFSPLASHSCREEN_H
#define QFSPLASHSCREEN_H
#include <QColor>
#include <QSplashScreen>

class QFSplashScreen : public QSplashScreen
{
        Q_OBJECT
    public:
        explicit QFSplashScreen(const QPixmap & pixmap = QPixmap(), Qt::WindowFlags f = 0);
        explicit QFSplashScreen(QWidget * parent, const QPixmap & pixmap = QPixmap(), Qt::WindowFlags f = 0);
        ~QFSplashScreen();

    signals:

    public slots:
        void incProgress();
        void setProgressValue(int value);
        void setProgressRange(int min, int max);
        void activateProgress(bool enabled=true);
    protected:
        int m_pmin;
        int m_pmax;
        int m_pvalue;
        bool m_progress;
        QColor m_progressColor;
        QColor m_progressBackground;
        QColor m_progressBorder;
        int m_progressWidth;
        int m_progressHeight;
        int m_progressMargin;


        virtual void drawContents(QPainter *painter);
    private:
        void init();
};

#endif // QFSPLASHSCREEN_H
