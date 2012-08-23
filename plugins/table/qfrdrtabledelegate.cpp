#include "qfrdrtabledelegate.h"
#include <QtGui>
#include "qfdoubleedit.h"

QFRDRTableDelegate::QFRDRTableDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget *QFRDRTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const {
    QVariant dat=index.data(Qt::DisplayRole);
    if (dat.type()==QVariant::Invalid) {
        dat=index.data(Qt::EditRole);
    }
     if ( dat.type() == QVariant::DateTime || dat.type() == QVariant::Time || dat.type() == QVariant::Date ) {
         QDateTimeEdit *editor = new QDateTimeEdit(parent);
         //editor->setDisplayFormat("dd/M/yyyy");
         editor->setCalendarPopup(true);
         return editor;
     }
     if ( !index.isValid() || dat.type() == QVariant::Double ) {
         QFDoubleEdit* editor=new QFDoubleEdit(parent);
         editor->setCheckBounds(false, false);
         return editor;
     }
     if ( dat.type() == QVariant::Int || dat.type() == QVariant::LongLong ) {
         QSpinBox* editor=new QSpinBox(parent);
         editor->setRange(INT_MIN, INT_MAX);
         return editor;
     }
     if ( dat.type() == QVariant::UInt || dat.type() == QVariant::ULongLong ) {
         QSpinBox* editor=new QSpinBox(parent);
         editor->setRange(0, UINT_MAX);
         return editor;
     }
     if ( dat.type() == QVariant::Bool) {
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
             QFDoubleEdit *dEditor = qobject_cast<QFDoubleEdit *>(editor);
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
    QFDoubleEdit *dEditor = qobject_cast<QFDoubleEdit *>(editor);
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
                 model->setData(index, dateEditor->dateTime());
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

void QFRDRTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QItemDelegate::paint(painter, option, index);


    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    QFont f(option.font);
    f.setPointSizeF(7);
    painter->setFont(f);
    painter->setPen(QPen(QColor("blue")));
    QFontMetricsF fm(f);

    QString id="?";
    switch(index.data().type()) {
        case QVariant::Bool: id="b"; break;
        case QVariant::Char: id="c"; break;
        case QVariant::Date: id="d"; break;
        case QVariant::DateTime: id="dt"; break;
        case QVariant::Double: id="f"; break;
        case QVariant::Int: id="i"; break;
        case QVariant::LongLong: id="i"; break;
        case QVariant::String: id="s"; break;
        case QVariant::Time: id="t"; break;
        case QVariant::UInt: id="u"; break;
        case QVariant::ULongLong: id="u"; break;
    }

    QColor c("white");
    c.setAlphaF(0.3);
    painter->fillRect(QRectF(option.rect.right()-fm.width(id)-4, option.rect.bottom()-fm.height()-4, fm.width(id)+2, fm.height()+2), QBrush(c));
    painter->drawText(option.rect.right()-fm.width(id)-2, option.rect.bottom()-fm.height()-2+fm.ascent(), id);
    painter->restore();
}

void QFRDRTableDelegate::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
