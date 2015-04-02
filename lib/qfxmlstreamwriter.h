/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
