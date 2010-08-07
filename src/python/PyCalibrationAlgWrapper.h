#ifndef PYCALIBRATIONALGWRAPPER_H
#define PYCALIBRATIONALGWRAPPER_H

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <vector>
#include <iostream>
#include <ICalibrationAlg.h>
#include <PyEnv.h>
#include <CalibrationVariable.h>
#include <ObjectiveFunctionVariable.h>
#include <CalibrationEnv.h>
#include <Calibration.h>

using namespace boost::python;
using namespace std;

struct CalibrationAlgWrapper : public ICalibrationAlg, wrapper<ICalibrationAlg> {
    CalibrationAlgWrapper()
    {
    }

    virtual ~CalibrationAlgWrapper()
    {
    }

    bool start(vector<CalibrationVariable*> calibrationpars, vector<ObjectiveFunctionVariable*> opars,CalibrationEnv *env, Calibration *calibration)
    {
        ScopedGILRelease scoped;
        try {
                if (python::override f = this->get_override("start"))
                    return f(calibrationpars,opars, ptr(env), ptr(calibration));
                else
                        throw CalimeroException("No methode with name \"start\" found");
        }
        catch(python::error_already_set const &)
        {
                handle_python_exception("Error in start methode of a calibration algorithm");
                return false;
        }

        return false;
    }

    object self;
};

void  wrapCalAlgFunction()
{
    python::implicitly_convertible<auto_ptr<CalibrationAlgWrapper>, auto_ptr<ICalibrationAlg> >();
    class_<CalibrationAlgWrapper, bases<IFunction>, auto_ptr<CalibrationAlgWrapper>, boost::noncopyable>("ICalibrationAlg")
            .def("start", pure_virtual(&CalibrationAlgWrapper::start))
            ;
}

#endif // PYCALIBRATIONALGWRAPPER_H
