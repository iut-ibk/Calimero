#ifndef PYCALIBRATIONWRAPPER_H
#define PYCALIBRATIONWRAPPER_H

#include <Calibration.h>

void wrapCalibration()
{
        class_<Calibration>("Calibration")
                .def("setCalibrationAlg", &Calibration::setCalibrationAlg)
                .def("setModelSimulator", &Calibration::setModelSimulator)
                .def("addParameter", &Calibration::addParameter)
                .def("removeParameter", &Calibration::removeParameter)
                .def("addGroup", &Calibration::addGroup)
                .def("removeGroup", &Calibration::removeGroup)
                .def("addParameterToGroup", &Calibration::addParameterToGroup)
                .def("removeParameterFromGroup", &Calibration::removeParameterFromGroup)
                .def("addEnabledGroup", &Calibration::addEnabledGroup)
                .def("removeEnabledGroup", &Calibration::removeEnabledGroup)
                .def("addDisabledGroup", &Calibration::addDisabledGroup)
                .def("removeDisabledGroup", &Calibration::removeDisabledGroup)
                .def("addEnabledOParameter", &Calibration::addEnabledOParameter)
                .def("removeEnabledOParameter", &Calibration::removeEnabledOParameter)
                .def("newIterationResult", &Calibration::newIterationResult ,
                     return_internal_reference<1,
                        with_custodian_and_ward<1, 2> >())
                .def("containsGroup", &Calibration::containsGroup)
                .def("containsParameter", &Calibration::containsParameter)
                .def("getNumOfComplete", &Calibration::getNumOfComplete)
                .def("getCalibrationAlg", &Calibration::getCalibrationAlg)
                .def("getModelSimulator", &Calibration::getModelSimulator)
                .def("getCalibrationAlgSettings", &Calibration::getCalibrationAlgSettings)
                .def("getModelSimulatorSettings", &Calibration::getModelSimulatorSettings)
                .def("getIterationResults", &Calibration::getIterationResults)
                .def("getDomain", &Calibration::getDomain,
                     return_internal_reference<1,
                        with_custodian_and_ward<1, 2> >())
                .def("clear", &Calibration::clear)
                .def("clearIterationResults", &Calibration::clearIterationResults)
                .def("getAllObjectiveFunctionParameters", &Calibration::getAllObjectiveFunctionParameters)
                .def("getAllIterationParameters", &Calibration::getAllIterationParameters)
                .def("getAllObservedParameters", &Calibration::getAllObservedParameters)
                .def("getAllCalibrationParameters", &Calibration::getAllCalibrationParameters)
                ;
}

#endif // PYCALIBRATIONWRAPPER_H
