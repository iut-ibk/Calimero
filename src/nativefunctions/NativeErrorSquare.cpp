#include <NativeErrorSquare.h>
#include <vector>
#include <Variable.h>
#include <ObjectiveFunctionVariable.h>

CALIMERO_DECLARE_OFUNCTION_NAME(NativeErrorSquare)

NativeErrorSquare::NativeErrorSquare(){}

NativeErrorSquare::~NativeErrorSquare(){}

std::vector<double> NativeErrorSquare::eval(std::vector<Variable*> iterationparameters,
                             std::vector<Variable*> observedparameters,
                             std::vector<ObjectiveFunctionVariable*> objectivefunctionparameters)

{
    std::vector<double> result;

    //extract vectors
    std::vector<std::vector<double> > v;

    for (uint index=0; index < iterationparameters.size(); index++)
        v.push_back(iterationparameters.at(index)->getValues());

    for (uint index=0; index<observedparameters.size(); index++)
        v.push_back(observedparameters.at(index)->getValues());

    for (uint index=0; index<objectivefunctionparameters.size(); index++)
        v.push_back(objectivefunctionparameters.at(index)->getValues());

    //check number of vectors
    if(v.size()!=2)
    {
        Logger(Error) << "Only two parameters are allowed in NativeErrorSquare function";
        result.push_back(999999999);
        return result;
    }

    //check length of vectors
    if(v[0].size() != v[1].size())
    {
        Logger(Error) << "Length of vectors must be equal";
        result.push_back(999999999);
        return result;
    }

    //calculate result vector
    for(uint index=0; index < v.at(0).size(); index++)
    {
        double v1 = v[0][index];
        double v2 = v[1][index];

        result.push_back((v1-v2)*(v1-v2));
    }

    return result;
}
