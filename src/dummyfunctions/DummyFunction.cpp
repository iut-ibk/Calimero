#include <DummyFunction.h>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <Logger.h>

using namespace std;

CALIMERO_DECLARE_OFUNCTION_NAME(DummyOFunction)

DummyOFunction::DummyOFunction()
{
    setDataType("fac n",UINT,"0");
    setDataType("iterations",UINT,"1000");
}

DummyOFunction::~DummyOFunction(){}

int DummyOFunction::fac(int x)
{
    if (x < 2)
    {
        return x;
    }
    return x*fac(x-1);
}

std::vector<double> DummyOFunction::eval(std::vector<Variable*> iterationparameters,
                                               std::vector<Variable*> observedparameters,
                                               std::vector<ObjectiveFunctionVariable*> objectivefunctionparameters)
{
    int facn = QString::fromStdString(getValueOfParameter("fac n")).toInt();
    int iterations = QString::fromStdString(getValueOfParameter("iterations")).toInt();

    int tmp = 0;

    for(int index=0; index < iterations; index++)
        tmp=fac(facn);

    std::vector<double> result;
    result.push_back(tmp+1);

    return result;
}
