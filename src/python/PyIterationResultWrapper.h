#ifndef PYITERATIONRESULTWRAPPER_H
#define PYITERATIONRESULTWRAPPER_H

#include <IterationResult.h>

void wrapIterationResult()
{
        class_<IterationResult, boost::noncopyable>("IterationResult",init<int>())
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
