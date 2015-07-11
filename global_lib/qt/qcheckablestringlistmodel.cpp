#include "qcheckablestringlistmodel.h"
#include <QDebug>
QCheckableStringListModel::QCheckableStringListModel(QObject *parent) :
    QStringListModel(parent)
{
    m_check.clear();
    m_editable=false;
}

void QCheckableStringListModel::setChecked(int i) {
    m_check.insert(i);
}

void QCheckableStringListModel::setChecked(int i, bool c) {
    if (c) m_check.insert(i);
    else m_check.remove(i);
}

void QCheckableStringListModel::setUnChecked(int i) {
    m_check.remove(i);
}

bool QCheckableStringListModel::isChecked(int i) const {
    qDebug()<<i<<m_check;
    return m_check.contains(i);
}

void QCheckableStringListModel::unCheckAll() {
    m_check.clear();
}

void QCheckableStringListModel::checkAll() {
    m_check.clear();
    for (int i=0; i<stringList().size(); i++) {
        m_check.insert(i);
    }
}

QVariant QCheckableStringListModel::data(const QModelIndex &index, int role) const {
    if (role==Qt::CheckStateRole) {
        if (isChecked(index.row())) return QVariant(Qt::Checked);
        else return QVariant(Qt::Unchecked);
    } else {
        return QStringListModel::data(index, role);
    }
}

bool QCheckableStringListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role==Qt::CheckStateRole) {
        setChecked(index.row(), value.toBool());
        return true;
    } else {
        return QStringListModel::setData(index, value, role);
    }
}

Qt::ItemFlags QCheckableStringListModel::flags(const QModelIndex &index) const {
    if (m_editable) return Qt::ItemIsUserCheckable|Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable;
    else return Qt::ItemIsUserCheckable|Qt::ItemIsSelectable|Qt::ItemIsEnabled;
}

void QCheckableStringListModel::setEditable(bool editable) {
    m_editable=editable;
}
