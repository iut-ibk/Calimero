#ifndef PYITERATIONRESULTWRAPPER_H
#define PYITERATIONRESULTWRAPPER_H

#include <IterationResult.h>
#include <boost/noncopyable.hpp>
#include <boost/python.hpp>

struct IterationResultWrapper: public IterationResult, public wrapper<IterationResult>
{
    IterationResultWrapper(int iterationnum): IterationResult(iterationnum){}
    IterationResultWrapper(int iterationnum,
                           map<string, vector<double> > calibrationparameters,
                           map<string, vector<double> > iterationparameters,
                           map<string, vector<double> > objectivefunctionparameters,
                           map<string, vector<double> > observedparameters): IterationResult(iterationnum,calibrationparameters,iterationparameters,objectivefunctionparameters,observedparameters){}
};

void wrapIterationResult()
{
    class_<IterationResult, shared_ptr<IterationResult>, boost::noncopyable>("__IterationResult", no_init)
            .def("setResults", &IterationResult::setResults)
            .def("isComplete" , &IterationResult::isComplete)
            .def("getIterationParameterResults" , &IterationResult::getIterationParameterResults)
            .def("getObservedParameterResults" , &IterationResult::getObservedParameterResults)
            .def("getCalibrationParameterResults" , &IterationResult::getCalibrationParameterResults)
            .def("getObjectiveFunctionParameterResults" , &IterationResult::getObjectiveFunctionParameterResults)
            .def("getIterationNumber", &IterationResult::getIterationNumber)
            ;

    class_<IterationResultWrapper, shared_ptr<IterationResultWrapper>, boost::noncopyable>("IterationResult", init<int>())
            .def(init<int ,
                 map<string, vector<double> >,
                 map<string, vector<double> >,
                 map<string, vector<double> >,
                 map<string, vector<double> > >())
            .def("setResults", &IterationResult::setResults)
            .def("isComplete" , &IterationResult::isComplete)
            .def("getIterationParameterResults" , &IterationResult::getIterationParameterResults)
            .def("getObservedParameterResults" , &IterationResult::getObservedParameterResults)
            .def("getCalibrationParameterResults" , &IterationResult::getCalibrationParameterResults)
            .def("getObjectiveFunctionParameterResults" , &IterationResult::getObjectiveFunctionParameterResults)
            .def("getIterationNumber", &IterationResult::getIterationNumber)
            ;
}

#endif // PYITERATIONRESULTWRAPPER_H
