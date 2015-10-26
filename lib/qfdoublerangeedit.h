#ifndef QFDOUBLERANGEEDIT_H
#define QFDOUBLERANGEEDIT_H


#include <QWidget>
#include "lib_imexport.h"
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include "qfdoubleedit.h"

class QFLIB_EXPORT QFDoubleRangeEdit : public QWidget
{
        Q_OBJECT
    public:
        explicit QFDoubleRangeEdit(QWidget *parent = 0);
        virtual ~QFDoubleRangeEdit();

        double getMin() const;
        double getMax() const;
        void addLayoutStretchAtEnd();
        void addWidget(QWidget* widget);

    public slots:
        void setAbsRange(double min, double max);
        void setValues(double min, double max);
        void setNoAbsRange();
        void setSuffix(const QString& unitname);
        void setDecimals(int dec);

    signals:
        void valueChanged(double min, double max);
    protected slots:
        void spinValueChanged();
    protected:
        QFDoubleEdit* spinMin;
        QFDoubleEdit* spinMax;
        QHBoxLayout* lay;
        QLabel* labUnit;
};

#endif // QFDOUBLERANGEEDIT_H
