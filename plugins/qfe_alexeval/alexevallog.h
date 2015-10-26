/*
Copyright (c) 2012-2015 by Sebastian Isbaner

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ALEXEVALLOG_H
#define ALEXEVALLOG_H
#include <QString>
#include <QTextStream>

class AlexEvalLogStream;

class AlexEvalLog
{

    public:
        AlexEvalLog();
//        AlexEvalLogStream stream(AlexEvalLog *inst = instance);
        inline static AlexEvalLog* getInstance()  { return instance; }
        static void error(const QString& message);
        static void warning(const QString& message);
        static void text(const QString& message);

    protected:
        static AlexEvalLog* instance;

        virtual void intLogError(const QString& message)=0;
        virtual void intLogWarning(const QString& message)=0;
        virtual void intLogText(const QString& message)=0;
};



class AlexEvalLogStream
{
    QString buffer;
    QTextStream ts;
    AlexEvalLog *instance;
public:
    AlexEvalLogStream(AlexEvalLog *instance);
    ~AlexEvalLogStream();

    inline AlexEvalLogStream &maybeSpace() { ts << ' '; return *this; }
//    inline AlexEvalLogStream &maybeSpace() { ts << ' '; buffer.clear(); return *this; }
    inline AlexEvalLogStream &operator<<(QChar t) { ts << '\'' << t << '\''; return maybeSpace(); }
    //inline AlexEvalLogStream &operator<<(QBool t) { ts << (bool(t != 0) ? "true" : "false"); return maybeSpace(); }
    inline AlexEvalLogStream &operator<<(bool t) { ts << (t ? "true" : "false"); return maybeSpace(); }
    inline AlexEvalLogStream &operator<<(char t) { ts << t; return maybeSpace(); }
    inline AlexEvalLogStream &operator<<(signed short t) { ts << t; return maybeSpace(); }
    inline AlexEvalLogStream &operator<<(unsigned short t) { ts << t; return maybeSpace(); }
    inline AlexEvalLogStream &operator<<(signed int t) { ts << t; return maybeSpace(); }
    inline AlexEvalLogStream &operator<<(unsigned int t) { ts << t; return maybeSpace(); }
    inline AlexEvalLogStream &operator<<(signed long t) { ts << t; return maybeSpace(); }
    inline AlexEvalLogStream &operator<<(unsigned long t) { ts << t; return maybeSpace(); }
    inline AlexEvalLogStream &operator<<(qint64 t)
        { ts << QString::number(t); return maybeSpace(); }
    inline AlexEvalLogStream &operator<<(quint64 t)
        { ts << QString::number(t); return maybeSpace(); }
    inline AlexEvalLogStream &operator<<(float t) { ts << t; return maybeSpace(); }
    inline AlexEvalLogStream &operator<<(double t) { ts << t; return maybeSpace(); }
    inline AlexEvalLogStream &operator<<(const char* t) { ts << QLatin1String(t); return maybeSpace(); }
    inline AlexEvalLogStream &operator<<(const QString & t) { ts << '\"' << t  << '\"'; return maybeSpace(); }
    inline AlexEvalLogStream &operator<<(const QStringRef & t) { return operator<<(t.toString()); }
    inline AlexEvalLogStream &operator<<(const QLatin1String &t) { ts << '\"'  << t.latin1() << '\"'; return maybeSpace(); }
    inline AlexEvalLogStream &operator<<(const QByteArray & t) { ts  << '\"' << t << '\"'; return maybeSpace(); }
    inline AlexEvalLogStream &operator<<(const void * t) { ts << t; return maybeSpace(); }
    inline AlexEvalLogStream &operator<<(QTextStreamFunction f) { ts << f; return *this;}
};

#endif // ALEXEVALLOG_H



