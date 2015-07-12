#ifndef QCHECKABLESTRINGLISTMODEL_H
#define QCHECKABLESTRINGLISTMODEL_H

#include <QStringListModel>
#include <QSet>
#include "libwid_imexport.h"

class QFWIDLIB_EXPORT QCheckableStringListModel : public QStringListModel {
    Q_OBJECT
public:
    explicit QCheckableStringListModel(QObject *parent = 0);

    void setChecked(int i);
    void setChecked(int i, bool c);
    void setUnChecked(int i);
    bool isChecked(int i) const;
    void unCheckAll();
    void checkAll();

    void setEditable(bool editable);
    bool isEditable() const { return m_editable; }

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;


protected:
    QSet<int> m_check;
    bool m_editable;
};

#endif // QCHECKABLESTRINGLISTMODEL_H
