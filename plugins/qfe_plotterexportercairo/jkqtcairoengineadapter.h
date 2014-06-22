#ifndef JKQTPCairoEngineAdapter_H
#define JKQTPCairoEngineAdapter_H

#include <QString>
#include <QStringList>
#include "jkqtpbaseplotter.h"

class JKQTPCairoEngineAdapter : public JKQtBasePlotter::JKQTPPaintDeviceAdapter
{
    public:
        JKQTPCairoEngineAdapter();

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

#endif // JKQTPCairoEngineAdapter_H
