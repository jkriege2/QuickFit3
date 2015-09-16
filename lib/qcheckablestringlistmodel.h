#ifndef QCHECKABLESTRINGLISTMODEL_H
#define QCHECKABLESTRINGLISTMODEL_H

#include <QStringListModel>
#include <QSet>
#include "lib_imexport.h"
#include <QList>
#include <QVector>
#include <QVariant>

class QFLIB_EXPORT QCheckableStringListModel : public QStringListModel {
    Q_OBJECT
public:
    explicit QCheckableStringListModel(QObject *parent = 0);

    void setChecked(int i);
    void setChecked(int i, bool c);
    void setUnChecked(int i);
    bool isChecked(int i) const;
    void unCheckAll();
    void checkAll();

    void setCheckable(bool en);
    bool isCheckable() const  { return m_checkable; }

    void setEditable(bool editable);
    bool isEditable() const { return m_editable; }

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    QList<int> getChecked() const;
    QVector<bool> getIsChecked() const;

    void addItem(const QString& item, bool checked=false, const QVariant &userData=QVariant());
    void removeItem(int i);
    void swapItems(int i, int j);
    void moveUp(int i);
    void moveDown(int i);

    void clear();

protected:
    QSet<int> m_check;
    bool m_editable;
    bool m_checkable;
};

#endif // QCHECKABLESTRINGLISTMODEL_H
