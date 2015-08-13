#ifndef QFFRAMERANGEEDIT_H
#define QFFRAMERANGEEDIT_H

#include <QWidget>
#include "libwid_imexport.h"
#include <QSpinBox>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>

class QFWIDLIB_EXPORT QFFrameRangeEdit : public QWidget
{
        Q_OBJECT
    public:
        explicit QFFrameRangeEdit(QWidget *parent = 0);
        ~QFFrameRangeEdit();

        int getFirst() const;
        int getLast() const;
        void addLayoutStretchAtEnd();
        void setFirstLastText(const QString& first, const QString& last);
        void addWidget(QWidget* widget);

    public slots:
        void setPixelSize(double width_and_height);
        void setPixelSize(QSizeF size);
        void setPixelSize(double width, double height);
        void setUnits(const QString& unitname);
        void setRange(int minRange, int maxRange);

    signals:
        void valueChanged(int first, int last);
    protected slots:
        void spinValueChanged();
        void checkChanged();
    protected:
        QSpinBox* spinFirst;
        QSpinBox* spinLast;
        QCheckBox* chkFirst;
        QCheckBox* chkLast;
        QHBoxLayout* lay;
};

#endif // QFFRAMERANGEEDIT_H
