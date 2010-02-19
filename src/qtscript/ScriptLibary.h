#ifndef SCRIPTLIBARY_H_INCLUDED
#define SCRIPTLIBARY_H_INCLUDED

//#define DEBUG

#include "../persistence/Persistence.h"
#include <QtScript>
#include "../core/CalculationVariable.h"
#include "../core/CalibrationVariable.h"
#include "../core/IterationResult.h"
#include <QMutex>

Q_DECLARE_METATYPE(QVector<CalibrationVariable*>)

class ScriptLibary
{
    private:
        QString comparescriptpath;
        QString calibrationscriptpath;
        QStringList *comparescripts;
        QStringList *calibrationscripts;
        static ScriptLibary *instance;
        static QMutex *mutex;

    protected:
        ScriptLibary();

    public:
        ~ScriptLibary();
        static QScriptValue printconsole(QScriptContext *context, QScriptEngine *engine);
        static QScriptValue printLog(QScriptContext *context, QScriptEngine *engine);
        static QScriptValue include(QScriptContext *context, QScriptEngine *engine);
        static ScriptLibary* getInstance();
        void saveCompareScript(QString algname);
        void saveCalibrationScript(QString algname);
        QString getCompareScript(QString name);
        QString getCalibrationScript(QString name);
        QString getCompareScriptPath();
        QString getCalibrationScriptPath();
        QStringList* getCompareScriptNames(QString path);
        QStringList* getCalibrationScriptNames(QString path);
        void update();
        QStringList* getAllCompareScriptNames();
        bool validScriptPath();
        QStringList* getAllCalibrationScriptNames();
        bool existsCompareScript(QString name);
        bool existsCalibrationScript(QString name);
        static QScriptValue calibrationVariableToScriptValue(QScriptEngine *engine, CalibrationVariable* const &in);
        static QScriptValue iterationResultToScriptValue(QScriptEngine *engine, IterationResult* const &in);
        static void iterationResultFromScriptValue(const QScriptValue &object, IterationResult* &out);
        static void calibrationVariableFromScriptValue(const QScriptValue &object, CalibrationVariable* &out);
        static QScriptValue variableToScriptValue(QScriptEngine *engine, Variable* const &in);
        static void variableFromScriptValue(const QScriptValue &object, Variable* &out);
        static QScriptValue calculationVariableToScriptValue(QScriptEngine *engine, CalculationVariable* const &in);
        static void calculationVariableFromScriptValue(const QScriptValue &object, CalculationVariable* &out);
};


#endif // SCRIPTLIBARY_H_INCLUDED
