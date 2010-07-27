#include <TestModel.h>
#include <Logger.h>
#include <Domain.h>
#include <CalibrationVariable.h>
#include <IterationResult.h>

CALIMERO_DECLARE_MODELSIMULATOR_NAME(TestModel)

TestModel::TestModel(){}

bool TestModel::exec(Domain *dom)
{
    CalibrationVariable* pi = static_cast<CalibrationVariable*>(dom->getAllPars(CALIBRATIONVARIABLE).at(0));
    Variable* iterationresult = dom->getAllPars(ITERATIONVARIABLE).at(0);
    vector<double> result = iterationresult->getValues();
    result[0]=pi->getValues()[0]*5;
    iterationresult->setValues(result);
    return true;
}

CALIMERO_DECLARE_MODELSIMULATOR_NAME(VectorModel)

VectorModel::VectorModel(){}

bool VectorModel::exec(Domain *dom)
{
    //extract calibration parameters
    if(dom->getAllPars(CALIBRATIONVARIABLE).size() != 2)
        return false;

    double value = 0.0;
    double size = 0.0;

    for(int index = 0; index < 2; index++)
    {
        CalibrationVariable* currentparameter = static_cast<CalibrationVariable*>(dom->getAllPars(CALIBRATIONVARIABLE).at(index));
        vector<double> currentvector = currentparameter->getValues();
        if(!currentvector.size())
            return false;

        double currentvalue = currentvector[0];
        if(currentparameter->getName()=="value")
            value = currentvalue;
        else
            size = currentvalue;
    }

    //extract result container
    if(!dom->getAllPars(ITERATIONVARIABLE).size())
        return false;

    Variable* iterationresult = dom->getAllPars(ITERATIONVARIABLE).at(0);

    //start calculation
    vector<double> result;

    for(int index = 0; index < size; index++)
        result.push_back((index+1)*(index+1)*value);

    iterationresult->setValues(result);
    return true;
}
