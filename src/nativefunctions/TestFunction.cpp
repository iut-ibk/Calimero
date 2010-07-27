#include <TestFunction.h>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <Logger.h>

using namespace std;

CALIMERO_DECLARE_OFUNCTION_NAME(TestOFunction)

TestOFunction::TestOFunction(){}

TestOFunction::~TestOFunction(){}

std::vector<double> TestOFunction::eval(std::vector<Variable*> iterationparameters,
                                               std::vector<Variable*> observedparameters,
                                               std::vector<ObjectiveFunctionVariable*> objectivefunctionparameters)
{
    Logger(Debug) << "eval called in native class \"TestOFunction\"";
    return std::vector<double>();
}
