#ifndef QFRDRIMAGINGFCSOVERLAYSTYLECOMBOBOX_H
#define QFRDRIMAGINGFCSOVERLAYSTYLECOMBOBOX_H

#include <QComboBox>
#include <QList>
#include <QColor>

class JKQTPOverlayImageEnhanced; // forward

class QFRDRImagingFCSOverlayStyleCombobox : public QComboBox
{
        Q_OBJECT
    public:
        explicit QFRDRImagingFCSOverlayStyleCombobox(QWidget *parent = 0);
        
        void setSelectedOverlayStyle(JKQTPOverlayImageEnhanced* plot) const;

    signals:
        
    public slots:
    protected:
        QList<QColor> selColors;
};

#endif // QFRDRIMAGINGFCSOVERLAYSTYLECOMBOBOX_H
