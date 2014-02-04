#ifndef JKQTPEMFEngineAdapter_H
#define JKQTPEMFEngineAdapter_H

#include "EmfEngine/src/EmfEngine.h"
#include <QString>
#include <QStringList>
#include "jkqtpbaseplotter.h"

class JKQTPEMFEngineAdapter : public JKQtBasePlotter::JKQTPPaintDeviceAdapter
{
    public:
        JKQTPEMFEngineAdapter();

        virtual QString getFilter() const;
        virtual QString getFormatName() const;
        virtual QString getFormatID() const;
        virtual QStringList getFileExtension() const;
        virtual bool getSetAbsolutePaperSize() const;
        virtual double getPrintSizeXInMM() const ;
        virtual double getPrintSizeYInMM() const ;
        virtual bool isPrinter() const;
        virtual QPaintDevice* createPaintdevice(const QString& filename, int widthPix, int heightPix) const;

    protected:
        QString id;
};

#endif // JKQTPEMFEngineAdapter_H
