#ifndef QFPIXELSIZEEDIT_H
#define QFPIXELSIZEEDIT_H

#include <QWidget>
#include "libwid_imexport.h"
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>

class QFWIDLIB_EXPORT QFPixelSizeEdit : public QWidget
{
        Q_OBJECT
    public:
        explicit QFPixelSizeEdit(QWidget *parent = 0);
        ~QFPixelSizeEdit();

        QSizeF getPixelSize() const;
        double getPixelWidth() const;
        double getPixelHeight() const;
        void addLayoutStretchAtEnd();
        void addWidget(QWidget* widget);

    public slots:
        void setPixelSize(double width_and_height);
        void setPixelSize(QSizeF size);
        void setPixelSize(double width, double height);
        void setUnits(const QString& unitname);
        void setRange(double minWidth, double maxWidth);
        void setDecimals(int dec);

    signals:
        void valueChanged(QSizeF size);
    protected slots:
        void spinValueChanged();
    protected:
        QDoubleSpinBox* spinWidth;
        QDoubleSpinBox* spinHeight;
        QHBoxLayout* lay;
};

#endif // QFPIXELSIZEEDIT_H
