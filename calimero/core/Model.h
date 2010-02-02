#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <QObject>
#include <QString>
#include <QtContainerFwd>
#include <QVector>
#include <QMutex>
#include "VariableContainer.h"
#include "Variable.h"

class QScriptEngine;
class QStringList;
class CalculationVariable;
class CalibrationVariable;
class IterationResult;
class ParameterPersistenceEvaluator;
class QStringList;
class CoreEngine;

class Model : public QObject
{

    Q_OBJECT

    //Private parameters
    private:
        VariableContainer<CalibrationVariable> *inputvarcontainer;
        VariableContainer<CalculationVariable> *calccontainer;
        VariableContainer<Variable> *resultvarcontainer;
        VariableContainer<Variable> *iterationvarcontainer;



        //all these files contain the init values of parameters
        bool cleariterations;
        bool persistent;
        int maxiterations;
        int cpunum;
        double maxerror;
        CoreEngine *coreengine;
        QString preexec;
        QString preexecargument;
        QString iterationexec;
        QString iterationexecargument;
        QString postexec;
        QString postexecargument;
        QString preworkspace;
        QString iterationworkspace;
        QString postworkspace;

        QString algname;

        QStringList *evaluatinggroups;
        QStringList *notevaluatinggroups;

        QStringList *groupnames;



        QVector<QVector<CalibrationVariable*>*> *groups;
        QVector<CalculationVariable*>* algparameters;

        //all results of each iteration
        QVector<IterationResult*> *iterationresults;


        //ParameterPersistenceEvaluator
        ParameterPersistenceEvaluator *parameterevaluator;

    public:
        QMutex *mutex;

        
    private:
        bool updateIterationParamters(int index);


    //newprivate
    public:
        bool importParameter(QString templatepath, QString importfilename, int type);
        bool existsFileName(QString filename, int type = -1);
        bool removeFileName(QString filename, int type);
        bool removeParameter(Variable *var);
        bool addParameter(Variable *var, QString templatestring, QString templatename);
        bool addTemplate(QString templatename, QString templatestring, int type);
        QString getTemplate(QString templatename);
        Variable* getParameter(int index, QString templatename);
        int getType(QString templatename);


    //public methodes
    public:
        Model(CoreEngine *core);
        ~Model();
        void setAlg(QString name);
        void setCoreEngine(CoreEngine *engine);
        CoreEngine* getCoreEngine();
        QString getAlg();
        int getMaxIterations();
        void setMaxIterations(int iterations);
        QScriptEngine* getScriptEngine();
        int  modelLoad(QString filename, bool forcescriptupdate = false);
        bool modelSave(QString filename);
        bool exportIteration(int iteration, QString directory);
        bool loadIteration(int index);
        int getBestIteration(double *value = NULL);
        bool isPersistent();

        bool setMaxError(double error);
        double getMaxError();

        bool addInputFileName(QString filename, QString text);
        bool addResultFileName(QString filename, QString text);
        bool addIterationFileName(QString filename, QString text);

        bool existsInputFileName(QString filename);
        bool existsResultFileName(QString filename);
        bool existsIterationFileName(QString filename);
        bool existsGroup(QString groupname);
        bool existsParameter(QString parametername);

        bool removeInputFileName(QString filename);
        bool removeResultFileName(QString filename);
        bool removeIterationFileName(QString filename);

        QString getInputFileName(int index);
        QString getResultFileName(int index);
        QString getIterationFileName(int index);

        int numberOfCompareParameters();
        int numberOfInputParameters(QString inputfilename);
        int numberOfResultParameters(QString resultfilename);
        int numberOfIterationParameters(QString iterationfilename);

        bool addCompareParameter(CalculationVariable* var);
        bool addInputParameter(CalibrationVariable* var, QString inputtemplate, QString inputfilename, bool force = true);
        bool addResultParameter(Variable* var, QString resulttemplate, QString resultfilename, bool force = true);
        bool addIterationParameter(Variable* var, QString iterationtemplate, QString iterationfilename, bool force = true);

        bool deleteCompareParameter(QString parametername);
        bool deleteInputParameter(QString parametername, QString inputfilename);
        bool deleteResultParameter(QString parametername, QString resultfilename);
        bool deleteIterationParameter(QString parametername, QString iterationfilename);

        CalibrationVariable* getInputParameter(int index, QString inputfilename);
        Variable* getResultParameter(int index, QString resultfilename);
        Variable* getIterationParameter(int index, QString iterationfilename);
        CalculationVariable* getCompareParameter(int index);

        Variable* getParameter(QString parametername);


        bool importInputParameters(QString templatestring, QString inputfilename);
        bool importResultParameters(QString templatestring, QString resultfilename);
        bool importIterationParameters(QString templatestring, QString iterationfilename);

        QString getInputTemplate(QString inputfilename);
        QString getResultTemplate(QString resultFileName);
        QString getIterationTemplate(QString iterationfilename);

        bool changeInputFileName(QString oldname, QString newname);
        bool changeResultFileName(QString oldname, QString newname);
        bool changeIterationFileName(QString oldname, QString newname);

        int numberOfInputFiles();
        int numberOfResultFiles();
        int numberOfIterationFiles();

        bool isFreeParameterName(QString name);
        void clear();

        QVector<CalibrationVariable*> getAllCalibrationParameters();
        QVector<Variable*> getAllResultParameters();
        QVector<Variable*> getAllIterationParameters();
        QVector<CalculationVariable*> getAllCompareParameters();

        QVector<CalibrationVariable*> getGroup(QString groupname);
        bool addParameterToGroup(QString groupname, CalibrationVariable* parameter);
        bool removeParameterFromGroup(QString groupname, QString parametername);
        bool newGroup(QString groupname);
        bool deleteGroup(QString groupname);
        bool deleteParameterFromAllGroups(QString parametername);
        QStringList getEvaluatingGroups();
        QStringList getNotEvaluatingGroups();
        bool addEvaluatingGroup(QString groupname);
        bool addNotEvaluatingGroup(QString groupname);
        bool removeEvaluatingGroup(QString groupname);
        bool removeNotEvaluatingGroup(QString groupname);
        void setClearIterations(bool clear);
        bool getClearIterations();
        void saveIteration(int index, bool saveall=true);
        void clearIterationResults();
        int numberOfIterations();
        int numberOfCompleteIterations();
        void setIterationResults(QVector<IterationResult*> *results);
        IterationResult* getIterationResult(int index, bool includenotfinished = false);



        QStringList getGroupNames();
        
        bool updateIterationResult(int index);
        bool createExternalInputFile(QString inputfilename, QString outputfilename, int index);
        bool createExternalFiles(int index);


        QString getPreExec();
        QString getPreExecArgument();
        QString getIterationExec();
        QString getIterationExecArgument();
        QString getPostExec();
        QString getPostExecArgument();
        QString getPreWorkspace();
        QString getPostWorkspace();
        QString getIterationWorkspace();

        void setPreExec(QString string);
        void setPreExecArgument(QString string);
        void setIterationExec(QString string);
        void setIterationExecArgument(QString string);
        void setPostExec(QString string);
        void setPostExecArgument(QString string);
        void setPreWorkspace(QString string);
        void setPostWorkspace(QString string);
        void setIterationWorkspace(QString string);

        int getCpus();
        void setCpus(int cpus);

        QVector<CalculationVariable*> getAlgParameters();
        bool addCalculationVariableToCalibration(QString name);
        bool removeCalculationVariableFromCalibration(QString name);

        QVector<CalibrationVariable*> getEvaluatingParameters();
        QVector<IterationResult*> getCompleteIterationResults();
        QMap<QString,QVector<QVector<double> > > getEvaluatingParametersResult();
        QMap<QString,QVector<QVector<double> > > getAlgParametersResult();
        bool modelLocked();


    signals:
        void modelchanged(Model* m);
        void erroroccured(QString message);

    public slots:
        void parameterchanged();
};

#endif // MODEL_H_INCLUDED
