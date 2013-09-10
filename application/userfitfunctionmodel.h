#ifndef USERFITFUNCTIONMODEL_H
#define USERFITFUNCTIONMODEL_H

#include <QAbstractTableModel>
#include <QSettings>
#include <QStringList>

class UserFitFunctionModel : public QAbstractTableModel
{
        Q_OBJECT
    public:
        explicit UserFitFunctionModel(QObject *parent = 0);
        void loadSettings(QSettings& settings);
        void storeSettings(QSettings& settings) const;

        void deleteParameter(int idx);
        void addParameter();
        void clear();
        void moveUp(int idx);
        void moveDown(int idx);
        QStringList getIDs() const;


        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;

        Qt::ItemFlags flags(const QModelIndex &index) const;

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    signals:
        
    public slots:
    protected:
        struct FPItem {
            QString id;
            QString name;
            QString label;
            QString unit;
            QString unitLabel;
            QString type;
            bool fit;
            bool userEditable;
            bool userRangeEditable;
            bool initialFix;
            double initialValue;
            double maxValue;
            double minValue;
            double absMaxValue;
            double absMinValue;
            double inc;
        };
        QList<FPItem> items;
        
};

#endif // USERFITFUNCTIONMODEL_H
