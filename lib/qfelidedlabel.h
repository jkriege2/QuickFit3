#ifndef QFELIDEDLABEL_H
#define QFELIDEDLABEL_H
#include "lib_imexport.h"
#include <QLabel>

class QFLIB_EXPORT QFElidedLabel : public QLabel
{
        Q_OBJECT
        Q_PROPERTY(bool isElided READ isElided)
        Q_PROPERTY(Qt::TextElideMode m_elidedMode READ elidedMode() WRITE setElidedMode())
    public:
        explicit QFElidedLabel(QWidget *parent=0, Qt::WindowFlags f=0);
        explicit QFElidedLabel(const QString &text, QWidget *parent=0, Qt::WindowFlags f=0);
        bool isElided() const { return elided; }
        Qt::TextElideMode elidedMode() const;
    public slots:
        void setElided(bool elided);
        void setElidedMode(Qt::TextElideMode elidedMode);
    protected:
        void paintEvent(QPaintEvent *event);


    private:
        bool elided;
        Qt::TextElideMode m_elidedMode;
        
};

#endif // QFELIDEDLABEL_H
