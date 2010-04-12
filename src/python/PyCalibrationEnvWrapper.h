#ifndef PYCALIBRATIONENVWRAPPER_H
#define PYCALIBRATIONENVWRAPPER_H

#include <CalibrationEnv.h>

void wrapCalibrationEnv()
{
    //scope().attr("CalibrationEnv") = &CalibrationEnv::getInstance();
    class_<CalibrationEnv, boost::noncopyable>("CalibrationEnv", no_init)
            .def("startCalibration" , &CalibrationEnv::startCalibration)
            .def("stopCalibration" , &CalibrationEnv::stopCalibration)
            ;
}


#endif // PYCALIBRATIONENVWRAPPER_H
