#ifndef QFENHANCEDLABEL_H
#define QFENHANCEDLABEL_H

#include <QLabel>
#include "lib_imexport.h"
#include <QUrl>

class QFLIB_EXPORT QFEnhancedLabel : public QLabel
{
        Q_OBJECT
    public:
        explicit QFEnhancedLabel(QWidget *parent=0, Qt::WindowFlags f=0);
        explicit QFEnhancedLabel(const QString &text, QWidget *parent=0, Qt::WindowFlags f=0);
        void setUnderlineOnHover(bool enabled);
        void setOpenUrl(QUrl url);
    signals:
        void clicked();
    public slots:
    protected:
        virtual void mouseReleaseEvent(QMouseEvent* event);
        virtual void leaveEvent(QEvent* event);
        virtual void enterEvent(QEvent* event);

        bool underlineOnHover;
        QUrl openUrl;


};

#endif // QFENHANCEDLABEL_H
