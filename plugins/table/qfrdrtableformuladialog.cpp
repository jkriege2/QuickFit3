#include "qfrdrtableformuladialog.h"
#include "ui_qfrdrtableformuladialog.h"
#include "programoptions.h"
#include "jkmathparser.h"
#include "qfpluginservices.h"

jkMathParser::jkmpResult QFRDRTableFormulaDialog_dummy(jkMathParser::jkmpResult* params, unsigned char n, jkMathParser* p) {
    jkMathParser::jkmpResult res;

    return res;
}

QFRDRTableFormulaDialog::QFRDRTableFormulaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QFRDRTableFormulaDialog)
{
    ui->setupUi(this);
    compExpression=new QFCompleterFromFile(this);
    ui->edtFormula->setCompleter(compExpression);

    QDir().mkpath(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/table/");
    compExpression->setFilename(ProgramOptions::getInstance()->getConfigFileDirectory()+"/completers/table/"+"table_expression.txt");
    QStringList sl=compExpression->stringlistModel()->stringList();
    QStringList defaultWords;

    defaultWords<<"pi";
    defaultWords<<"e";
    defaultWords<<"sqrt2";
    defaultWords<<"version";
    defaultWords<<"log2e";
    defaultWords<<"log10e";
    defaultWords<<"ln2";
    defaultWords<<"ln10";
    defaultWords<<"h";
    defaultWords<<"hbar";
    defaultWords<<"epsilon0";
    defaultWords<<"mu0";
    defaultWords<<"c";
    defaultWords<<"ce";
    defaultWords<<"muB";
    defaultWords<<"muB_eV";
    defaultWords<<"muN";
    defaultWords<<"muN_eV";
    defaultWords<<"me";
    defaultWords<<"mp";
    defaultWords<<"mn";
    defaultWords<<"NA";
    defaultWords<<"kB";
    defaultWords<<"kB_eV";
    defaultWords<<"sinc";
    defaultWords<<"asin";
    defaultWords<<"acos";
    defaultWords<<"atan";
    defaultWords<<"atan2";
    defaultWords<<"sin";
    defaultWords<<"cos";
    defaultWords<<"tan";
    defaultWords<<"sinh";
    defaultWords<<"cosh";
    defaultWords<<"tanh";
    defaultWords<<"log";
    defaultWords<<"log2";
    defaultWords<<"log10";
    defaultWords<<"exp";
    defaultWords<<"sqrt";
    defaultWords<<"sqr";
    defaultWords<<"abs";
    defaultWords<<"if";
    defaultWords<<"erf";
    defaultWords<<"erfc";
    defaultWords<<"lgamma";
    defaultWords<<"tgamma";
    defaultWords<<"j0";
    defaultWords<<"j1";
    defaultWords<<"jn";
    defaultWords<<"y0";
    defaultWords<<"y1";
    defaultWords<<"yn";
    defaultWords<<"rand";
    defaultWords<<"srand";
    defaultWords<<"ceil";
    defaultWords<<"floor";
    defaultWords<<"trunc";
    defaultWords<<"round";
    defaultWords<<"fmod";
    defaultWords<<"min";
    defaultWords<<"max";
    defaultWords<<"floattostr";
    defaultWords<<"booltostr";
    defaultWords<<"gauss";
    defaultWords<<"slit";
    defaultWords<<"theta";
    defaultWords<<"tanc";
    defaultWords<<"sigmoid";
    defaultWords<<"sign";


    defaultWords<<"rows";
    defaultWords<<"columns";
    defaultWords<<"row";
    defaultWords<<"column";
    defaultWords<<"col";
    defaultWords<<"data";
    defaultWords<<"dataleft";

    for (int i=0; i<defaultWords.size(); i++) {
        if (!sl.contains(defaultWords[i])) sl.append(defaultWords[i]);
    }
    qSort(sl);
    compExpression->stringlistModel()->setStringList(sl);


    ui->edtFormula->setText(ProgramOptions::getConfigValue("QFRDRTableFormulaDialog/expression", "sin(row/10*2*pi)").toString());
}

QFRDRTableFormulaDialog::~QFRDRTableFormulaDialog()
{
    ProgramOptions::setConfigValue("QFRDRTableFormulaDialog/expression", ui->edtFormula->text());
    delete ui;
}

QString QFRDRTableFormulaDialog::getExpression() const
{
    return ui->edtFormula->text();
}

void QFRDRTableFormulaDialog::on_edtFormula_textChanged(QString text) {
    try {
        ui->labError->setText(tr("<font color=\"darkgreen\">OK</font>"));
        jkMathParser mp; // instanciate
        mp.addFunction("data", QFRDRTableFormulaDialog_dummy);
        mp.addFunction("dataleft", QFRDRTableFormulaDialog_dummy);
        mp.addVariableDouble("row", 0.0);
        mp.addVariableDouble("rows", 1.0);
        mp.addVariableDouble("col", 0.0);
        mp.addVariableDouble("column", 0.0);
        mp.addVariableDouble("columns", 1.0);
        jkMathParser::jkmpNode* n;
        // parse some numeric expression
        n=mp.parse(getExpression().toStdString());
        delete n;
    } catch(std::exception& E) {
        ui->labError->setText(tr("<font color=\"red\">ERROR:</font> %1").arg(E.what()));
    }

}

void QFRDRTableFormulaDialog::on_btnHelp_clicked()
{
    QFPluginServices::getInstance()->displayHelpWindow(QFPluginServices::getInstance()->getPluginHelpDirectory("table")+"mathparser.html");
}
