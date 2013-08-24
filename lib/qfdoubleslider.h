#ifndef QFDOUBLESLIDER_H
#define QFDOUBLESLIDER_H

#include <QSlider>

#include "lib_imexport.h"

/*! \brief a slider that support double-values
    \ingroup qf3lib_widgets

 */
class QFLIB_EXPORT QFDoubleSlider : public QSlider
{
        Q_OBJECT
    public:
        explicit QFDoubleSlider(QWidget *parent = 0);
        double dvalue() const;
    signals:
        void dsliderMoved(double value);
    public slots:
        void setDRange(double min, double max);
        void setDValue(double value);
        void setDMinimum(double min);
        void setDMaximum(double max);
    protected slots:
        void dsliderMovedSlot(int pos);
    protected:
        virtual void sliderChange(SliderChange change);
        double m_value;
        double d_min;
        double d_max;
        int m_steps;
        
};

#endif // QFDOUBLESLIDER_H
