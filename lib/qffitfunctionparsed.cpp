#include "qffitfunctionparsed.h"

QFFitFunctionParsed::QFFitFunctionParsed(const QString &configfile) :
    QFFitFunction()
{
    m_valid=false;
    m_errors=QObject::tr("unknown error");
    QSettings set(configfile, QSettings::IniFormat);

    m_id=set.value("function/id", "").toString();
    if (!m_id.isEmpty()) {
        m_name=set.value("function/name", m_id).toString();
        m_shortName=set.value("function/short_name", m_name).toString();
        m_expression=set.value("function/expression", "").toString();

        int paramCount=set.value("function/param_count", 0).toInt();
        if (paramCount>0) {
            for (int i=0; i<paramCount; i++) {
                QString grp=QString("parameter%1/").arg(i+1);
                ParameterType type=FloatNumber;
                QString fftype=set.value(grp+"type", "").toString().toLower().trimmed();
                if (fftype=="log") type=LogFloatNumber;
                if (fftype=="int_combo") type=IntCombo;
                if (fftype=="int") type=IntNumber;
                QString ffid=set.value(grp+"id", QString("p%1").arg(i+1)).toString();
                QString ffname=set.value(grp+"name", ffid).toString();
                QString label=set.value(grp+"name_html", ffname).toString();
                QString unit=set.value(grp+"unit", "").toString();;
                QString unitLabel=set.value(grp+"unit_html", unit).toString();;
                bool fit=set.value(grp+"fit", true).toBool();;
                bool userEditable=set.value(grp+"editable", true).toBool();
                bool userRangeEditable=set.value(grp+"range_editable", true).toBool();;
                ErrorDisplayMode displayError=DisplayError;
                bool initialFix=set.value(grp+"init_fix", false).toBool();;
                double initialValue=set.value(grp+"init_value", 0.0).toDouble();;
                double minValue=set.value(grp+"min", -DBL_MAX).toDouble();;
                double maxValue=set.value(grp+"max", DBL_MAX).toDouble();
                double inc=set.value(grp+"inc", 1).toDouble();;
                double absMinValue=set.value(grp+"abs_min", -DBL_MAX).toDouble();
                double absMaxValue=set.value(grp+"abs_max", DBL_MAX).toDouble();
                QStringList comboItems=QStringList();

                addParameter(type, ffid, ffname, label, unit, unitLabel, fit, userEditable, userRangeEditable, displayError, initialFix, initialValue, minValue, maxValue, inc, absMinValue, absMaxValue, comboItems);


            }

            params=QVector<double>(paramCount, 0.0);
            double* p=params.data();
            x=0;
            parser.addVariable("x", QFMathParser::qfmpVariable(&x));
            for (int i=0; i<paramCount; i++) {
                p[i]=getDescription(i).initialValue;
                parser.addVariable(getDescription(i).id, QFMathParser::qfmpVariable(&(p[i])));
            }

            parser.resetErrors();
            QFMathParser::qfmpNode* n=parser.parse(m_expression);
            bcenv=QFMathParser::ByteCodeEnvironment(&parser);
            bcenv.init(&parser);
            pnode=NULL;
            useBytecode=false;
            if (n->createByteCode(bprog, &bcenv)) {
                delete n;
                useBytecode=true;
                m_valid=true;
            } else if (!parser.hasErrorOccured()) {
                pnode=n;
                m_valid=true;
            } else {
                delete n;
                m_errors=QObject::tr("error generating bytecode:\n   + %1").arg(parser.getLastErrors().join("\n   + "));
            }
        } else {
            m_errors=QObject::tr("no parameters specified");
        }
    }
    //qDebug()<<"user FF: "<<m_expression<<"   valid: "<<m_valid;
}

QFFitFunctionParsed::~QFFitFunctionParsed()
{
}


bool QFFitFunctionParsed::isValid() const
{
    return m_valid;
}

QString QFFitFunctionParsed::getErrors() const
{
    return m_errors;
}

bool QFFitFunctionParsed::usesBytecode() const
{
    return useBytecode;
}

QString QFFitFunctionParsed::name() const
{
    return m_name;
}

QString QFFitFunctionParsed::shortName() const
{
    return m_shortName;
}

QString QFFitFunctionParsed::id() const
{
    return m_id;
}


bool QFFitFunctionParsed::get_implementsDerivatives()
{
    return false;
}


QString QFFitFunctionParsed::transformParametersForAdditionalPlot(int plot, double *params)
{
    return "";
}


unsigned int QFFitFunctionParsed::getAdditionalPlotCount(const double *params)
{
    return 0;
}


bool QFFitFunctionParsed::isParameterVisible(int parameter, const double *parameterValues) const
{
    return true;
}


void QFFitFunctionParsed::calcParameter(double *parameterValues, double *error) const
{
}


double QFFitFunctionParsed::evaluate(double t, const double *parameters) const
{
    copyArray(params.data(), parameters, paramCount());
    x=t;
    if (useBytecode){
        return parser.evaluateBytecode(bprog);
    } else if (pnode) {
        qfmpResult res;
        pnode->evaluate(res);
        return res.asNumber();
    }
    return 0;
}
