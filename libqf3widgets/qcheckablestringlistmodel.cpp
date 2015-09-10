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
    //qDebug()<<i<<m_check;
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

Qt::ItemFlags QCheckableStringListModel::flags(const QModelIndex &/*index*/) const {
    if (m_editable) return Qt::ItemIsUserCheckable|Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable;
    else return Qt::ItemIsUserCheckable|Qt::ItemIsSelectable|Qt::ItemIsEnabled;
}

QList<int> QCheckableStringListModel::getChecked() const
{
    QList<int> res;
    for (int i=0; i<rowCount(); i++) {
        if (isChecked(i)) res<<i;
    }
    return res;
}

QVector<bool> QCheckableStringListModel::getIsChecked() const
{
    QVector<bool> res;
    for (int i=0; i<rowCount(); i++) {
        res<<isChecked(i);
    }
    return res;
}

void QCheckableStringListModel::addItem(const QString &item, bool checked, const QVariant& userData)
{
    int r=rowCount();
    insertRow(r);
    setData(index(r), item);
    if (userData.isValid()) setData(index(r), userData, Qt::UserRole);
    setChecked(r, checked);
}

void QCheckableStringListModel::removeItem(int i)
{
    if (i>=0 && i<rowCount()) {
        QSet<int> ns;
        {
            QSetIterator<int> it(m_check);
            while (it.hasNext()) {
                int rr=it.next();
                if (rr<i) ns.insert(rr);
                else if (rr>i) ns.insert(rr-1);
            }
        }
        m_check=ns;
        removeRow(i);
    }
}

void QCheckableStringListModel::swapItems(int i, int j)
{
    if (i>=0 && j>=0 && i<rowCount() && j<rowCount()) {
        QVariant di=data(index(i), Qt::EditRole);
        QVariant dj=data(index(j), Qt::EditRole);
        QVariant ddi=data(index(i), Qt::DisplayRole);
        QVariant ddj=data(index(j), Qt::DisplayRole);
        QVariant dui=data(index(i), Qt::UserRole);
        QVariant duj=data(index(j), Qt::UserRole);
        bool isCi=isChecked(i);
        bool isCj=isChecked(j);
        setData(index(i), dj);
        setData(index(j), di);
        setData(index(i), duj, Qt::UserRole);
        setData(index(j), dui, Qt::UserRole);
        setData(index(i), ddi, Qt::DisplayRole);
        setData(index(j), ddj, Qt::DisplayRole);
        setChecked(i, isCj);
        setChecked(j, isCi);
    }
}

void QCheckableStringListModel::moveUp(int i)
{
    if (i>=1 && i<rowCount()) {
        swapItems(i, i-1);
    }
}

void QCheckableStringListModel::moveDown(int i)
{
    if (i>=0 && i<rowCount()-1) {
        swapItems(i, i+1);
    }
}

void QCheckableStringListModel::clear()
{

    m_check.clear();
    setStringList(QStringList());
    beginResetModel();
    endResetModel();
}

void QCheckableStringListModel::setEditable(bool editable) {
    m_editable=editable;
}
