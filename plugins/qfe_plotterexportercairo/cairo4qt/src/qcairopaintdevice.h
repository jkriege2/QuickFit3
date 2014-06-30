#ifndef QCAIROPAINTDEVICE_H
#define QCAIROPAINTDEVICE_H

#include <QPaintDevice>
#include <QString>
#include <QSize>

class QCairoPaintEngine;

class QCairoPaintDevice : public QPaintDevice
{
    public:
        enum CairoFileType {
            cftPDF14=0,
            cftPDF15=1,
            cftPDF=cftPDF15,
            cftPS2=10,
            cftPS3=11,
            cftPS=cftPS3,
            cftEPS2=20,
            cftEPS3=21,
            cftEPS=cftEPS3,
            cftSVG11=30,
            cftSVG12=31,
            cftSVG=cftSVG12,
        };

        explicit QCairoPaintDevice(const QSizeF& s, const QString& fileName, CairoFileType filetype);
        virtual ~QCairoPaintDevice();

        virtual QPaintEngine * paintEngine () const;

        CairoFileType getFileType() const;
        QString getFileName() const;
        QSizeF getFileSize() const;
        QSizeF getFileSizeMM() const;

    protected:
        virtual int metric (PaintDeviceMetric metric) const;

    protected:
        QString m_fileName;
        CairoFileType m_filetype;
        QSizeF m_size;
        QCairoPaintEngine* engine;
};



#endif // QCAIROPAINTDEVICE_H