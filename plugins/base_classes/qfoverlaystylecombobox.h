#ifndef QFOVERLAYSTYLECOMBOBOX_H
#define QFOVERLAYSTYLECOMBOBOX_H

#include <QComboBox>
#include <QList>
#include <QColor>

class JKQTPOverlayImageEnhanced; // forward

class QFOverlayStyleCombobox : public QComboBox
{
        Q_OBJECT
    public:
        explicit QFOverlayStyleCombobox(QWidget *parent = 0);
        
        void setSelectedOverlayStyle(JKQTPOverlayImageEnhanced* plot) const;

    signals:
        
    public slots:
    protected:
        QList<QColor> selColors;
};

#endif // QFOVERLAYSTYLECOMBOBOX_H
