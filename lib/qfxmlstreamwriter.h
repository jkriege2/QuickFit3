#ifndef QFXMLSTREAMWRITER_H
#define QFXMLSTREAMWRITER_H

#include <QXmlStreamWriter>
#include <QString>
#include <QTextCodec>

class QFXMLStreamWriter  {
    public:
        explicit QFXMLStreamWriter(QIODevice * device );


        void setCodec(QTextCodec *codec);
        void setCodec(const char *codecName);
        QTextCodec *codec() const;

        void setAutoFormatting(bool);
        bool autoFormatting() const;

        void setAutoFormattingIndent(int spacesOrTabs);
        int autoFormattingIndent() const;

        void writeAttribute(const QString &qualifiedName, const QString &value);
        void writeAttribute(const QString &namespaceUri, const QString &name, const QString &value);
        void writeAttribute(const QXmlStreamAttribute& attribute);
        void writeAttributes(const QXmlStreamAttributes& attributes);

        void writeCDATA(const QString &text);
        void writeCharacters(const QString &text);
        void writeComment(const QString &text);

        void writeDTD(const QString &dtd);

        void writeEmptyElement(const QString &qualifiedName);
        void writeEmptyElement(const QString &namespaceUri, const QString &name);

        void writeTextElement(const QString &qualifiedName, const QString &text);
        void writeTextElement(const QString &namespaceUri, const QString &name, const QString &text);

        void writeEndDocument();
        void writeEndElement();

        void writeEntityReference(const QString &name);
        void writeNamespace(const QString &namespaceUri, const QString &prefix = QString());
        void writeDefaultNamespace(const QString &namespaceUri);
        void writeProcessingInstruction(const QString &target, const QString &data = QString());

        void writeStartDocument();
        void writeStartDocument(const QString &version);
        void writeStartDocument(const QString &version, bool standalone);
        void writeStartElement(const QString &qualifiedName);
        void writeStartElement(const QString &namespaceUri, const QString &name);

    #ifndef QT_NO_XMLSTREAMREADER
        void writeCurrentToken(const QXmlStreamReader &reader);
    #endif

        bool hasError() const;

    private:
        QIODevice* m_device;
        
};

#endif // QFXMLSTREAMWRITER_H
