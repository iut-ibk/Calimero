#ifndef PYCALIBRATIONALGWRAPPER_H
#define PYCALIBRATIONALGWRAPPER_H

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <vector>
#include <iostream>
#include <ICalibrationAlg.h>
#include <PyEnv.h>

using namespace boost::python;
using namespace std;


struct CalibrationAlgWrapper : ICalibrationAlg, wrapper<ICalibrationAlg> {
    CalibrationAlgWrapper(PyObject *self)
    {
        this->self = self;
        Py_INCREF(self);
    }

    virtual ~CalibrationAlgWrapper()
    {
        Py_DECREF(self);
    }

    bool start(vector<CalibrationVariable*> calibrationpars, vector<ObjectiveFunctionVariable*> opars, CalibrationEnv *env, Calibration *calibration)
    {
        try {
                if (python::override f = this->get_override("start")) {
                        return f(calibrationpars,opars,env,calibration);
                } else {
                        //TODO do something here now reason there is no f method
                }
        } catch(python::error_already_set const &) {
                Logger(Error) << __FILE__ << ":" << __LINE__;
                handle_python_exception();
        }

        return false;
    }

private:
    PyObject *self;
};

void wrapCalAlgFunction()
{
    python::implicitly_convertible<auto_ptr<CalibrationAlgWrapper>, auto_ptr<ICalibrationAlg> >();
    class_<ICalibrationAlg, bases<IFunction>, auto_ptr<CalibrationAlgWrapper>, boost::noncopyable>("ICalibrationAlg")
            .def("start", pure_virtual(&CalibrationAlgWrapper::start))
            ;
}
#endif // PYCALIBRATIONALGWRAPPER_H
