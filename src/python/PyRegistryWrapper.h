#ifndef PYREGISTRYWRAPPER_H
#define PYREGISTRYWRAPPER_H

#include <Registry.h>
#include <IObjectiveFunction.h>
#include <ICalibrationAlg.h>
#include <IModelSimulator.h>

void wrapRegistry()
{
    class_<Registry<IObjectiveFunction> >("RegOFun")
            .def("addNativePlugin", &Registry<IObjectiveFunction>::addNativePlugin)
            .def("getSettingTypes", &Registry<IObjectiveFunction>::getSettingTypes)
            .def("contains",&Registry<IObjectiveFunction>::contains)
            .def("getAvailableFunctions", &Registry<IObjectiveFunction>::getAvailableFunctions)
            ;

    class_<Registry<ICalibrationAlg> >("RegAlgFun")
            .def("addNativePlugin", &Registry<ICalibrationAlg>::addNativePlugin)
            .def("getSettingTypes", &Registry<ICalibrationAlg>::getSettingTypes)
            .def("contains",&Registry<ICalibrationAlg>::contains)
            .def("getAvailableFunctions", &Registry<ICalibrationAlg>::getAvailableFunctions)
            ;

    class_<Registry<IModelSimulator> >("RegModelSimFun")
            .def("addNativePlugin", &Registry<IModelSimulator>::addNativePlugin)
            .def("getSettingTypes", &Registry<IModelSimulator>::getSettingTypes)
            .def("contains",&Registry<IModelSimulator>::contains)
            .def("getAvailableFunctions", &Registry<IModelSimulator>::getAvailableFunctions)
            ;
}
#endif // PYREGISTRYWRAPPER_H
