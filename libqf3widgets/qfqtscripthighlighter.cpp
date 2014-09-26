/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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


/*
  Name: sdffhighlighter.cpp
  Copyright: (c) 2008
  Author: Jan krieger <jan@jkrieger.de>, http://www.jkrieger.de/

*/
 #include <QtGui>

 #include "qfqtscripthighlighter.h"
#include "programoptions.h"

 QFQtScriptHighlighter::QFQtScriptHighlighter(const QString &settingsDir, QTextDocument *parent)
     : QSyntaxHighlighter(parent)
 {
     HighlightingRule rule;
     QString prefix="";

     // open the sdff-editor ini file and load the highlighting formats
     QString sd=settingsDir;
     if (sd.isEmpty()) {
         sd=ProgramOptions::getInstance()->getAssetsDirectory()+"/highlighter/qtscript/";
     }
     QSettings settings(sd+"/highlight.ini", QSettings::IniFormat);
     loadFormat(settings, prefix+"style/keywords", keywordFormat, "blue", true, false, false);
     loadFormat(settings, prefix+"style/operators", operatorFormat, "red", false, false, false);
     loadFormat(settings, prefix+"style/comments", singleLineCommentFormat, "darkgray", false, false, false);
     loadFormat(settings, prefix+"style/multicomments", multiLineCommentFormat, "darkgray", false, false, false);
     loadFormat(settings, prefix+"style/strings", quotationFormat, "darkred", false, false, false);
     loadFormat(settings, prefix+"style/functions", functionFormat, "darkblue", false, false, false);
     loadFormat(settings, prefix+"style/variables", variableFormat, "black", false, false, false);
     loadFormat(settings, "highlighting/specialfunctions", specialfunctionFormat, "darkblue", true, false, false);

     // recognizes variables
     rule.pattern = QRegExp(settings.value(prefix+"rules/variables", "\\b[A-Za-z0-9_]+(?!\\()").toString());
     rule.format = variableFormat;
     highlightingRules.append(rule);

     // recognizes functions
     rule.pattern = QRegExp(settings.value(prefix+"rules/functions", "\\b[A-Za-z0-9_]+(?=\\()").toString());
     rule.format = functionFormat;
     highlightingRules.append(rule);




     // create a list of all valid keywords of SDFF
     // here the assertion \b (in C string constants typed as \\b, as we use the \-escape sequence here!)
     // says "word boundary", so \bname\b will only math 'name', not 'myname', as there is no word boundary
     // in front of the secont 'name' (see QRegExp documentation for details).
     QStringList keywordPatterns=settings.value(prefix+"lists/keywords", "null,true,false,break,case,catch,continue,debugger,default,delete,do,else,finally,for,function,if,in,instanceof,new,return,switch,this,throw,try,typeof,var,void,while,with").toString().split(",");
     foreach (QString pattern, keywordPatterns) {
         rule.pattern = QRegExp("\\b"+pattern.trimmed()+"\\b");
         //qDebug()<<"keyword: "<<rule.pattern.pattern();
         rule.format = keywordFormat;
         highlightingRules.append(rule);
     }

     // create a list of all valid operators (just the same as above)
     QStringList operatorPatterns=settings.value(prefix+"lists/operators", "+,*,/,-,~,!,%,<<,>>,>>>,<,>,<=,>=,==,!=,++,--,+=,-=,/=,*=,^,^=,|,|=,||,&,&&,&=,>>>=,%=").toString().split(",");
     foreach (QString pattern, operatorPatterns) {
         rule.pattern = QRegExp(pattern.trimmed().replace("+", "\\+").replace("*", "\\*").replace("^", "\\^").replace("$", "\\$").replace("?", "\\?").replace("|", "\\|").replace("%", "\\%").replace("&", "\\&").replace("!", "\\!").replace("/", "\\/").replace("~", "\\~"));
         //qDebug()<<"operator: "<<rule.pattern.pattern();
         rule.format = operatorFormat;
         highlightingRules.append(rule);
     }

     // implements a rule for single line comments
     rule.pattern = QRegExp(settings.value(prefix+"rules/comments", "//[^\n]*").toString());
     rule.format = singleLineCommentFormat;
     highlightingRules.append(rule);

     // ruls for multi-line comments
     commentStartExpression = QRegExp(settings.value(prefix+"rules/comment_start", "/\\*").toString());
     commentEndExpression = QRegExp(settings.value(prefix+"rules/comment_end", "\\*/").toString());

     // rules for quoted strings
     rule.pattern = QRegExp("\".*\"");
     rule.format = quotationFormat;
     highlightingRules.append(rule);
     rule.pattern = QRegExp("\'.*\'");
     rule.format = quotationFormat;
     highlightingRules.append(rule);
 }

 void QFQtScriptHighlighter::setSpecialFunctions(const QStringList &specialfunctions)
 {

    sfhighlightingRules.clear();
     // recognizes special functions
     if (specialfunctions.size()>0) {
         QString rx="\\b(";
         rx+=specialfunctions.join("|").replace('.', "\\.").replace('(', "\\(").replace(')', "\\)");
         rx+=")(?=\\()";
         HighlightingRule rule;
         rule.pattern = QRegExp(rx);
         //qDebug()<<"specfunc: "<<rule.pattern.pattern();
         rule.format = specialfunctionFormat;
         sfhighlightingRules.append(rule);
     }
 }

 void QFQtScriptHighlighter::loadFormat(QSettings& settings, QString key, QTextCharFormat& format, QString default_fcolor, bool default_bold, bool default_italic, bool default_underlined){
     bool bold=settings.value(key+".bold", QVariant(default_bold)).toBool();
     bool italic=settings.value(key+".italic", QVariant(default_italic)).toBool();
     bool underlined=settings.value(key+".underlined", QVariant(default_underlined)).toBool();
     QString col=settings.value(key+".color", default_fcolor).toString();

     format.setForeground(QColor(col));
     format.setFontItalic(italic);
     format.setFontWeight((bold)?(QFont::Bold):(QFont::Normal));
     format.setFontUnderline(underlined);
     format.setFontFixedPitch(settings.value("editor/fixedpitch", true).toBool());
     format.setFontFamily(settings.value("editor/fontname", "Courier New").toString());
     format.setFontPointSize(settings.value("editor/fontsize", 10).toDouble());

}

 void QFQtScriptHighlighter::highlightBlock(const QString &text)
 {
     if (text.isEmpty() || text.size()<2) return;
     // test all rules whether (and where) they apply
     QVector<HighlightingRule> hr=highlightingRules;
     hr<<sfhighlightingRules;
     foreach (HighlightingRule rule, hr) {
         QRegExp expression(rule.pattern);
         int index = text.indexOf(expression);
         while (index >= 0) {
             int length = expression.matchedLength();
             setFormat(index, length, rule.format);
             index = text.indexOf(expression, index + length);
         }
     }
     setCurrentBlockState(0); // by default the current block is not ended by a comment (i.e. we are not
                              // ninside a block-comment.

     // handle block comments
     //   previousBlockState() is 1 if we are inside a comment, it is 0 if we are not inside a
     //   multi-line comment
     int startIndex = 0;
     if (previousBlockState() != 1)
         startIndex = text.indexOf(commentStartExpression); // try to find a comment if we are NOT already i one

     while (startIndex >= 0) {
         // if we found a comment start, we have to find the end of the comment
         int endIndex = text.indexOf(commentEndExpression, startIndex);
         int commentLength;
         // if the end is NOT part of the current text block, we store the state of beeing inside
         // a comment in the currentBlockState (store as 1)
         if (endIndex == -1) {
             setCurrentBlockState(1);
             commentLength = text.length() - startIndex;
         } else {
             commentLength = endIndex - startIndex
                             + commentEndExpression.matchedLength();
         }
         // now we can set the comment format for the current block.
         setFormat(startIndex, commentLength, multiLineCommentFormat);
         // and look for the next comment start in the current block
         startIndex = text.indexOf(commentStartExpression,
                                                 startIndex + commentLength);
     }
 }
