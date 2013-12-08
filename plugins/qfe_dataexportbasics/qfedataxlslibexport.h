#ifndef QFEDATAXLSLIBEXPORT_H
#define QFEDATAXLSLIBEXPORT_H

#include "datatools.h"

class QFEDataXlsLibExport: public QFDataExportHandler::DataWriter

{
    public:
        QFEDataXlsLibExport(bool flipped=false);
        virtual QString getFilter() const;
        virtual void save(const QList<QList<QVariant> > &data, const QString& filename, const QStringList& columnHeaders=QStringList(), const QStringList& rowHeaders=QStringList());
    protected:
        bool flipped;
};

#endif // QFEDATAXLSLIBEXPORT_H
