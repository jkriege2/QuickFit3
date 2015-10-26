#ifndef NICHANNELSMODEL_H
#define NICHANNELSMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QStyledItemDelegate>
#include <QSettings>

enum NIChannelType {
    NIC_Analog_SingleEnded,
    NIC_Analog_NR_SingleEnded,
    NIC_Analog_Differential,
    NIC_Analog_Pseudo_Differential,
};

QString NIChannelTypeToQString(NIChannelType t);
NIChannelType QStringToNIChannelType(QString t);

struct NIChannel {
    QString physicalChannel;
    QString name;
    double minVal;
    double maxVal;
    NIChannelType type;
    double linTransformOffset;
    double linTransformSlope;
};

class NIChannelsModel : public QAbstractTableModel
{
        Q_OBJECT
    public:
        explicit NIChannelsModel(QObject *parent = 0);
        virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
        virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
        virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
        virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
        virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
        virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

        void storeSettings(QSettings& set, const QString& prefix=QString("NIChannels/"));
        void loadSettings(QSettings& set, const QString& prefix=QString("NIChannels/"));

        QList<NIChannel> getChannels() const;
        NIChannel getChannel( int i) const;
        int getChannelCount() const;
    signals:

    public slots:
        void addChannel();
        void removeChannel(int i);
        void removeChannel(QList<int> i);
    protected:
        QList<NIChannel> channels;

};



class NIChannelsDelegate : public QStyledItemDelegate {
        Q_OBJECT
    public:
        NIChannelsDelegate(QObject* parent=NULL);

        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
        void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // NICHANNELSMODEL_H
