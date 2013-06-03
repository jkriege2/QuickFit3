#ifndef QFFCCSFILESETSMODEL_H
#define QFFCCSFILESETSMODEL_H
#include <QAbstractListModel>
#include "qfrawdatarecord.h"
#include <QModelIndex>
#include <QList>

class QFFCCSFitEvaluationItem; // forward

class QFFCCSFileSetsModel : public QAbstractListModel
{
        Q_OBJECT
    public:
        explicit QFFCCSFileSetsModel(QFFCCSFitEvaluationItem *parent = 0);
        QList<QFRawDataRecord*> getFileSetForIndex(const QModelIndex& idx);
        QList<QFRawDataRecord*> getFileSetForRow(int row) const;

        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
        virtual Qt::ItemFlags flags(const QModelIndex & index ) const;
    public slots:
        void rebuildModel();
    signals:
        
    public slots:

    protected:
        int fittedSets, guessedSets;
        QStringList names;
        QFFCCSFitEvaluationItem* FCCSfit;
        
};

#endif // QFFCCSFILESETSMODEL_H
