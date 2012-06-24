#include "qfrdrtabledelegate.h"
#include <QtGui>
#include "numberedit.h"
#include "jkdoubleedit.h"

QFRDRTableDelegate::QFRDRTableDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget *QFRDRTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const {
     if ( index.data().type() == QVariant::DateTime || index.data().type() == QVariant::Time || index.data().type() == QVariant::Date ) {
         QDateTimeEdit *editor = new QDateTimeEdit(parent);
         //editor->setDisplayFormat("dd/M/yyyy");
         editor->setCalendarPopup(true);
         return editor;
     }
     if ( !index.isValid() || index.data().type() == QVariant::Double ) {
         JKDoubleEdit* editor=new JKDoubleEdit(parent);
         editor->setCheckBounds(false, false);
         return editor;
     }
     if ( index.data().type() == QVariant::Int || index.data().type() == QVariant::LongLong ) {
         QSpinBox* editor=new QSpinBox(parent);
         editor->setRange(INT_MIN, INT_MAX);
         return editor;
     }
     if ( index.data().type() == QVariant::UInt || index.data().type() == QVariant::ULongLong ) {
         QSpinBox* editor=new QSpinBox(parent);
         editor->setRange(0, UINT_MAX);
         return editor;
     }
     if ( index.data().type() == QVariant::Bool) {
         QCheckBox* editor=new QCheckBox(parent);
         return editor;
     }
     QLineEdit *editor = new QLineEdit(parent);

     // create a completer with the strings in the column as model
     QStringList allStrings;
     for (int i = 1; i<index.model()->rowCount(); i++) {
         QString strItem(index.model()->data(index.sibling(i, index.column()),
             Qt::EditRole).toString());

         if (!allStrings.contains(strItem))
             allStrings.append(strItem);
     }

     QCompleter *autoComplete = new QCompleter(allStrings);
     editor->setCompleter(autoComplete);
     connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));
     return editor;
}

void QFRDRTableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    QLineEdit *edit = qobject_cast<QLineEdit*>(editor);
    if (edit) {
        edit->setText(index.model()->data(index, Qt::EditRole).toString());
    } else {
         QDateTimeEdit *dateEditor = qobject_cast<QDateTimeEdit *>(editor);
         if (dateEditor) {
             dateEditor->setDateTime(index.model()->data(index, Qt::EditRole).toDateTime());
         } else {
             JKDoubleEdit *dEditor = qobject_cast<JKDoubleEdit *>(editor);
             if (dEditor) {
                 dEditor->setValue(index.model()->data(index, Qt::EditRole).toDouble());
             } else {
                 QSpinBox *sEditor = qobject_cast<QSpinBox *>(editor);
                 if (sEditor) {
                     sEditor->setValue(index.model()->data(index, Qt::EditRole).toLongLong());
                 } else {
                     QCheckBox *check = qobject_cast<QCheckBox *>(editor);
                     if (check) {
                         check->setChecked(index.model()->data(index, Qt::EditRole).toBool());
                     }
                 }

             }

         }
    }
}

void QFRDRTableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    JKDoubleEdit *dEditor = qobject_cast<JKDoubleEdit *>(editor);
    if (dEditor) {
        QVariant v( dEditor->value());
        v.convert(QVariant::Double);
        qDebug()<<v;
        model->setData(index, v);
    } else {
        QLineEdit *edit = qobject_cast<QLineEdit*>(editor);
        if (edit) {
            model->setData(index, edit->text());
        } else {
             QDateTimeEdit *dateEditor = qobject_cast<QDateTimeEdit *>(editor);
             if (dateEditor) {
                 model->setData(index, dateEditor->date().toString());
             } else {
                QSpinBox *sEditor = qobject_cast<QSpinBox *>(editor);
                 if (sEditor) {
                     model->setData(index, sEditor->value());
                 } else {
                     QCheckBox *check = qobject_cast<QCheckBox *>(editor);
                     if (check) {
                          model->setData(index, check->isChecked());
                     }
                 }

             }

         }
    }
}

void QFRDRTableDelegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
