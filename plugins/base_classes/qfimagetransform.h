#ifndef QFIMAGETRANSFORM_H
#define QFIMAGETRANSFORM_H

#include <QObject>
#include <QVector>
#include <QWidget>

class QFImageTransformWidget: public QWidget
{
        Q_OBJECT
    public:
        explicit QFImageTransformWidget(QWidget* parent);
        virtual ~QFImageTransformWidget();
        virtual bool transform(const QVector<double>& input, int width, int height, QVector<double>& output, int& width_out, int& height_out);
};

#endif // QFIMAGETRANSFORM_H
