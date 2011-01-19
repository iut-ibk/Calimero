%module(directors="1", allprotected="1") pycalimero
%feature("director");
%{
#include <IObjectiveFunction.h>
#include <IFunction.h>
#include <Variable.h>
#include <ObjectiveFunctionVariable.h>
#include <CalibrationVariable.h>
#include <ICalibrationAlg.h>
#include <Domain.h>
#include <IModelSimulator.h>
#include <IResultHandler.h>
#include <IFunctionFactory.h>
#include <Registry.h>
#include <Exception.h>
#include <IterationResult.h>
#include <Calibration.h>
#include <CalibrationEnv.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;
%}



%include std_vector.i
%include std_string.i
%include std_map.i
%include pointer.i
#include typemaps.i


namespace std {
    %template(stringvector) vector<string>;
    %template(doublevector) vector<double>;
    %template(doublevectorvector) vector<vector<double> >;
    %template(integervector) vector<int>;
    %template(variablevector) vector<Variable*>;
    %template(objectivefunctionvariablevector) vector<ObjectiveFunctionVariable*>;
    %template(iterationresultvector) vector<shared_ptr<IterationResult> >;
    %template(calibrationvariablevector) vector<CalibrationVariable*>;
    %template(stringvectormap) map<string,vector<double> >;
    %template(stringmap) map<string,string>;
    %template(datatypemap) map<int,DATATYPE>;
    %template(datatypmap) map<string, DATATYPE>;
}

%feature("director:except") {
	if ($error != NULL) {
                PyErr_Print();
                throw CalimeroException("======================================");
	}
}

enum IFUNCTIONTYPE { NOTYPE = -1,
                     OBJECTIVEFUNCTION = 0,
                     CALIBRATIONALGORITHM = 1,
                     MODELSIMULATOR = 2,
                     RESULTFUNCTION = 3
                    };

enum DATATYPE { STRING = 1,
                DOUBLE = 2,
                INT = 3,
                BOOL = 4,
                UINT = 5,
                FILESTRING = 6,
                DIRSTRING = 7
              };


enum  VARTYPE {CALIBRATIONVARIABLE = 0,
               OBJECTIVEFUNCTIONVARIABLE = 1,
               OBSERVEDVARIABLE = 2,
               ITERATIONVARIABLE = 3,
              };


class IFunction {
public:
    IFunction();
    virtual ~IFunction();
    map<std::string, DATATYPE > getDataTypes();


    map< std::string,std::string > getParameterValues();

    bool setDataType(std::string name, DATATYPE type, std::string value);

    bool containsParameter(std::string name);

    std::string getValueOfParameter(std::string name);

    bool setValues(map<std::string,std::string> values);
};

%extend IFunction {
    static IFUNCTIONTYPE getType() {
            return NOTYPE;
    }
}



/*%extend {

void barrier()
{
    PyEnv::getInstance()->barrier();
}

bool execIteration(vector<CalibrationVariable*> cp)
{
    return PyEnv::getInstance()->execIteration(cp);
}
}*/

class Domain
{
public:
    Domain();
    Domain(const Domain &olddomain);
    ~Domain();
    Variable* getPar(const std::string &name);
    bool contains(string var);
    void setPar(Variable* var);
    bool removePar(const string &name);
    vector<Variable*> getAllPars(const VARTYPE &type);
};

class Calibration
{
public:
    //create
    Calibration();
    ~Calibration();

    //setup
    bool setCalibrationAlg(std::string ca, map<std::string,std::string> settings);
    bool setModelSimulator(std::string ms, map<std::string,std::string> settings);
    bool addParameter(Variable *parameter);
    bool removeParameter(std::string parameter);
    bool addGroup(std::string name);
    bool removeGroup(std::string name);
    bool addParameterToGroup(std::string var, std::string groupname);
    bool removeParameterFromGroup(std::string var, std::string groupname);
    bool addEnabledGroup(std::string groupname);
    bool removeEnabledGroup(std::string groupname);
    bool addDisabledGroup(std::string groupname);
    bool removeDisabledGroup(std::string groupname);
    bool addEnabledOParameter(std::string parameter);
    bool removeEnabledOParameter(std::string parameter);
    bool setIterationResults(map<int,shared_ptr<IterationResult>  > iterationresults);
    bool addResultHandler(std::string name, std::string functionname, map<std::string,std::string> settings, bool enabled);
    bool removeResultHandler(std::string name);
    shared_ptr<IterationResult>   newIterationResult();

    //contains
    bool containsGroup(std::string groupname);
    bool containsParameter(std::string var);
    bool containsGroupMember(std::string varname, std::string groupname);

    //getter
    int getNumOfComplete();
    std::string getCalibrationAlg();
    map<std::string, std::string> getResultHandlers();
    map<std::string, std::string> getResultHandlerSettings(std::string name);
    bool isResultHandlerEnabled(std::string name);
    std::string getModelSimulator();
    map<std::string,std::string> getCalibrationAlgSettings();
    map<std::string,std::string> getModelSimulatorSettings();
    vector<shared_ptr<IterationResult>  > getIterationResults();
    vector<std::string> getAllCalibrationParameters();
    vector<std::string> getAllObservedParameters();
    vector<std::string> getAllIterationParameters();
    vector<std::string> getAllObjectiveFunctionParameters();
    Domain* getDomain();
    set<std::string> evalCalibrationParameters();
    set<std::string> evalObjectiveFunctionParameters();
    ExternalParameterRegistry* getExternalParameterRegistry();
    vector<std::string> getAllGroups();
    map<std::string,bool> getDisabledGroups();
    map<std::string,bool> getEnabledGroups();
    set<std::string> getGroupMembers(const std::string &name);
    bool isEnabledGroup(const std::string &name);
    bool isDisabledGroup(const std::string &name);

    //destroy
    void clear();
    void clearIterationResults();
};

class IterationResult
{
public:
    IterationResult(int iterationnum);
    IterationResult(int iterationnum,
                    map<std::string, vector<double> > calibrationparameters,
                    map<std::string, vector<double> > iterationparameters,
                    map<std::string, vector<double> > objectivefunctionparameters,
                    map<std::string, vector<double> > observedparameters);
    void setResults(Domain *dom);
    bool isComplete();
    int getIterationNumber();
    vector<double> getResults(std::string name);
    vector<double> getIterationParameterResults(std::string name);
    vector<double> getObservedParameterResults(std::string name);
    vector<double> getCalibrationParameterResults(std::string name);
    vector<double> getObjectiveFunctionParameterResults(std::string name);
    vector<std::string> getNamesOfObjectiveFunctionParameters();
    vector<std::string> getNamesOfObservedParameters();
    vector<std::string> getNamesOfCalibrationParameters();
    vector<std::string> getNamesOfIterationParameters();
};

class CalibrationEnv
{
    bool startCalibration();
    void stopCalibration();
};



%newobject CalibrationEnv::getInstance;

%extend IObjectiveFunction {
    static IFUNCTIONTYPE getType() {
            return OBJECTIVEFUNCTION;
    }
}

class IObjectiveFunction : public IFunction
{
public:

    IObjectiveFunction();
    virtual ~IObjectiveFunction();

    virtual vector<double> eval(vector<Variable*> iterationparameters,
                                     vector<Variable*> observedparameters,
                                     vector<ObjectiveFunctionVariable*> objectivefunctionparameters) = 0;
};

%extend IObjectiveFunction {
    static IFUNCTIONTYPE getType() {
            return OBJECTIVEFUNCTION;
    }
}

class Variable
{
    public:
        Variable(std::string Name, vector<double> value, VARTYPE TYPE);
        Variable(const Variable &oldvar);
        virtual ~Variable();
        virtual vector<double> getValues();
        virtual bool setValues(const vector<double> values);
        virtual bool addSuccessor(const std::string &var);
        virtual bool removeSuccessor(const std::string &var);
        virtual void fireUpdate();
        std::string getName() const;
        VARTYPE getType() const;
        void setDomain(Domain* dom);
};

class ObjectiveFunctionVariable : public Variable
{
    public:
        ObjectiveFunctionVariable(const ObjectiveFunctionVariable &oldvar);
        ObjectiveFunctionVariable(std::string Name);
        virtual ~ObjectiveFunctionVariable();
        bool addParameter(const std::string &var);
        bool removeParameter(const std::string &var);
        set<std::string> getIterationParameters();
        set<std::string> getObservedParameters();
        set<std::string> getObjectiveFunctionParameters();
        vector<double> getValues();
        virtual void fireUpdate();
        bool setObjectiveFunction(std::string ofunction, map<std::string,std::string> settings);
        std::string getObjectiveFunction();
        bool containsParameter(const std::string &var);
        map<std::string,std::string> getObjectiveFunctionSettings();
        bool parameterCycleCheck(std::string var);
};

class CalibrationVariable : public Variable
{
    public:
        CalibrationVariable(std::string Name, vector<double> value);
        CalibrationVariable(const CalibrationVariable &oldvar);
        virtual ~CalibrationVariable();
        vector<double> getInitValues();
        void setInitValues(vector<double> value);
        bool setValues( vector<double> vec);
        void setMax(double value);
        double getMax();
        void setMin(double value);
        double getMin();
        double getStep();
        void setStep(double step);
};

enum LogLevel {
	Debug = 0,
	Standard = 1,
	Warning = 2,
	Error = 3
};

%inline %{
void log(std::string s, LogLevel l) {
	Logger(l) << s;
}
%}

class ICalibrationAlg : public IFunction
{
    public:
        ICalibrationAlg(){};
        virtual bool start(vector<CalibrationVariable*> calibrationpars, vector<ObjectiveFunctionVariable*> opars, CalibrationEnv *env, Calibration *calibration) = 0;
};

%extend ICalibrationAlg {
        static IFUNCTIONTYPE getType()
        {
            return CALIBRATIONALGORITHM;
        };
}

class IResultHandler : public IFunction
{
    public:

        IResultHandler(){};
        virtual bool run(std::vector<shared_ptr<IterationResult> > iterationresults) = 0;
        bool test(){Logger(Error) << "test"; return false;};
};

%extend IResultHandler {
        static IFUNCTIONTYPE getType()
        {
            return RESULTFUNCTION;
        };
}

%rename(execModel) IModelSimulator::exec(Domain*);
class IModelSimulator : public IFunction
{
    public:
        IModelSimulator(){};


        virtual bool exec(Domain *dom) = 0;
};


%extend IModelSimulator {
        static IFUNCTIONTYPE getType()
        {
            return MODELSIMULATOR;
        };
}

class IRegistry {
    public:
        virtual ~IRegistry(){}
        IFUNCTIONTYPE getType();
        virtual bool registerFunction(IFunctionFactory* factory) = 0;
        virtual bool addNativePlugin(const std::string &plugin_path) = 0;
        virtual IFunction* getFunction(std::string name) = 0;
        virtual map<std::string,DATATYPE> getSettingTypes(std::string name) = 0;
        virtual bool contains(std::string name) = 0;
        virtual vector<std::string> getAvailableFunctions() = 0;
};

class IFunctionFactory {
    public:
        virtual ~IFunctionFactory(){}
        virtual IFunction *createFunction() const = 0;
        virtual std::string getFunctionName() = 0;
};

%pythoncode %{
class FunctionFactory(IFunctionFactory):
    def __init__(self, klass):
        IFunctionFactory.__init__(self)
        self.klass = klass

    def createFunction(self):
        return self.klass().__disown__()

    def getFunctionName(self):
        return self.klass.__name__

def registerFunctions(registry):
    FUNCTION_CLASSES = [IObjectiveFunction, ICalibrationAlg, IModelSimulator, IResultHandler]
    func_type_klass = FUNCTION_CLASSES[registry.getType()]
    for klass in func_type_klass.__subclasses__():
        registry.registerFunction(FunctionFactory(klass).__disown__())
%}
