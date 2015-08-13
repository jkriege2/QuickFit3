#ifndef QFCROPPIXELSEDIT_H
#define QFCROPPIXELSEDIT_H

#include <QWidget>
#include "libwid_imexport.h"
#include <QSpinBox>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>

class QFWIDLIB_EXPORT QFCropPixelsEdit : public QWidget
{
        Q_OBJECT
    public:
        explicit QFCropPixelsEdit(QWidget *parent = 0);

        int getX1() const;
        int getX2() const;
        int getY1() const;
        int getY2() const;
        int getWIDTH() const;
        int getHEIGHT() const;
        void addLayoutStretchAtEnd();
        void addWidget(QWidget* widget);

    signals:
        void valueChanged(int x1, int x2, int y1, int y2);
    public slots:
        void spinValueChanged();
        void setImageSize(int width, int height);
        void setCropRange(int x1, int x2, int y1, int y2);
        void setFullImageSize();

    protected:
        void connectWidgets(bool connected=true);
        QSpinBox* spinX1;
        QSpinBox* spinX2;
        QSpinBox* spinY1;
        QSpinBox* spinY2;
        QHBoxLayout* lay;

};

#endif // QFCROPPIXELSEDIT_H
