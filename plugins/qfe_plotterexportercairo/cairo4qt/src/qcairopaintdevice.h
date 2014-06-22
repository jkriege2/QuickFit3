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
            cftPDF14,
            cftPDF15,
            cftPDF=cftPDF15,
            cftPS2,
            cftPS3,
            cftPS=cftPS3,
            cftEPS2,
            cftEPS3,
            cftEPS=cftPS3,
            cftSVG11,
            cftSVG12,
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
