#ifndef PYDOMAINWRAPPER_H
#define PYDOMAINWRAPPER_H

#include <Domain.h>
#include <boost/python.hpp>

void wrapDomain()
{
    class_<Domain >("Domain")
            .def("getPar", &Domain::getPar,
                 return_internal_reference<1,
                         with_custodian_and_ward<1, 2> >())
            .def("contains", &Domain::contains)
            .def("setPar", &Domain::setPar)
            .def("removePar", &Domain::removePar)
            .def("getAllPars", &Domain::getAllPars)
            ;
}

#endif // PYDOMAINWRAPPER_H
