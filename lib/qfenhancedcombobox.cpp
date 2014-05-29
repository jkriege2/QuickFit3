#include "qfenhancedcombobox.h"

QFEnhancedComboBox::QFEnhancedComboBox(QWidget *parent) :
    QComboBox(parent)
{
}

QVariant QFEnhancedComboBox::currentData(int role) const
{
    return itemData(currentIndex(), role);
}

void QFEnhancedComboBox::setCurrentData(const QVariant &data, int role)
{
    int idx=findData(data, role);
    setCurrentIndex(idx);
}
