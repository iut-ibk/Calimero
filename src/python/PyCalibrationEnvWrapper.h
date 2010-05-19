#ifndef PYCALIBRATIONENVWRAPPER_H
#define PYCALIBRATIONENVWRAPPER_H

#include <CalibrationEnv.h>

void wrapCalibrationEnv()
{
    class_<CalibrationEnv, boost::noncopyable>("CalibrationEnv", no_init)
            .def("startCalibration" , &CalibrationEnv::startCalibration)
            .def("stopCalibration" , &CalibrationEnv::stopCalibration)
            .def("setCalibration", &CalibrationEnv::setCalibration)
            .def("getCalibration", &CalibrationEnv::getCalibration,
                 return_internal_reference<1,
                         with_custodian_and_ward<1, 2> >())
            .def("isCalibrationRunning", &CalibrationEnv::isCalibrationRunning)
            .def("getNumThreads", &CalibrationEnv::getNumThreads)
            .def("setNumThreads", &CalibrationEnv::setNumThreads)
            .def("getObjectiveFunctionreg", &CalibrationEnv::getObjectiveFunctionReg,
                 return_internal_reference<1,
                         with_custodian_and_ward<1, 2> >())
            .def("getModelSimulatorReg", &CalibrationEnv::getModelSimulatorReg,
                 return_internal_reference<1,
                         with_custodian_and_ward<1, 2> >())
            .def("getCalibrationAlgReg", &CalibrationEnv::getCalibrationAlgReg,
                 return_internal_reference<1,
                         with_custodian_and_ward<1, 2> >())
            .def("execIteration", &CalibrationEnv::execIteration)
            ;
}


#endif // PYCALIBRATIONENVWRAPPER_H
