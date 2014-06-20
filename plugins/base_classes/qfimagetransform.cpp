#include "qfimagetransform.h"



QFImageTransformWidget::QFImageTransformWidget(QWidget *parent)
{

}

QFImageTransformWidget::~QFImageTransformWidget()
{

}

bool QFImageTransformWidget::transform(const QVector<double> &input, int width, int height, QVector<double> &output, int &width_out, int &height_out)
{
    output=input;
    width_out=width;
    height_out=height;
    return false;
}
