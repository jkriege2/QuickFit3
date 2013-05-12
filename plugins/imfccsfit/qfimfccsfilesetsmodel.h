#ifndef QFIMFCCSFILESETSMODEL_H
#define QFIMFCCSFILESETSMODEL_H
#include <QAbstractListModel>
#include "qfrawdatarecord.h"
#include <QModelIndex>
#include <QList>

class QFImFCCSFitEvaluationItem; // forward

class QFImFCCSFileSetsModel : public QAbstractListModel
{
        Q_OBJECT
    public:
        explicit QFImFCCSFileSetsModel(QFImFCCSFitEvaluationItem *parent = 0);
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
        QFImFCCSFitEvaluationItem* imfccsfit;
        
};

#endif // QFIMFCCSFILESETSMODEL_H
