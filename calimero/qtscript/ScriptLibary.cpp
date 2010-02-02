#include "ScriptLibary.h"
#include <QTextEdit>
#include <QSettings>
#include <QDebug>
#include <QMutexLocker>

ScriptLibary* ScriptLibary::instance = 0;
QMutex* ScriptLibary::mutex = new QMutex(QMutex::Recursive);

ScriptLibary::ScriptLibary()
{
    qDebug() << "ScriptLibary::ScriptLibary()";
    update();
    qDebug() << "ScriptLibary::ScriptLibary() DONE";
}

ScriptLibary::~ScriptLibary()
{
}

bool ScriptLibary::validScriptPath()
{
    QMutexLocker locker(mutex);
    QSettings settings;
    QString dirname = settings.value("scriptpath","").toString();
    if(dirname!="")
    {
        QDir dir(dirname);

        if(dir.exists("comparescripts") && dir.exists("calibrationscripts"))
            return true;
    }
    return false;
}

QScriptValue ScriptLibary::printLog(QScriptContext *context, QScriptEngine *engine)
{
    QString result;
    for (int i = 0; i < context->argumentCount(); ++i)
    {
        if (i > 0)
            result.append(" ");
        result.append(context->argument(i).toString());
    }
    qDebug() << "Script: " << result;
    return engine->undefinedValue();
}

QScriptValue ScriptLibary::printconsole(QScriptContext *context, QScriptEngine *engine)
{
    QString result;
    for (int i = 0; i < context->argumentCount(); ++i)
    {
        if (i > 0)
            result.append(" ");
        result.append(context->argument(i).toString());
    }

    QScriptValue calleeData = context->callee().data();
    QTextEdit *edit = qobject_cast<QTextEdit*>(calleeData.toQObject());
    result = edit->document()->toPlainText() + result + "\n";
    edit->setPlainText(result);
    QTextCursor cursor = edit->textCursor();
    cursor.movePosition(QTextCursor::End,QTextCursor::MoveAnchor);
    edit->setTextCursor(cursor);
    return engine->undefinedValue();
}

ScriptLibary* ScriptLibary::getInstance()
{
    qDebug() << "ScriptLibary::getInstance()";
    QMutexLocker locker(mutex);
    if(instance==NULL)
        instance = new ScriptLibary();
    qDebug() << "ScriptLibary::getInstance DONE()";
    return instance;
}

void ScriptLibary::saveCompareScript(QString algname)
{
    QMutexLocker locker(mutex);
    if(!validScriptPath())
        qWarning("ScriptLibary::saveCompareScript() Scriptpath is not valid. Please start calimero with Gui support and change the scriptpath.");
    QDir dir(getCompareScriptPath());
    dir.mkdir(algname);
    if(!Persistence::saveStringAsFile(comparescriptpath + algname + "/main.qs", "/*main.qs*/"))
    {
        qWarning("ScriptLibary::saveCompareScript() Not able to save script file");
        return;
    }
    update();
}

QString ScriptLibary::getCompareScript(QString name)
{
    QMutexLocker locker(mutex);
    if(!validScriptPath())
        qWarning("ScriptLibary::getCompareScript() Scriptpath is not valid. Please start calimero with Gui support and change the scriptpath.");
    QString result = "";
    if(!Persistence::loadFileAsString(comparescriptpath + name, &result))
        qWarning("ScriptLibary::getCompareScript() Not able to load script file");
    return result;
}

QStringList* ScriptLibary::getCompareScriptNames(QString )
{
    QMutexLocker locker(mutex);
    if(!validScriptPath())
        qWarning("Scriptpath is not valid. Please start calimero with Gui support and change the scriptpath.");
    QStringList *result = new QStringList();
    QDir dir(comparescriptpath);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot  | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        if(QFile::exists(fileInfo.absoluteFilePath() + "/main.qs"))
            result->append(fileInfo.fileName());
    }

    return result;
}

void ScriptLibary::update()
{
    QMutexLocker locker(mutex);
    QSettings settings;
    QString prefix = settings.value("scriptpath","").toString();
    comparescriptpath=QString(prefix + "/comparescripts/");
    calibrationscriptpath=QString(prefix + "/calibrationscripts/");

    if(!validScriptPath())
        qWarning("Scriptpath is not valid. Please start calimero with Gui support and change the scriptpath.");
    comparescripts = getCompareScriptNames(comparescriptpath);
    calibrationscripts = getCalibrationScriptNames(calibrationscriptpath);
}

QStringList* ScriptLibary::getAllCompareScriptNames()
{
    QMutexLocker locker(mutex);
    if(!validScriptPath())
        qWarning("Scriptpath is not valid. Please start calimero with Gui support and change the scriptpath.");
    return comparescripts;
}

bool ScriptLibary::existsCompareScript(QString name)
{
    QMutexLocker locker(mutex);
    qDebug() << "ScriptLibary::existsCompareScript()";

    if(!validScriptPath())
        qWarning("Scriptpath is not valid. Please start calimero with Gui support and change the scriptpath.");
    return comparescripts->contains(name);
}

void ScriptLibary::calibrationVariableFromScriptValue(const QScriptValue &object, CalibrationVariable* &out)
{
    out = qobject_cast<CalibrationVariable*>(object.toQObject());
}

QScriptValue ScriptLibary::calibrationVariableToScriptValue(QScriptEngine *engine, CalibrationVariable* const &in)
{

    return engine->newQObject(in);
}

QScriptValue ScriptLibary::iterationResultToScriptValue(QScriptEngine *engine, IterationResult* const &in)
{
    QScriptValue result = engine->newObject();
    QScriptValue calibrationparameter = qScriptValueFromSequence(engine,in->input.values());
    QScriptValue calculationparameter = qScriptValueFromSequence(engine,in->compare.values());
    result.setProperty("calibrationparameter", calibrationparameter);
    result.setProperty("calculationparameter", calculationparameter);
    return result;
}

 void ScriptLibary::iterationResultFromScriptValue(const QScriptValue &object, IterationResult* &out)
 {
    Q_UNUSED(object);
    Q_UNUSED(out);
    return;
 }

QScriptValue ScriptLibary::variableToScriptValue(QScriptEngine *engine, Variable* const &in)
{
    return qScriptValueFromSequence(engine,in->getValues());
}

void ScriptLibary::variableFromScriptValue(const QScriptValue &object, Variable* &out)
{
    QVector<double> vector(out->getValues());
    vector.clear();
    qScriptValueToSequence(object,vector);
}

void ScriptLibary::saveCalibrationScript(QString algname)
{
    QMutexLocker locker(mutex);
    if(!validScriptPath())
        qWarning("ScriptLibary::saveCalibrationScript() Scriptpath is not valid. Please start calimero with Gui support and change the scriptpath.");
    QDir dir(getCalibrationScriptPath());
    dir.mkdir(algname);
    if(!Persistence::saveStringAsFile(getCalibrationScriptPath() + algname + "/main.qs", "/*main.qs*/"))
        qWarning("ScriptLibary::saveCalibrationScript() Not able to save script file");
    update();
}

QString ScriptLibary::getCalibrationScript(QString name)
{
    QMutexLocker locker(mutex);
    if(!validScriptPath())
        qWarning("ScriptLibary::getCalibrationScript() Scriptpath is not valid. Please start calimero with Gui support and change the scriptpath.");
    QString result = "";
    if(!Persistence::loadFileAsString(calibrationscriptpath + name, &result))
        qWarning("ScriptLibary::getCalibrationScript() Not able to load script file");
    return result;
}

QStringList* ScriptLibary::getCalibrationScriptNames(QString /*path*/)
{
    QMutexLocker locker(mutex);
    if(!validScriptPath())
        qWarning("Scriptpath is not valid. Please start calimero with Gui support and change the scriptpath.");
    QStringList *result = new QStringList();
    QDir dir(calibrationscriptpath);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();

    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);

        if(QFile::exists(fileInfo.absoluteFilePath() + "/main.qs"))
            result->append(fileInfo.fileName());
    }

    return result;
}

QStringList* ScriptLibary::getAllCalibrationScriptNames()
{
    QMutexLocker locker(mutex);
    if(!validScriptPath())
        qWarning("Scriptpath is not valid. Please start calimero with Gui support and change the scriptpath.");
    return calibrationscripts;
}

bool ScriptLibary::existsCalibrationScript(QString name)
{
    QMutexLocker locker(mutex);
    if(!validScriptPath())
        qWarning("Scriptpath is not valid. Please start calimero with Gui support and change the scriptpath.");
    return calibrationscripts->contains(name);
}

QString ScriptLibary::getCompareScriptPath()
{
    QMutexLocker locker(mutex);
    if(!validScriptPath())
        qWarning("Scriptpath is not valid. Please start calimero with Gui support and change the scriptpath.");
    return comparescriptpath;
}

QString ScriptLibary::getCalibrationScriptPath()
{
    QMutexLocker locker(mutex);
    if(!validScriptPath())
        qWarning("Scriptpath is not valid. Please start calimero with Gui support and change the scriptpath.");
    return calibrationscriptpath;
}

QScriptValue ScriptLibary::include(QScriptContext *context, QScriptEngine *engine)
{

    QString result;
    for (int i = 0; i < context->argumentCount(); ++i)
    {
        if (i > 0)
            result.append(" ");
        result.append(context->argument(i).toString());
    }

    QScriptValue calleeData = context->callee().property("WORKINGDIR");
    QString pwd = calleeData.toString();
    QString script = "";

    if(!Persistence::loadFileAsString(pwd + "/" + result,&script))
    {
        engine->evaluate("print(\" " + result + " does not exist\")");
        engine->evaluate("engine.abortCalibration(\"Include Error: File does not exist\")");
        return engine->undefinedValue();
    }

    return engine->evaluate(script,result);
}

QScriptValue ScriptLibary::calculationVariableToScriptValue(QScriptEngine *engine, CalculationVariable* const &in)
{
    return qScriptValueFromSequence(engine,in->getValues());
}

void ScriptLibary::calculationVariableFromScriptValue(const QScriptValue &object, CalculationVariable* &out)
{
    Q_UNUSED(object);
    Q_UNUSED(out);
    qFatal("ScriptLibary::calculationVariableFromScriptValue(): NOT ALLOWED TO CHANGE VALUES OF A CALCULATING PARAMETER INSIDE A SCRIPT");
    return;
}
