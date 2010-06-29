#ifndef PYCALIBRATIONENVWRAPPER_H
#define PYCALIBRATIONENVWRAPPER_H

#include <CalibrationEnv.h>
#include <boost/python.hpp>

void wrapCalibrationEnv()
{
    class_<CalibrationEnv, boost::noncopyable>("CalibrationEnv", no_init)
            .def("getInstance", &CalibrationEnv::getInstance,return_value_policy<reference_existing_object>())
            .staticmethod("getInstance")
            .def("startCalibration" , &CalibrationEnv::startCalibration)
            .def("stopCalibration" , &CalibrationEnv::stopCalibration)
            .def("setCalibration", &CalibrationEnv::setCalibration)
            .def("getCalibration", &CalibrationEnv::getCalibration,return_value_policy<reference_existing_object>())
            .def("isCalibrationRunning", &CalibrationEnv::isCalibrationRunning)
            .def("getNumThreads", &CalibrationEnv::getNumThreads)
            .def("setNumThreads", &CalibrationEnv::setNumThreads)
            .def("getObjectiveFunctionreg", &CalibrationEnv::getObjectiveFunctionReg, return_value_policy<reference_existing_object>())
            .def("getModelSimulatorReg", &CalibrationEnv::getModelSimulatorReg, return_value_policy<reference_existing_object>())
            .def("getCalibrationAlgReg", &CalibrationEnv::getCalibrationAlgReg, return_value_policy<reference_existing_object>())
            .def("execIteration", &CalibrationEnv::execIteration)
            .def("barrier", &CalibrationEnv::barrier)
            ;
}


#endif // PYCALIBRATIONENVWRAPPER_H
