#ifndef JKQTPLatexEngineAdapter_H
#define JKQTPLatexEngineAdapter_H

#include "QTeXEngine/src/QTeXEngine.h"
#include <QString>
#include <QStringList>
#include "jkqtpbaseplotter.h"

class JKQTPLatexEngineAdapter : public JKQtBasePlotter::JKQTPPaintDeviceAdapter
{
    public:
        JKQTPLatexEngineAdapter(bool docMode=false, bool colorMode=true, QTeXPaintDevice::OutputMode outputMode=QTeXPaintDevice::Tikz);

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
        bool docMode;
        bool colorMode;
        QTeXPaintDevice::OutputMode outputMode;
        QString id;
};

#endif // JKQTPLatexEngineAdapter_H
