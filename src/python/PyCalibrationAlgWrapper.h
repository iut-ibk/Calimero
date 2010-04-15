#ifndef PYCALIBRATIONALGWRAPPER_H
#define PYCALIBRATIONALGWRAPPER_H

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <vector>
#include <iostream>
#include <ICalibrationAlg.h>


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

    bool start()
    {
        return call_method<bool>(self, "start");
    }

private:
    PyObject *self;
};

void wrapCalAlgFunction()
{
        class_<ICalibrationAlg, bases<IFunction>, auto_ptr<CalibrationAlgWrapper>, boost::noncopyable>("ICalibrationAlg")
                .def("start", pure_virtual(&CalibrationAlgWrapper::start))
                ;
}
#endif // PYCALIBRATIONALGWRAPPER_H
