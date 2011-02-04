#include <DummyModel.h>
#include <Logger.h>
#include <Domain.h>
#include <CalibrationVariable.h>
#include <IterationResult.h>
#include <QString>

CALIMERO_DECLARE_MODELSIMULATOR_NAME(DummyModel)

DummyModel::DummyModel()
{
    setDataType("fac n",UINT,"0");
    setDataType("iterations",UINT,"1000");
}

int DummyModel::fac(int x) {
    if (x < 2)
    {
        return x;
    }
    return x*fac(x-1);
}

bool DummyModel::exec(Domain *dom)
{
    //extract result container
    if(!dom->getAllPars(ITERATIONVARIABLE).size())
        return false;

    Variable* iterationresult = dom->getAllPars(ITERATIONVARIABLE).at(0);

    //start calculation
    vector<double> result;

    int facn = QString::fromStdString(getValueOfParameter("fac n")).toInt();
    int iterations = QString::fromStdString(getValueOfParameter("iterations")).toInt();

    int tmp = 0;

    for(int index=0; index < iterations; index++)
        tmp=fac(facn);


    result.push_back(tmp+1);
    iterationresult->setValues(result);

    return true;
}
