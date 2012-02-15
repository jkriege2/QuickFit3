#include "qfstyleditemdelegate.h"
#include "limits.h"
#include "float.h"

QFItemDelegate::QFItemDelegate(QObject *parent):
    QItemDelegate(parent)
{
}

QWidget *QFItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    switch (index.data(Qt::EditRole).type()) {
        case QVariant::Int:
        case QVariant::LongLong: {
            QSpinBox *editor = new QSpinBox(parent);
            editor->setMinimum(INT_MIN);
            editor->setMaximum(INT_MAX);

            return editor;
        }
        case QVariant::UInt:
        case QVariant::ULongLong: {
            QSpinBox *editor = new QSpinBox(parent);
            editor->setMinimum(0);
            editor->setMaximum(UINT_MAX);

            return editor;
        }
        case QVariant::Double: {
            QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
            editor->setMinimum(-DBL_MAX);
            editor->setMaximum(DBL_MAX);
            editor->setDecimals(10);

            return editor;
        }
        default: return QItemDelegate::createEditor(parent, option, index);

    }
    return QItemDelegate::createEditor(parent, option, index);
}

void QFItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    switch (index.data(Qt::EditRole).type()) {
        case QVariant::UInt:
        case QVariant::ULongLong:
        case QVariant::Int:
        case QVariant::LongLong:
        case QVariant::Double:
            editor->setGeometry(option.rect);
            break;
        default:
            QItemDelegate::updateEditorGeometry(editor, option, index);
            break;
    }
}

void QFItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    switch (index.data(Qt::EditRole).type()) {
        case QVariant::UInt:
        case QVariant::ULongLong:
        case QVariant::Int:
        case QVariant::LongLong: {
                QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
                spinBox->interpretText();
                model->setData(index, spinBox->value(), Qt::EditRole);
            }
            break;
        case QVariant::Double: {
                QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
                spinBox->interpretText();
                model->setData(index, spinBox->value(), Qt::EditRole);
            }
            break;
        default:
            QItemDelegate::setModelData(editor, model, index);
            break;

    }
}

void QFItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    switch (index.data(Qt::EditRole).type()) {
        case QVariant::UInt:
        case QVariant::ULongLong:
        case QVariant::Int:
        case QVariant::LongLong: {
                int value = index.model()->data(index, Qt::EditRole).toInt();

                QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
                spinBox->setValue(value);
            }
            break;
        case QVariant::Double:{
                double value = index.model()->data(index, Qt::EditRole).toDouble();

                QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
                spinBox->setValue(value);
            }
            break;
        default:
            QItemDelegate::setEditorData(editor, index);
            break;
    }
}
