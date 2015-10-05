#include "nichannelsmodel.h"
#include <QComboBox>
#include <QLineEdit>
#include <QDoubleSpinBox>

NIChannelsModel::NIChannelsModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    addChannel();
}

Qt::ItemFlags NIChannelsModel::flags(const QModelIndex &index) const
{
    int col=index.column();
    int row=index.row();
    return Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable;
}

QVariant NIChannelsModel::data(const QModelIndex &index, int role) const
{
    int col=index.column();
    int row=index.row();
    if (row>=0 && row<channels.size()) {
        if (col==0) {
            if (role==Qt::EditRole || role==Qt::DisplayRole) {
                if (channels[row].name.isEmpty()) return channels[row].physicalChannel;
                else return channels[row].name;
            }
        } else if (col==1) {
            if (role==Qt::EditRole || role==Qt::DisplayRole) {
                return channels[row].physicalChannel;
            }
        } else if (col==2) {
            if (role==Qt::EditRole || role==Qt::DisplayRole) {
                return NIChannelTypeToQString(channels[row].type);
            }
        } else if (col==3) {
            if (role==Qt::EditRole || role==Qt::DisplayRole) {
                return QString("%1 V").arg(channels[row].minVal);
            }
        } else if (col==4) {
            if (role==Qt::EditRole || role==Qt::DisplayRole) {
                return QString("%1 V").arg(channels[row].maxVal);
            }
        } else if (col==5) {
            if (role==Qt::EditRole || role==Qt::DisplayRole) {
                return QString("%1").arg(channels[row].linTransformSlope);
            }
        } else if (col==6) {
            if (role==Qt::EditRole || role==Qt::DisplayRole) {
                return QString("%1").arg(channels[row].linTransformOffset);
            }
        }
    }
    return QVariant();
}

QVariant NIChannelsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation==Qt::Horizontal) {
            int col=section;
            if (col==0) {
                return tr("Name");
            } else if (col==1) {
                return tr("Input Channel");
            } else if (col==2) {
                return tr("Type");
            } else if (col==3) {
                return tr("Minimum");
            } else if (col==4) {
                return tr("Maximum");
            } else if (col==5) {
                return tr("lin. transform: slope");
            } else if (col==6) {
                return tr("lin. transform: offset");
            }
        }
    }
    return QVariant();
}

int NIChannelsModel::rowCount(const QModelIndex &parent) const
{
    return channels.size();
}

int NIChannelsModel::columnCount(const QModelIndex &parent) const
{
    return 7;
}

bool NIChannelsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int col=index.column();
    int row=index.row();
    if (row>=0 && row<channels.size()) {
        if (col==0) {
            if (role==Qt::EditRole || role==Qt::DisplayRole) {
                channels[row].name=value.toString();
                return true;
            }
        } else if (col==1) {
            if (role==Qt::EditRole || role==Qt::DisplayRole) {
                channels[row].physicalChannel=value.toString();
                return true;
            }
        } else if (col==2) {
            if (role==Qt::EditRole || role==Qt::DisplayRole) {
                channels[row].type=QStringToNIChannelType(value.toString());
                return true;
            }
        } else if (col==3) {
            if (role==Qt::EditRole || role==Qt::DisplayRole) {
                channels[row].minVal=value.toDouble();
                return true;
            }
        } else if (col==4) {
            if (role==Qt::EditRole || role==Qt::DisplayRole) {
                channels[row].maxVal=value.toDouble();
                return true;
            }
        } else if (col==5) {
            if (role==Qt::EditRole || role==Qt::DisplayRole) {
                channels[row].linTransformSlope=value.toDouble();
                return true;
            }
        } else if (col==6) {
            if (role==Qt::EditRole || role==Qt::DisplayRole) {
                channels[row].linTransformOffset=value.toDouble();
                return true;
            }
        }
    }
    return false;
}

void NIChannelsModel::storeSettings(QSettings &set, const QString &prefix)
{
    set.setValue(prefix+"channelcount", channels.size());
    for (int i=0; i<channels.size(); i++) {
        QString p=prefix+QString("channel%1/").arg(i);
        set.setValue(p+"name", channels[i].name);
        set.setValue(p+"physicalChannel", channels[i].physicalChannel);
        set.setValue(p+"type", NIChannelTypeToQString(channels[i].type));
        set.setValue(p+"min", channels[i].minVal);
        set.setValue(p+"linTransformSlope", channels[i].linTransformSlope);
        set.setValue(p+"linTransformOffset", channels[i].linTransformOffset);
    }
}

void NIChannelsModel::loadSettings(QSettings &set, const QString &prefix)
{
    beginResetModel();
    channels.clear();
    int cnt=set.value(prefix+"channelcount", 0).toInt();
    for (int i=0; i<cnt; i++) {
        QString p=prefix+QString("channel%1/").arg(i);
        NIChannel c;
        c.name=set.value(p+"name", "/Dev1/ai0").toString();
        c.physicalChannel=set.value(p+"physicalChannel", "/Dev1/ai0").toString();
        c.type=QStringToNIChannelType(set.value(p+"physicalChannel").toString());
        c.minVal=set.value(p+"min", "-10").toDouble();
        c.maxVal=set.value(p+"max", "10").toDouble();
        c.linTransformSlope=set.value(p+"linTransformSlope", "1").toDouble();
        c.linTransformOffset=set.value(p+"linTransformOffset", "0").toDouble();
        channels.append(c);
    }
    endResetModel();
}

QList<NIChannel> NIChannelsModel::getChannels() const
{
    return channels;
}

NIChannel NIChannelsModel::getChannel(int i) const
{
    return channels.value(i);
}

int NIChannelsModel::getChannelCount() const
{
    return channels.size();
}

void NIChannelsModel::addChannel()
{
    NIChannel c;
    c.minVal=-10;
    c.maxVal=10;
    c.physicalChannel="/Dev1/ai0";
    c.name=c.physicalChannel;
    c.type=NIC_Analog_SingleEnded;
    c.linTransformOffset=0;
    c.linTransformSlope=1;
    beginInsertRows(QModelIndex(), channels.size(), channels.size());
    channels.append(c);
    endInsertRows();
}

void NIChannelsModel::removeChannel(int i)
{
    if (i>=0 && i<channels.size()) {
        beginRemoveRows(QModelIndex(), i, i);
        channels.removeAt(i);
        endRemoveRows();
    }
}

void NIChannelsModel::removeChannel(QList<int> idx)
{
    qSort(idx);
    for (int i=idx.size()-1; i>=0; i--) {
        removeChannel(idx[i]);
    }
}


QString NIChannelTypeToQString(NIChannelType t)
{
    switch(t) {
        case NIC_Analog_SingleEnded: return QString("SingleEnded");
        case NIC_Analog_NR_SingleEnded: return QString("NR_SingleEnded");
        case NIC_Analog_Differential: return QString("Differential");
        case NIC_Analog_Pseudo_Differential: return QString("Pseudo_Differential");
        default:
            return QString();
    }
    return QString();
}


NIChannelType QStringToNIChannelType(QString t)
{
    QString tt=t.toLower().trimmed().simplified();
    if (tt=="se" || tt=="singleended") return NIC_Analog_SingleEnded;
    if (tt=="nrse" || tt=="nr_singleended") return NIC_Analog_NR_SingleEnded;
    if (tt=="de" || tt=="diff" || tt=="differential") return NIC_Analog_Differential;
    if (tt=="pde" || tt=="pdiff" || tt=="pseudo_differential") return NIC_Analog_Pseudo_Differential;
    return NIC_Analog_SingleEnded;
}


NIChannelsDelegate::NIChannelsDelegate(QObject *parent):
    QStyledItemDelegate(parent)
{

}

QWidget *NIChannelsDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int col=index.column();
    int row=index.row();
    if (col==0) {
        QLineEdit* edt=new QLineEdit(parent);
        return edt;
    } else if (col==1) {
        QLineEdit* edt=new QLineEdit(parent);
        return edt;
    } else if (col==2) {
        QComboBox* cmb=new QComboBox(parent);
        cmb->setEditable(false);
        cmb->addItem(NIChannelTypeToQString(NIC_Analog_SingleEnded));
        cmb->addItem(NIChannelTypeToQString(NIC_Analog_NR_SingleEnded));
        cmb->addItem(NIChannelTypeToQString(NIC_Analog_Differential));
        cmb->addItem(NIChannelTypeToQString(NIC_Analog_Pseudo_Differential));
        return cmb;
    } else if (col==3 || col==4) {
        QDoubleSpinBox* edt=new QDoubleSpinBox(parent);
        edt->setRange(-1000,1000);
        edt->setDecimals(2);
        edt->setSuffix(tr(" V"));
        return edt;
    } else if (col==5 || col==6) {
        QDoubleSpinBox* edt=new QDoubleSpinBox(parent);
        edt->setRange(-1e10,1e10);
        edt->setDecimals(6);
        edt->setSuffix(tr(""));
        return edt;
    }


    return NULL;
}

void NIChannelsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    int col=index.column();
    int row=index.row();
    if (col==0) {
        QLineEdit* edt=qobject_cast<QLineEdit*>(editor);
        if (edt) edt->setText(index.data().toString());
    } else if (col==1) {
        QLineEdit* edt=qobject_cast<QLineEdit*>(editor);
        if (edt) edt->setText(index.data().toString());
    } else if (col==2) {
        QComboBox* edt=qobject_cast<QComboBox*>(editor);
        if (edt) edt->setCurrentIndex(edt->findText(index.data().toString()));
    } else if (col>=3 && col<=6) {
        QDoubleSpinBox* edt=qobject_cast<QDoubleSpinBox*>(editor);
        if (edt) edt->setValue(index.data().toDouble());
    }
}

void NIChannelsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    int col=index.column();
    int row=index.row();
    if (col==0) {
        QLineEdit* edt=qobject_cast<QLineEdit*>(editor);
        if (edt) model->setData(index, edt->text());
    } else if (col==1) {
        QLineEdit* edt=qobject_cast<QLineEdit*>(editor);
        if (edt) model->setData(index, edt->text());
    } else if (col==2) {
        QComboBox* edt=qobject_cast<QComboBox*>(editor);
        if (edt) model->setData(index, edt->currentText());
    } else if (col>=3 && col<=6) {
        QDoubleSpinBox* edt=qobject_cast<QDoubleSpinBox*>(editor);
        if (edt) model->setData(index, edt->value());
    }
}

void NIChannelsDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}
